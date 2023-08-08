@echo off
setlocal

:: Set the variables
set "QT_DIR=C:\Qt\6.5.2\msvc2019_64"
set "PROJECT_DIR=F:\Repositories\BrianApp"
set "VCPKG_DIR=F:\Repositories\vcpkg"
set "BUILD_DIR=%PROJECT_DIR%\build\debug"

:: Third-party DLL's.
echo Copying third-party DLLs...

copy /Y "%PROJECT_DIR%\dep\MySQL\Connector C++ 8.0\lib64\vs14\debug\mysqlcppconn-9-vs14.dll" "%BUILD_DIR%"
copy /Y "%PROJECT_DIR%\dep\MySQL\Connector C++ 8.0\lib64\libcrypto-3-x64.dll" "%BUILD_DIR%"
copy /Y "%PROJECT_DIR%\dep\MySQL\Connector C++ 8.0\lib64\libssl-3-x64.dll" "%BUILD_DIR%"

copy /Y "%VCPKG_DIR%\packages\curl_x64-windows\bin\*" "%BUILD_DIR%"
copy /Y "%VCPKG_DIR%\packages\zlib_x64-windows\bin\zlib1.dll" "%BUILD_DIR%"

echo Deployment completed.
pause
exit