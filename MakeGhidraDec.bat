@echo off
setlocal

cd /d "%~dp0"

if "%BUILD_DIR%"=="" set "BUILD_DIR=build"
if "%CONFIG%"=="" set "CONFIG=Release"
if "%PLATFORM%"=="" set "PLATFORM=x64"
if "%CMAKE_EXE%"=="" set "CMAKE_EXE=cmake"

if not "%WIN_FLEX_BISON_PATH%"=="" set "PATH=%PATH%;%WIN_FLEX_BISON_PATH%"

if "%IDA_SDK_DIR%"=="" (
    echo IDA_SDK_DIR is required. Example:
    echo   set IDA_SDK_DIR=^<path-to-idasdk93^>
    exit /b 1
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

"%CMAKE_EXE%" .. -DIDA_SDK_DIR="%IDA_SDK_DIR%" -DIDA_PATH="%IDA_PATH%"
if errorlevel 1 exit /b 1

"%CMAKE_EXE%" --build . --config "%CONFIG%" -- -m
if errorlevel 1 exit /b 1

if not "%IDA_PATH%"=="" (
    "%CMAKE_EXE%" --build . --config "%CONFIG%" --target install
    if errorlevel 1 exit /b 1
)

endlocal
