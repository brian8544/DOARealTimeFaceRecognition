cmake_minimum_required(VERSION 3.5)

# Set the vcpkg toolchain file path
set(CMAKE_TOOLCHAIN_FILE "F:/repositories/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_PATH "F:/Repositories/vcpkg")
set(CMAKE_PREFIX_PATH "C:/Qt/6.5.2/msvc2019_64" ${CMAKE_PREFIX_PATH})

project(RealTimeFacialRecognition VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# --- Packages | Don't forget to manually copy DLL files! ---

# Curl:
set(CURL_INCLUDE_DIRS "${VCPKG_PATH}/installed/include")
set(CURL_LIBRARIES "${VCPKG_PATH}/installed/lib/libcurl.lib")
find_package(CURL REQUIRED)

set(PROJECT_SOURCES
        src/PCH.h
        src/Main.cpp

        src/CameraGui.cpp
        src/CameraGui.h
        src/CameraGui.ui

        res/qt-resource.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BrianApp
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${CMAKE_SOURCE_DIR}/res/resource.rc
    )
endif()

target_link_libraries(BrianApp
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    CURL::libcurl
)

message(STATUS "Don't forget to run QT Creator as an administrator!")
set_target_properties(BrianApp PROPERTIES
    WIN32_EXECUTABLE TRUE
    #LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
)

install(TARGETS BrianApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BrianApp)
endif()
