cmake_minimum_required(VERSION 3.5)

# Set the vcpkg toolchain file path
set(CMAKE_TOOLCHAIN_FILE "F:/repositories/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_PATH "F:/Repositories/vcpkg")
set(CMAKE_PREFIX_PATH "C:/Qt/6.5.2/msvc2019_64" ${CMAKE_PREFIX_PATH})

project(RealTimeFaceRecognition VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# --- Packages | Don't forget to manually copy DLL files! ---

# OpenCV:
set(OpenCV_DIR "${VCPKG_PATH}/installed/x64-windows/share/opencv")
find_package(OpenCV REQUIRED)

set(PROJECT_SOURCES
        src/PCH.h
        src/Main.cpp

        src/CameraGui/CameraGui.cpp
        src/CameraGui/CameraGui.h
        src/CameraGui/CameraGui.ui

        res/qt-resource.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RealTimeFaceRecognition
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${CMAKE_SOURCE_DIR}/res/resource.rc
    )
endif()

target_link_libraries(RealTimeFaceRecognition
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${OpenCV_LIBS}
)

message(STATUS "Don't forget to run QT Creator as an administrator!")
set_target_properties(RealTimeFaceRecognition PROPERTIES
    WIN32_EXECUTABLE TRUE
    #LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
)

install(TARGETS RealTimeFaceRecognition
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RealTimeFaceRecognition)
endif()
