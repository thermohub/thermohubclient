REM call build\test\%CONFIGURATION%\tests.exe
call conda activate thermohubclient
call python -c "import thermohubclient"
call pytest -ra -vv --color=yes .
if errorlevel 1 exit 1
