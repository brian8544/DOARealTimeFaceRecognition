@echo off
setlocal

:: Set the variables
set "QT_DIR=C:\Qt\6.5.2\msvc2019_64"
set "PROJECT_DIR=F:\Repositories\RealTimeFaceRecognition"
set "VCPKG_DIR=F:\Repositories\vcpkg"
set "BUILD_DIR=%PROJECT_DIR%\build\debug"

:: Third-party DLL's.
echo Copying third-party DLLs...
copy /y "%VCPKG_DIR%\installed\x64-windows\bin\*.dll" "%BUILD_DIR%"

echo Deployment completed.
pause
exit