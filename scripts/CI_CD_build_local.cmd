@echo off

echo.
echo ===== LOCAL BUILD START =====

call %~dp0CI_CD_set_vars_local.cmd
call %~dp0CI_CD_set_vars_core.cmd

call %~dp0CI_CD_install_buildtools_local.cmd

call %~dp0CI_CD_configure_binary_cache.cmd

call %~dp0CI_CD_build_core.cmd
