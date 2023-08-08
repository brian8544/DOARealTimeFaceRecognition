@echo off
taskkill /F /IM RealTimeFacialRecognition.exe
cls
setlocal

:: Set the variables
set "QT_DIR=C:\Qt\6.5.2\msvc2019_64"
set "CMAKE_DIR=C:\Program Files\CMake\bin"
set "VCPKG_DIR=F:\Repositories\vcpkg"
set "PROJECT_DIR=F:\Repositories\RealTimeFaceRecognition"
set "BUILD_DIR=%PROJECT_DIR%\build"
set "APP_EXE_PATH=%BUILD_DIR%\release\RealTimeFaceRecognition.exe"
set "DEPLOY_DIR=%BUILD_DIR%\Deploy"

echo === Fetching Required Packages ===

:: Prepare library packages, if not exist.
call %VCPKG_DIR%\bootstrap-vcpkg.bat
%VCPKG_DIR%\vcpkg install curl
%vcpkgPath%\vcpkg install opencv4
%vcpkgPath%\vcpkg install dlib
%VCPKG_DIR%\vcpkg integrate install

echo === Starting Build and Deployment ===

:: Check if the build directory exists, if it exists, delete it
if exist "%BUILD_DIR%" (
    echo Deleting existing build directory...
    rmdir /s /q "%BUILD_DIR%"
)

:: Create the build directory
echo Creating build directory...
mkdir "%BUILD_DIR%"

:: Change the current directory to the build directory
cd /d "%BUILD_DIR%"

:: Run CMake to configure the project
echo Configuring the project with CMake...
"%CMAKE_DIR%\cmake.exe" -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=Release "%PROJECT_DIR%"

:: Build the project using the appropriate build tool (e.g., ninja or msbuild)
echo Building the project...
"%CMAKE_DIR%\cmake.exe" --build . --config Release

:: Check if the build was successful
if errorlevel 1 (
    echo Build failed. Please check the build logs for errors.
	pause
    exit /b 1
)

echo Build successful.

echo Deploying %APP_EXE_PATH%...

:: Create the deploy directory
mkdir "%DEPLOY_DIR%"

:: Copy the application executable to the deploy directory
copy /Y "%APP_EXE_PATH%" "%DEPLOY_DIR%"

:: Run windeployqt to deploy required Qt libraries and plugins
echo Deploying Qt libraries and plugins...
"%QT_DIR%\bin\windeployqt.exe" --release --no-translations --dir "%DEPLOY_DIR%" "%DEPLOY_DIR%\RealTimeFaceRecognition.exe"

:: Third-party DLL's.
echo Copying third-party DLLs...
copy /Y "%VCPKG_DIR%\installed\x64-windows\bin\*.dll" "%DEPLOY_DIR%"

:: Manual labor:
echo Copying custom files...

:: Cleanup
echo Cleaning up...

echo Deployment completed.

echo === Build and Deployment process completed successfully ===
pause
exit /b 0