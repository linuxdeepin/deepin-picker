#!/bin/bash

################################################################################
# Unit Test Runner Script
# 一键流程：编译 → 直接运行 gtest 二进制(per-target XML) → lcov 覆盖率 → genhtml → ut-summary.json
# 特性：ASAN/LSan 泄漏检测、CMAKE_SAFETYTEST_ARG、headless(Qt offscreen)、--from-step 断点续跑
################################################################################

set -e

# 测试目录名（框架搭建时确定）
TEST_DIR="autotests"

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_step()    { echo -e "${BLUE}[STEP $1]${NC} $2"; }
print_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
print_error()   { echo -e "${RED}[ERROR]${NC} $1"; }
print_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
print_info()    { echo -e "${BLUE}[INFO]${NC} $1"; }

show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --from-step <N>   Start from step N (1-6)"
    echo "  --parallel <N>    Parallel build jobs (default: $(nproc))"
    echo "  -h, --help        Show this help"
    echo ""
    echo "Steps:"
    echo "  1. Prepare build env"
    echo "  2. Configure CMake"
    echo "  3. Compile tests"
    echo "  4. Run unit tests (per-target gtest XML)"
    echo "  5. Generate coverage report (lcov + genhtml)"
    echo "  6. Generate ut-summary.json"
    echo ""
    echo "Examples:"
    echo "  $0                 # Run all steps"
    echo "  $0 --from-step 4   # Skip build, run tests + coverage"
    echo "  $0 --from-step 6   # Only regenerate summary from existing data"
}

# Parse args
START_STEP=1
PARALLEL_JOBS=$(nproc)
while [[ $# -gt 0 ]]; do
    case $1 in
        --from-step)
            START_STEP="$2"
            if ! [[ "$START_STEP" =~ ^[1-6]$ ]]; then
                print_error "Invalid step: $START_STEP (must be 1-6)"
                exit 1
            fi
            shift 2 ;;
        --parallel)
            PARALLEL_JOBS="$2"
            if ! [[ "$PARALLEL_JOBS" =~ ^[1-9][0-9]*$ ]]; then
                print_error "Invalid parallel: $PARALLEL_JOBS (positive integer)"
                exit 1
            fi
            shift 2 ;;
        -h|--help) show_usage; exit 0 ;;
        *) print_error "Unknown option: $1"; show_usage; exit 1 ;;
    esac
done

# Directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build-${TEST_DIR}"
REPORT_DIR="$BUILD_DIR/test-reports"
GTEST_XML_DIR="$BUILD_DIR/report"

# Headless Qt (CI 友好)
export QT_QPA_PLATFORM=${QT_QPA_PLATFORM:-offscreen}

# ASAN/LSan：启用泄漏检测 + 抑制 Qt/DTK 框架误报
export ASAN_OPTIONS=${ASAN_OPTIONS:-detect_leaks=1}
export LSAN_OPTIONS=suppressions="$SCRIPT_DIR/lsan_suppressions.txt"

# State
TEST_PASSED=false
TEST_EXIT_CODE=0
COVERAGE_SUCCESS=false

echo "========================================"
echo "  Unit Test Runner"
echo "========================================"
echo "Project root:  $PROJECT_ROOT"
echo "Build dir:     $BUILD_DIR"
echo "Reports:       $REPORT_DIR"
echo "Parallel jobs: $PARALLEL_JOBS"
[ "$START_STEP" -gt 1 ] && print_info "Starting from step $START_STEP"
echo ""

step_1_prepare_build_env() {
    print_step 1 "Preparing build environment..."
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR" "$REPORT_DIR"
    print_success "Build environment prepared"
}

step_2_configure_cmake() {
    print_step 2 "Configuring CMake..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    # 注意：cmake 源指向项目根（CMAKE_SOURCE_DIR=项目根），而非 autotests 目录
    cmake "$PROJECT_ROOT" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DBUILD_TESTS=ON \
        -DCMAKE_SAFETYTEST_ARG="CMAKE_SAFETYTEST_ARG_ON"
    print_success "CMake configuration completed"
}

step_3_compile_tests() {
    print_step 3 "Compiling tests..."
    cd "$BUILD_DIR"
    cmake --build . -j "$PARALLEL_JOBS"
    print_success "Compilation completed"
}

# 发现 gtest 二进制：优先 {test_dir}/src，回退到 build 目录递归
discover_test_binaries() {
    local found
    found=$(find "$BUILD_DIR/$TEST_DIR/src" -maxdepth 1 -type f -executable -name "test_*" 2>/dev/null | sort)
    if [ -z "$found" ]; then
        found=$(find "$BUILD_DIR" -maxdepth 4 -type f -executable -name "test_*" -not -name "*.so" -not -name "*.a" -not -name "*.cmake" 2>/dev/null | sort)
    fi
    echo "$found"
}

