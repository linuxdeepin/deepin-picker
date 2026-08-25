cmake_minimum_required(VERSION 3.12)

project(deepin-picker VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt version detection: try Qt6 first, fallback to Qt5
find_package(Qt6 COMPONENTS Core Widgets Gui DBus QUIET)
if(Qt6Widgets_FOUND)
    set(QT_VERSION_MAJOR 6)
else()
    set(QT_VERSION_MAJOR 5)
endif()

# C++ standard: DTK6 requires C++17, DTK5 works with C++11
if(QT_VERSION_MAJOR EQUAL 6)
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 11)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# PkgConfig (only once)
find_package(PkgConfig REQUIRED)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Gui DBus)

# DTK
if(QT_VERSION_MAJOR EQUAL 6)
    pkg_check_modules(DTK REQUIRED IMPORTED_TARGET dtk6widget dtk6gui)
else()
    pkg_check_modules(DTK REQUIRED IMPORTED_TARGET dtkwidget dtkcore dtkgui)
endif()

# xcb
pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb xcb-util)

# Sources
set(SOURCES
    src/main.cpp
    src/cpickermanager.cpp
    src/desktopinfo.cpp
    src/colormenu.cpp
    src/clipboard.cpp
    src/animation.cpp
    src/dbusnotify.cpp
    src/settings.cpp
    src/utils.cpp
)

set(HEADERS
    src/cpickermanager.h
    src/desktopinfo.h
    src/colormenu.h
    src/clipboard.h
    src/animation.h
    src/dbusnotify.h
    src/settings.h
    src/utils.h
)

# Resources
set(RESOURCES deepin-picker.qrc)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    add_subdirectory(autotests)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::DBus
    PkgConfig::DTK
    PkgConfig::XCB
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _FORTIFY_SOURCE=2
    DSR_LANG_PATH="/usr/share/deepin-picker/translations"
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wno-error=deprecated-declarations -Wno-deprecated-declarations
    -fstack-protector-strong -O2 -g
)

target_link_options(${PROJECT_NAME} PRIVATE
    -Wl,--as-needed -Wl,-z,noexecstack -Wl,-z,now
)

# Install (paths match .pro file conventions)
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)

install(FILES image/deepin-picker.svg
    DESTINATION /usr/share/icons/hicolor/scalable/apps)

install(FILES deepin-picker.desktop
    DESTINATION /usr/share/applications)

install(FILES com.deepin.Picker.service
    DESTINATION /usr/share/dbus-1/services)

# Translations (optional, requires LinguistTools)
find_package(Qt${QT_VERSION_MAJOR}LinguistTools QUIET)
if(Qt${QT_VERSION_MAJOR}LinguistTools_FOUND)
    file(GLOB TS_FILES translations/*.ts)
    if(QT_VERSION_MAJOR EQUAL 6)
        qt_add_translation(QM_FILES ${TS_FILES})
    else()
        qt5_add_translation(QM_FILES ${TS_FILES})
    endif()
    install(FILES ${QM_FILES}
        DESTINATION /usr/share/deepin-picker/translations)
endif()
