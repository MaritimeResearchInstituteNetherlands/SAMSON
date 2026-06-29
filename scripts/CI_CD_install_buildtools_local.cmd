@echo off

if "%CI_CD_BUILDTOOLS_ROOT%"=="" (
    echo Variables not set: call set_vars_local.cmd before calling this script
    exit /b 1
)

if "%NUGET_EXE%"=="" (
    echo Variables not set: call set_vars_local.cmd before calling this script
    exit /b 1
)

REM CMAKE
"%NUGET_EXE%" install %CMAKE_PACKAGE% ^
    -Version %CMAKE_VERSION% ^
    -source %CI_CD_BUILDTOOLS_URL% ^
    -NonInteractive ^
    -ForceEnglishOutput ^
    -PackageSaveMode nuspec ^
    -OutputDirectory %CI_CD_BUILDTOOLS_ROOT%

REM VS_BUILDTOOLS
"%NUGET_EXE%" install %VS_BUILDTOOLS_PACKAGE% ^
    -Version %VS_BUILDTOOLS_VERSION% ^
    -source %CI_CD_BUILDTOOLS_URL% ^
    -NonInteractive ^
    -ForceEnglishOutput ^
    -PackageSaveMode nuspec ^
    -OutputDirectory %CI_CD_BUILDTOOLS_ROOT%
    
REM WINDOWS_SDK
"%NUGET_EXE%" install %WINDOWS_SDK_PACKAGE%.CPP ^
    -Version %WINDOWS_SDK_VERSION% ^
    -source %CI_CD_BUILDTOOLS_URL% ^
    -NonInteractive ^
    -ForceEnglishOutput ^
    -PackageSaveMode nuspec ^
    -OutputDirectory %CI_CD_BUILDTOOLS_ROOT%

REM WINDOWS_SDK_CPP_x64
"%NUGET_EXE%" install %WINDOWS_SDK_PACKAGE%.CPP.x64 ^
    -Version %WINDOWS_SDK_VERSION% ^
    -source %CI_CD_BUILDTOOLS_URL% ^
    -NonInteractive ^
    -ForceEnglishOutput ^
    -PackageSaveMode nuspec ^
    -OutputDirectory %CI_CD_BUILDTOOLS_ROOT%

REM DOTNET_SDK
"%NUGET_EXE%" install %DOTNET_SDK_PACKAGE% ^
    -Version %DOTNET_SDK_VERSION% ^
    -source %CI_CD_BUILDTOOLS_URL% ^
    -NonInteractive ^
    -ForceEnglishOutput ^
    -PackageSaveMode nuspec ^
    -OutputDirectory %CI_CD_BUILDTOOLS_ROOT%

REM pgutil
dotnet tool install ^
    --global pgutil ^
    --add-source %CI_CD_BUILDTOOLS_URL%