step_4_run_tests() {
    print_step 4 "Running unit tests..."
    mkdir -p "$GTEST_XML_DIR"
    TEST_START_TIME=$(date +%s)
    TEST_PASSED=true
    TEST_EXIT_CODE=0

    local binaries
    binaries=$(discover_test_binaries)
    if [ -z "$binaries" ]; then
        print_error "No test binaries found under $BUILD_DIR (did you compile?)"
        TEST_PASSED=false
        TEST_EXIT_CODE=1
        return
    fi

    while IFS= read -r binary; do
        [ -z "$binary" ] && continue
        local target
        target=$(basename "$binary")
        echo "==> Running $target ..."
        set +e
        "$binary" --gtest_output="xml:$GTEST_XML_DIR/report_${target}.xml"
        local ec=$?
        set -e
        if [ $ec -ne 0 ]; then
            print_error "$target FAILED (exit $ec)"
            TEST_PASSED=false
            TEST_EXIT_CODE=$ec
        else
            print_success "$target PASSED"
        fi
    done <<< "$binaries"

    TEST_END_TIME=$(date +%s)
    print_info "Test execution completed in $((TEST_END_TIME - TEST_START_TIME))s"
}

step_5_generate_coverage() {
    print_step 5 "Generating coverage report..."
    if ! command -v lcov &> /dev/null; then
        print_warning "lcov not installed, skipping coverage"
        COVERAGE_SUCCESS=false
        return
    fi

    set +e
    mkdir -p "$BUILD_DIR/html"
    # 采集 → 仅保留业务源码 → 排除测试/第三方 → genhtml
    lcov -d "$BUILD_DIR" -c -o "$BUILD_DIR/coverage.info" > "$REPORT_DIR/coverage_output.log" 2>&1 || true
    lcov --extract "$BUILD_DIR/coverage.info" '*/src/*' -o "$BUILD_DIR/coverage.info" >> "$REPORT_DIR/coverage_output.log" 2>&1 || true
    lcov --remove "$BUILD_DIR/coverage.info" '*/test*' '*/'"${TEST_DIR}"'/*' '*/3rdparty/*' -o "$BUILD_DIR/coverage.info" >> "$REPORT_DIR/coverage_output.log" 2>&1 || true
    genhtml -o "$BUILD_DIR/html" --title "Coverage Report" --show-details --legend "$BUILD_DIR/coverage.info" >> "$REPORT_DIR/coverage_output.log" 2>&1
    local genhtml_rc=$?
    if [ $genhtml_rc -eq 0 ] && [ -s "$BUILD_DIR/coverage.info" ]; then
        print_success "Coverage report generated"
        COVERAGE_SUCCESS=true
        print_success "Coverage HTML: file://$BUILD_DIR/html/index.html"
    else
        print_warning "Coverage report generation failed (see $REPORT_DIR/coverage_output.log)"
        COVERAGE_SUCCESS=false
    fi
    set -e
}

step_6_generate_summary() {
    print_step 6 "Generating ut-summary.json..."
    if ! command -v python3 &> /dev/null; then
        print_warning "Python 3 not installed, skipping summary"
        return
    fi

    # 轻量解析：从已有 gtest XML + lcov summary 生成 JSON，不重跑测试
    export projectdir="$PROJECT_ROOT"
    export builddir="build-${TEST_DIR}"
    export reportdir="build-${TEST_DIR}"
    export GTEST_XML_DIR="$GTEST_XML_DIR"
    export COVERAGE_INFO="$BUILD_DIR/coverage.info"

    if python3 "$SCRIPT_DIR/gen-ut-summary.py"; then
        print_success "Summary: file://$BUILD_DIR/ut-summary.json"
        # 若从 step 6 起，依据 summary 推断测试是否通过
        if [ "$START_STEP" -eq 6 ]; then
            local failed
            failed=$(python3 -c "import json;print(json.load(open('$BUILD_DIR/ut-summary.json'))['test_cases']['failed'])" 2>/dev/null || echo 1)
            [ "$failed" = "0" ] && TEST_PASSED=true || TEST_PASSED=false
        fi
    else
        print_warning "Summary generation failed"
    fi
}

# Execute steps based on START_STEP
case $START_STEP in
    1) step_1_prepare_build_env; step_2_configure_cmake; step_3_compile_tests; step_4_run_tests; step_5_generate_coverage; step_6_generate_summary ;;
    2) mkdir -p "$BUILD_DIR" "$REPORT_DIR"; step_2_configure_cmake; step_3_compile_tests; step_4_run_tests; step_5_generate_coverage; step_6_generate_summary ;;
    3) mkdir -p "$BUILD_DIR" "$REPORT_DIR"; step_3_compile_tests; step_4_run_tests; step_5_generate_coverage; step_6_generate_summary ;;
    4) mkdir -p "$REPORT_DIR"; step_4_run_tests; step_5_generate_coverage; step_6_generate_summary ;;
    5) mkdir -p "$REPORT_DIR"; step_5_generate_coverage; step_6_generate_summary ;;
    6) mkdir -p "$REPORT_DIR"; step_6_generate_summary ;;
esac

# 收集 ASAN 日志（若存在；无崩溃时不产生）
cp "$BUILD_DIR"/asan*.log* "$REPORT_DIR/asan.log" 2>/dev/null || true

echo ""
echo "========================================"
if [ "$TEST_PASSED" = true ]; then
    print_success "Unit test execution completed!"
else
    print_error "Unit tests have failures"
fi
echo ""
echo "Generated artifacts:"
echo "  gtest XML:     $GTEST_XML_DIR/"
echo "  coverage HTML: $BUILD_DIR/html/index.html"
echo "  summary JSON:  $BUILD_DIR/ut-summary.json"
echo "  logs:          $REPORT_DIR/"
echo "========================================"

if [ "$TEST_PASSED" != true ]; then
    exit 1
fi
