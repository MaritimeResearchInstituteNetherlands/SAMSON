@ECHO OFF
REM check if we have a python interpreter
python --version  > NUL 2>&1 || GOTO :NOPY
python build_docs.py --debug
GOTO :END

:NOPY
ECHO No python interpreter found.
ECHO Maybe you should activate a conda environment first.
ECHO Exiting.
exit /b 1

:END