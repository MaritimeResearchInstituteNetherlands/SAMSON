@echo off

setlocal

echo.
echo ===== build_core =====

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

if "%SAMSON_ROOT_DIR%"=="" (
    echo ERROR: SAMSON_ROOT_DIR not set!
    echo Example:
    echo   set SAMSON_ROOT_DIR=D:\projects\samson
    exit /b 1
)

echo.
echo ===== Initialise build environment =====

call "%VS_BUILDTOOLS_ROOT%\Common7\Tools\VsDevCmd.bat" -arch=x64

set "WindowsSdkDir=%WINDOWS_SDK_ROOT%"
set "WindowsSDKVersion=%WINDOWS_SDK_VERSION%.0\"
set "LIB=%WINDOWS_SDK_LIBS%;%LIB%"
set "INCLUDE=%WINDOWS_SDK_INCLUDES%;%INCLUDE%"

if defined VCPKG_CHECKOUT_DIR (
    set VCPKG_ROOT=%VCPKG_CHECKOUT_DIR%
)

rem REM Install vcpkg.exe
rem pgutil assets download ^
rem     --source=%UTILITIES_URL% ^
rem     --feed=%UTILITIES_FEED% ^
rem     --path=%UTILITIES_VCPKG_PATH% ^
rem     --output=%VCPKG_ROOT%

REM Installing vcpkg.exe with curl:
curl %UTILITIES_VCPKG_CURL% --output "%VCPKG_ROOT%\vcpkg.exe"

REM Determing CMAKE_GENERATOR
set "generatorArgs="
if "%CMAKE_GENERATOR%"=="vs" (
    set "generatorArgs=-G "Visual Studio 17 2022" -DCMAKE_GENERATOR_INSTANCE=%VS_BUILDTOOLS_ROOT%,version=%VS_BUILDTOOLS_VERSION%.0"
) else if "%CMAKE_GENERATOR%"=="ninja" (
    set "generatorArgs=-G Ninja"
)

set "PATH=%VCPKG_ROOT%;%NINJA_BIN%;%WINDOWS_SDK_BIN%;%PATH%"

cd %SAMSON_ROOT_DIR%

setlocal EnableDelayedExpansion
if 0!CI_CD_DEBUG_LEVEL! GEQ 1 (
    echo.
    echo ===== Analyse build environment =====

    echo WindowsSdkDir=!WindowsSdkDir!
    echo WindowsSDKVersion=!WindowsSDKVersion!
    echo VCPKG_ROOT=!VCPKG_ROOT!
    echo DOTNET_ROOT=!DOTNET_ROOT!

    echo LIB=!LIB!
    echo INCLUDE=!INCLUDE!

    where vcpkg.exe
    where msbuild.exe
    where cl.exe
    where rc.exe
    where mt.exe

    vcpkg.exe --version

    echo generatorArgs=!generatorArgs!

    if "%CMAKE_GENERATOR%"=="vs" (
        echo Using Visual Studio generator
    ) else if "%CMAKE_GENERATOR%"=="ninja" (
        echo Using Ninja generator
    )

    if 0!CI_CD_DEBUG_LEVEL! GEQ 2 (
        echo Listing environment
        set

        REM Compile a singe file to check windows headers are included from the nuget package, instead of Windows Kit
        echo Calling cl /Bv /showIncludes
        echo #include ^<windows.h^> > test.cpp
        echo int main^(^){} >> test.cpp
        cl test.cpp /Bv /showIncludes 2>&1
        del test.cpp
        del test.exe
    )
)
endlocal

"%CMAKE_EXE%" ^
    -S . ^
    -B build ^
    --preset CI_CD ^
    %generatorArgs%

if errorlevel 1 exit /b 1


echo.
echo ===== Build =====

"%CMAKE_EXE%" ^
    --build ^
    --preset CI_CD

if errorlevel 1 exit /b 1


echo.
echo ===== Test =====

echo Running unit-tests
"%CTEST_EXE%" ^
    --test-dir build ^
    --output-on-failure ^
    --output-junit unit_test_results\results.xml
    
if errorlevel 1 exit /b 1


echo.
echo ===== Install =====

"%CMAKE_EXE%" ^
    --install build ^
    --config Release

if errorlevel 1 exit /b 1
