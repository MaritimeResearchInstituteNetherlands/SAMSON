call %~dp0..\scripts\CI_CD_set_vars_local.cmd

rem set "url=https://github.com/git-for-windows/git/releases/download/v2.47.1.windows.2/PortableGit-2.47.1.2-64-bit.7z.exe"
rem set sha512=a96dd809b2dc940d871a84a2c3f2f4600552fe67f437ffeb29abb3b3da636f3534fa3118af61fca9349e97ec8dbb54210c213dee0c8beadbd9ba27a6c091dd98
rem set "dst=%VCPKG_ROOT%\downloads\PortableGit-2.47.1.2-64-bit.7z.exe.2476.part"
rem call %~dp0asset-provider.bat %url% %sha512% %dst%

set url=http://download.osgeo.org/shapelib/shapelib-1.6.0.zip
set sha512=f3f43f2028fe442e020558de2559b24eae9c7a1d0c84cc242f23ea985cf1fb5ff39fbfef7738f9b8ef5df9a5d0b9f3e891a61b3d5fbbe5b224f41a46589723a3
set dst=%VCPKG_ROOT%\downloads\shapelib-1.6.0.zip.5420.part
call %~dp0asset-provider.bat %url% %sha512% %dst%

rem set url=https://github.com/PowerShell/PowerShell/releases/download/v7.2.24/PowerShell-7.2.24-win-x64.zip
rem set sha512=a08b72958f5a552240d3f68c581d8c8cb580468a71f5e55ca54a1dd0c0fcd81da9df11036653e2300fc4a5778a77c0147832ca06f7837f03417e9795e577a76f
rem set dst=%VCPKG_ROOT%\downloads\PowerShell-7.2.24-win-x64.zip.8464.part
rem call %~dp0asset-provider.bat %url% %sha512% %dst%
