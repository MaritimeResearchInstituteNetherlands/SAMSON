@echo off

rem Configure debugging by setting environment variable
rem CI_CD_DEBUG_LEVEL=1 for debug output
rem CI_CD_DEBUG_LEVEL=2 for trace output

echo.
echo ===== set_vars_core =====

REM ---- Check if required variables are set ----
if "%CI_CD_BUILDTOOLS_ROOT%"=="" (
    echo ERROR: CI_CD_BUILDTOOLS_ROOT not set!
    echo Example:
    echo   set CI_CD_BUILDTOOLS_ROOT=D:\nuget
    exit /b 1
)

if "%NUGET_EXE%"=="" (
    echo ERROR: NUGET_EXE not set!
    echo Example:
    echo   set NUGET_EXE=D:\Software\nuget\5.5\bin\nuget.exe
    exit /b 1
)

if 0%CI_CD_DEBUG% GEQ 1 (
    echo Variables optionally set by upstream:
    echo "CI_CD_BUILDTOOLS_ROOT=%CI_CD_BUILDTOOLS_ROOT%"
    echo "NUGET_EXE=%NUGET_EXE%"
    echo "CMAKE_GENERATOR=%CMAKE_GENERATOR%"
    echo "VCPKG_CHECKOUT_DIR=%VCPKG_CHECKOUT_DIR%"
    echo "CI_CD_DEBUG=%CI_CD_DEBUG%"
)

REM ==== INPUT VARIABLES ====

REM ---- Versions ----
set CMAKE_PACKAGE=cmake
set CMAKE_VERSION=4.3.2

set VS_BUILDTOOLS_PACKAGE=VisualStudioBuildTools
set VS_BUILDTOOLS_VERSION_MAJOR=17
set VS_BUILDTOOLS_VERSION_MINOR=14
set VS_BUILDTOOLS_VERSION_INC=6
set VS_BUILDTOOLS_VERSION_YEAR=2022

set WINDOWS_SDK_PACKAGE=Microsoft.Windows.SDK
set WINDOWS_SDK_VERSION_BASE=10.0.26100
set WINDOWS_SDK_VERSION_REVISION=4654

set DOTNET_SDK_PACKAGE=dotnet-sdk.x64
set DOTNET_SDK_VERSION=8.0.401

set UTILITIES_FEED=utilities
set UTILITIES_VCPKG_PACKAGE=vcpkg
set UTILITIES_VCPKG_VERSION=2026-04-08

REM ---- urls ----
set "CI_CD_BUILDTOOLS_URL=https://proget.marin.local/nuget/Default/v3/index.json"
set "VCPKG_BINARY_CACHING_URL=https://proget.marin.local/nuget/vcpkg_binaries/v3/index.json"
set "VCPKG_ASSET_CACHING_URL=https://proget.marin.local/nuget/vcpkg_assets/v3/index.json"
set "UTILITIES_URL=https://proget.marin.local"
set "UTILITIES_CURL_URL=http://proget.marin.local/endpoints/utilities/content"

REM ---- General ----
set ARCH=x64


REM ==== DERIVED VARIABLES ====

REM ---- Visual Studio ----
set VS_BUILDTOOLS_VERSION=%VS_BUILDTOOLS_VERSION_MAJOR%.%VS_BUILDTOOLS_VERSION_MINOR%.%VS_BUILDTOOLS_VERSION_INC%
set "VS_BUILDTOOLS_ROOT=%CI_CD_BUILDTOOLS_ROOT%\%VS_BUILDTOOLS_PACKAGE%.%VS_BUILDTOOLS_VERSION%\%VS_BUILDTOOLS_VERSION_YEAR%\BuildTools"
set "VS_INSTANCE=%VS_BUILDTOOLS_ROOT%"
set "VS_VERSION=%VS_BUILDTOOLS_VERSION_MAJOR%"

REM ---- Windows SDK ----
set WINDOWS_SDK_VERSION=%WINDOWS_SDK_VERSION_BASE%.%WINDOWS_SDK_VERSION_REVISION%

set "WINDOWS_SDK_ROOT=%CI_CD_BUILDTOOLS_ROOT%\%WINDOWS_SDK_PACKAGE%"
set "WINDOWS_SDK_CPP_ROOT=%WINDOWS_SDK_ROOT%.CPP.%WINDOWS_SDK_VERSION%\c"

set "WINDOWS_SDK_BIN=%WINDOWS_SDK_CPP_ROOT%\bin\%WINDOWS_SDK_VERSION_BASE%.0\%ARCH%"

set "WINDOWS_SDK_INCLUDE=%WINDOWS_SDK_CPP_ROOT%\Include\%WINDOWS_SDK_VERSION_BASE%.0"
set "WINDOWS_SDK_INCLUDES=%WINDOWS_SDK_INCLUDE%\cppwinrt;%WINDOWS_SDK_INCLUDE%\shared;%WINDOWS_SDK_INCLUDE%\ucrt;%WINDOWS_SDK_INCLUDE%\um;%WINDOWS_SDK_INCLUDE%\winrt;"

set "WINDOWS_SDK_LIB=%WINDOWS_SDK_ROOT%.CPP.%ARCH%.%WINDOWS_SDK_VERSION%\c"
set "WINDOWS_SDK_LIBS=%WINDOWS_SDK_LIB%\ucrt\%ARCH%;%WINDOWS_SDK_LIB%\um\%ARCH%"

REM ---- CMake ----
set "CMAKE_EXE=%CI_CD_BUILDTOOLS_ROOT%\cmake.%CMAKE_VERSION%\bin\cmake.exe"
set "CTEST_EXE=%CI_CD_BUILDTOOLS_ROOT%\cmake.%CMAKE_VERSION%\bin\ctest.exe"

REM ---- Ninja ----
SET "NINJA_BIN=%VS_BUILDTOOLS_ROOT%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"

REM ---- Determine root ----
set "SAMSON_ROOT_DIR=%~dp0.."
for %%I in ("%SAMSON_ROOT_DIR%") do set "SAMSON_ROOT_DIR=%%~fI"

REM ---- Binary caching ----
if "%VCPKG_BINARY_CACHING_TYPE%"=="none" (
    set "VCPKG_BINARY_SOURCES=clear"
) else if "%VCPKG_BINARY_CACHING_TYPE%"=="nuget_read" (
    set "VCPKG_BINARY_SOURCES=clear;nugetconfig,%SAMSON_ROOT_DIR%\NuGet.config,read"
) else if "%VCPKG_BINARY_CACHING_TYPE%"=="nuget_readwrite" (
    set "VCPKG_BINARY_SOURCES=clear;nugetconfig,%SAMSON_ROOT_DIR%\NuGet.config,readwrite"
) else (
    set "VCPKG_BINARY_SOURCES="
)

REM ---- Utilities ----
set UTILITIES_VCPKG_PATH=%UTILITIES_VCPKG_PACKAGE%/%UTILITIES_VCPKG_VERSION%/vcpkg.exe
set UTILITIES_VCPKG_CURL=%UTILITIES_CURL_URL%/%UTILITIES_VCPKG_PACKAGE%/%UTILITIES_VCPKG_VERSION%/vcpkg.exe


