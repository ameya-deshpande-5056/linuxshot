cmake_minimum_required(VERSION 3.22)
project(LinuxShot
    VERSION 0.1.2
    HOMEPAGE_URL "https://github.com/ameya-deshpande-5056/linuxshot"
    LANGUAGES CXX
)

string(TIMESTAMP LINUXSHOT_BUILD_TIMESTAMP "%Y-%m-%dT%H:%M:%SZ" UTC)
set(LINUXSHOT_PROJECT_URL "${PROJECT_HOMEPAGE_URL}")

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE LINUXSHOT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT LINUXSHOT_COMMIT_HASH)
    set(LINUXSHOT_COMMIT_HASH "unknown")
endif()

add_compile_definitions(
    LINUXSHOT_VERSION="${PROJECT_VERSION}"
    LINUXSHOT_BUILD_TIMESTAMP="${LINUXSHOT_BUILD_TIMESTAMP}"
    LINUXSHOT_PROJECT_URL="${LINUXSHOT_PROJECT_URL}"
    LINUXSHOT_COMMIT_HASH="${LINUXSHOT_COMMIT_HASH}"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Build type defaults
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)

# Sanitizers for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
    option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
    if(ENABLE_ASAN)
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()
    if(ENABLE_UBSAN)
        add_compile_options(-fsanitize=undefined)
        add_link_options(-fsanitize=undefined)
    endif()
endif()

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network DBus)

# Find XCB
find_package(PkgConfig REQUIRED)
pkg_check_modules(XCB REQUIRED xcb xcb-shm xcb-keysyms)

# Source files
set(CORE_SOURCES
    src/core/Application.cpp
)

set(SETTINGS_SOURCES
    src/settings/SettingsManager.cpp
)

set(PLATFORM_SOURCES
    src/platform/GlobalShortcutManager.cpp
    src/platform/Platform.cpp
)

set(CAPTURE_SOURCES
    src/capture/CaptureManager.cpp
    src/capture/X11CaptureBackend.cpp
    src/capture/PortalCaptureBackend.cpp
)

set(UI_SOURCES
    src/ui/SelectionOverlay.cpp
    src/ui/AnnotationEditor.cpp
    src/ui/FloatingTextEditor.cpp
    src/ui/Toolbar.cpp
    src/ui/TrayIcon.cpp
    src/ui/SettingsDialog.cpp
    src/ui/AboutDialog.cpp
    src/ui/ColorButton.cpp
    src/ui/IconHelper.cpp
)

set(ANNOTATION_SOURCES
    src/annotation/CommandHistory.cpp
    src/annotation/PenTool.cpp
    src/annotation/LineTool.cpp
    src/annotation/ArrowTool.cpp
    src/annotation/RectTool.cpp
    src/annotation/CircleTool.cpp
    src/annotation/MarkerTool.cpp
    src/annotation/TextTool.cpp
    src/annotation/BlurTool.cpp
)

set(EXPORT_SOURCES
    src/export/ExportManager.cpp
    src/export/ImageExporter.cpp
)

set(UPLOAD_SOURCES
    src/upload/UploadProvider.cpp
    src/upload/UploadManager.cpp
    src/upload/SelfHostedProvider.cpp
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

# Main executable
add_executable(linuxshot
    src/main.cpp
    ${CORE_SOURCES}
    ${SETTINGS_SOURCES}
    ${PLATFORM_SOURCES}
    ${CAPTURE_SOURCES}
    ${UI_SOURCES}
    ${ANNOTATION_SOURCES}
    ${EXPORT_SOURCES}
    ${UPLOAD_SOURCES}
    ${RESOURCES}
)

target_include_directories(linuxshot PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${XCB_INCLUDE_DIRS}
)

target_link_libraries(linuxshot PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::DBus
    ${XCB_LIBRARIES}
)

# Install
install(TARGETS linuxshot RUNTIME DESTINATION bin)
install(FILES packaging/linuxshot.desktop DESTINATION share/applications)
install(FILES resources/icons/linuxshot.png DESTINATION share/icons/hicolor/256x256/apps)
install(FILES resources/icons/linuxshot.svg DESTINATION share/icons/hicolor/scalable/apps)

# Uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )
    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()

# Testing
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    find_package(Qt6 REQUIRED COMPONENTS Test)
    
    add_executable(test_command_history
        tests/test_command_history.cpp
        src/annotation/CommandHistory.cpp
    )
    target_include_directories(test_command_history PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(test_command_history PRIVATE Qt6::Core Qt6::Gui Qt6::Test Qt6::Widgets)
    add_test(NAME CommandHistory COMMAND test_command_history)

    add_executable(test_settings
        tests/test_settings.cpp
        src/settings/SettingsManager.cpp
    )
    target_include_directories(test_settings PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(test_settings PRIVATE Qt6::Core Qt6::Gui Qt6::Test Qt6::Widgets)
    add_test(NAME Settings COMMAND test_settings)

    add_executable(test_image_exporter
        tests/test_image_exporter.cpp
        src/export/ImageExporter.cpp
    )
    target_include_directories(test_image_exporter PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(test_image_exporter PRIVATE Qt6::Core Qt6::Gui Qt6::Test Qt6::Widgets)
    add_test(NAME ImageExporter COMMAND test_image_exporter)

    add_executable(test_annotation_tools
        tests/test_annotation_tools.cpp
        ${ANNOTATION_SOURCES}
        src/ui/FloatingTextEditor.cpp
    )
    target_include_directories(test_annotation_tools PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(test_annotation_tools PRIVATE Qt6::Core Qt6::Gui Qt6::Test Qt6::Widgets)
    add_test(NAME AnnotationTools COMMAND test_annotation_tools)

    add_executable(test_platform
        tests/test_platform.cpp
        src/platform/Platform.cpp
    )
    target_include_directories(test_platform PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(test_platform PRIVATE Qt6::Core Qt6::Gui Qt6::DBus Qt6::Test)
    add_test(NAME Platform COMMAND test_platform)

    add_executable(test_icons
        tests/test_icons.cpp
        src/ui/IconHelper.cpp
        ${RESOURCES}
    )
    target_include_directories(test_icons PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(test_icons PRIVATE Qt6::Core Qt6::Gui Qt6::Test Qt6::Widgets)
    add_test(NAME Icons COMMAND test_icons)

    set_tests_properties(
        CommandHistory Settings ImageExporter AnnotationTools Platform Icons
        PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    )

    add_executable(generate_screenshots
        tools/generate_screenshots.cpp
        ${UI_SOURCES}
        ${ANNOTATION_SOURCES}
        ${SETTINGS_SOURCES}
        ${EXPORT_SOURCES}
        ${UPLOAD_SOURCES}
        ${PLATFORM_SOURCES}
        ${CAPTURE_SOURCES}
        ${RESOURCES}
    )
    target_include_directories(generate_screenshots PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${XCB_INCLUDE_DIRS}
    )
    target_link_libraries(generate_screenshots PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::DBus
        ${XCB_LIBRARIES}
    )
endif()

