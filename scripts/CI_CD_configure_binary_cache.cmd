@echo off

echo.
echo ===== CONFIGURE BINARY CACHE START =====

REM set defaultPushSource
"%NUGET_EXE%" config ^
    -Set defaultPushSource="%VCPKG_BINARY_CACHING_URL%" ^
    -ConfigFile .\NuGet.config

REM remove existing source to prevent warning about duplicate key
"%NUGET_EXE%" sources remove ^
    -Name "vcpkg-binary-cache" ^
    -ConfigFile .\NuGet.config >nul 2>&1

REM add source
"%NUGET_EXE%" sources add ^
    -Name "vcpkg-binary-cache" ^
    -Source "%VCPKG_BINARY_CACHING_URL%" ^
    -ConfigFile .\NuGet.config
