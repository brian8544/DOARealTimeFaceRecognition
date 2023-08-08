@echo off
taskkill /F /IM BrianApp.exe
cls
setlocal

:: Set the variables
set "QT_DIR=C:\Qt\6.5.2\msvc2019_64"
set "THEMIDA_DIR=D:\iCloudDrive\Games & Apps\Utilities\Themida\"
set "CMAKE_DIR=C:\Program Files\CMake\bin"
set "VCPKG_DIR=F:\Repositories\vcpkg"
set "PROJECT_DIR=F:\Repositories\BrianApp"
set "BUILD_DIR=%PROJECT_DIR%\build"
set "APP_EXE_PATH=%BUILD_DIR%\release\BrianApp.exe"
set "DEPLOY_DIR=%BUILD_DIR%\Deploy"
set "UPLOAD_DIR=\\10.10.10.10\disk\Websites\www\cdn.brianoost.com\files\brianapp"

echo === Fetching Required Packages ===

:: Prepare library packages, if not exist.
call %VCPKG_DIR%\bootstrap-vcpkg.bat
%VCPKG_DIR%\vcpkg install curl
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
"%QT_DIR%\bin\windeployqt.exe" --release --no-translations --dir "%DEPLOY_DIR%" "%DEPLOY_DIR%\BrianApp.exe"

:: Third-party DLL's.
echo Copying third-party DLLs...
copy /Y "%PROJECT_DIR%\dep\MySQL\Connector C++ 8.0\lib64\mysqlcppconn-9-vs14.dll" "%DEPLOY_DIR%"
copy /Y "%PROJECT_DIR%\dep\MySQL\Connector C++ 8.0\lib64\libcrypto-3-x64.dll" "%DEPLOY_DIR%"
copy /Y "%PROJECT_DIR%\dep\MySQL\Connector C++ 8.0\lib64\libssl-3-x64.dll" "%DEPLOY_DIR%"
copy /Y "%VCPKG_DIR%\packages\curl_x64-windows\bin\*" "%DEPLOY_DIR%"
copy /Y "%VCPKG_DIR%\packages\zlib_x64-windows\bin\zlib1.dll" "%DEPLOY_DIR%"

:: Manual labor:
echo Copying custom files...
copy /Y "%PROJECT_DIR%\dep\patcher\patcher.cmd" "%DEPLOY_DIR%"

:: Obfuscation
cd /d "%THEMIDA_DIR%"
themida64.exe /protect F:\Repositories\BrianApp\contrib\obfuscation\themida.tmd
cd /d "%DEPLOY_DIR%"
del /s /q "BrianApp.exe"
ren "BrianApp_protected.exe" "BrianApp.exe"

:: Cleanup
echo Cleaning up...
rmdir /s /q "%DEPLOY_DIR%\tls"
rmdir /s /q "%DEPLOY_DIR%\networkinformation"
rmdir /s /q "%DEPLOY_DIR%\imageformats"
rmdir /s /q "%DEPLOY_DIR%\iconengines"
rmdir /s /q "%DEPLOY_DIR%\generic"
del /s /q "%DEPLOY_DIR%\D3Dcompiler_47.dll"
del /s /q "%DEPLOY_DIR%\opengl32sw.dll"
del /s /q "%DEPLOY_DIR%\Qt6Pdf.dll"
del /s /q "%DEPLOY_DIR%\Qt6Svg.dll"
del /s /q "%DEPLOY_DIR%\Qt6Network.dll"
del /s /q "%DEPLOY_DIR%\*.pdb"

echo Deployment completed.

echo Sending to CDN...
powershell Compress-Archive -Path %DEPLOY_DIR%/* -DestinationPath %UPLOAD_DIR%\BrianApp-latest.zip -Force

echo === Build and Deployment process completed successfully ===
pause
exit /b 0