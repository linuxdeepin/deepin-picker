cmake_minimum_required(VERSION 3.16)

# Qt version
if(Qt6Widgets_FOUND)
    set(QT_VERSION_MAJOR 6)
else()
    set(QT_VERSION_MAJOR 5)
endif()

# 代码覆盖率
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov")
endif()

# 项目源码（被测代码直接编入测试目标，不链接 .a/.so）
file(GLOB PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
# 排除 main.cpp（含 main 函数）
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

# stub-ext
set(STUB_SHADOW "${CMAKE_SOURCE_DIR}/autotests/3rdparty/stub/stub-shadow.cpp")

# 收集所有测试文件
file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")

if(NOT TEST_SOURCES)
    message(WARNING "No test files found in ${CMAKE_CURRENT_SOURCE_DIR}")
    return()
endif()

# 每个测试文件独立目标
foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    string(REPLACE "test_" "" CLASS_NAME ${TEST_NAME})

    add_executable(${TEST_NAME}
        ${TEST_FILE}
        ${PROJECT_SOURCES}
        ${STUB_SHADOW}
    )

    target_link_libraries(${TEST_NAME}
        PRIVATE
        GTest::gtest
        GTest::gtest_main
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::DBus
    )

    # DTK
    if(DTK_FOUND)
        target_link_libraries(${TEST_NAME} PRIVATE PkgConfig::DTK)
    endif()

    # XCB
    pkg_check_modules(XCB IMPORTED_TARGET xcb xcb-util)
    if(XCB_FOUND)
        target_link_libraries(${TEST_NAME} PRIVATE PkgConfig::XCB)
    endif()

    # 覆盖率库
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(${TEST_NAME} PRIVATE gcov)
    endif()

    target_include_directories(${TEST_NAME}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/autotests/3rdparty/stub
        ${CMAKE_SOURCE_DIR}/src
    )

    gtest_discover_tests(${TEST_NAME} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endforeach()

message(STATUS "UT: autotests/src configured with ${TEST_SOURCES}")
