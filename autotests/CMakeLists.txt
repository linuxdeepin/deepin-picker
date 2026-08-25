cmake_minimum_required(VERSION 3.16)
project(ut_deepin_picker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(UnitTestUtils)

enable_testing()

# Google Test
find_package(GTest REQUIRED)
include(GoogleTest)

# 代码覆盖率：Debug 模式下启用 gcov 插桩
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov")
    message(STATUS "UT: Code coverage enabled (gcov instrumentation)")
endif()

# Third-party packages
find_package(PkgConfig REQUIRED)
if(Qt6Widgets_FOUND)
    set(QT_VERSION_MAJOR 6)
    pkg_check_modules(DTK IMPORTED_TARGET dtk6widget dtk6gui)
else()
    set(QT_VERSION_MAJOR 5)
    pkg_check_modules(DTK IMPORTED_TARGET dtkwidget dtkcore dtkgui)
endif()

# Test subdirectories
# {ADD_SUBDIRECTORIES}
add_subdirectory(src)

message(STATUS "UT: Unit tests configuration complete")
