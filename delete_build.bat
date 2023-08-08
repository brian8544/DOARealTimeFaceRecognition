@echo off
cls
taskkill /F /IM BrianApp.exe

set "PROJECT_DIR=F:\Repositories\BrianApp"
set "BUILD_DIR=%PROJECT_DIR%\build"

:: Check if the build directory exists, if it exists, delete it
if exist "%BUILD_DIR%" (
    echo Deleting existing build directory...
    rmdir /s /q "%BUILD_DIR%"
)