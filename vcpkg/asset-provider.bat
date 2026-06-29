@echo off
setlocal

set "url=%~1"
set "sha512=%~2"
set "dst=%~3"
set "_CD=%CD%"

REM Normalize path separators
set "_dst=%dst:/=\%"

rem strip trailing backslash
set "BASE_PATH=%~dp3"
if "%BASE_PATH:~-1%"=="\" set "BASE_PATH=%BASE_PATH:~0,-1%"

echo ============ start of asset-provider =============
echo ---- Start of data for create_asset_package.cmd ----
REM use %VCPKG_ROOT% in data for create_asset_package
set "dst_rel=%_dst%"
call set "dst_rel=%%dst_rel:%VCPKG_CHECKOUT_DIR%=%%"
set "dst_var=%%VCPKG_ROOT%%%dst_rel%"

echo set url=%url%
echo set sha512=%sha512%
echo set dst=%dst_var%

rem echo --- absolute dst:
rem echo set dst=%_dst%

echo ---- End of data for create_asset_package.cmd ----

REM Shorten sha
set "_sha512=%sha512:~0,50%"
set "description=vcpkg download asset: %url%"

REM Ensure working dir exists
mkdir "%~dp3"

REM Change directory safely
cd /d "%~dp3"

if errorlevel 1 (
    echo ERROR: Failed to change directory to "%~dp3"
    exit /b 1
)

echo Pull from the NuGet feed
echo calling: "%NUGET_EXE%" install "%_sha512%" -Source "%VCPKG_ASSET_CACHING_URL%"
"%NUGET_EXE%" install "%_sha512%" -Source "%VCPKG_ASSET_CACHING_URL%"
echo.

if exist "%_sha512%.1.0.0" (
    echo check if "%_dst%" exists
    if not exist "%_dst%"  (
        echo "%_dst%" does not exist
        cd "%_sha512%.1.0.0"

        REM Assume it is a file, not a directory
        REM The current version of vcpkg calls this script with the dst argument
        REM with .part suffix. Other versions (probably older) don't use this suffix
        echo F | xcopy /f "*.part" "%_dst%"
    )
) else (
    echo Fetch from the url
    curl.exe -L "%url%" --create-dirs --output "%dst%"

    echo Create nuget package
    "%NUGET_EXE%" pack "%~dp0asset-source.nuspec" ^
        -BasePath "%BASE_PATH%" ^
        -Properties "sha=%_sha512%;file=%dst%;description=%description%;url=%url%" ^
        -OutputDirectory "%TEMP%\vcpkg_assets"

    echo Upload nuget package
    "%NUGET_EXE%" push -SkipDuplicate "%TEMP%\vcpkg_assets\%_sha512%.1.0.0.nupkg" -Source "%VCPKG_ASSET_CACHING_URL%"
)

cd %_CD%
echo ============ end of asset-provider =============

endlocal