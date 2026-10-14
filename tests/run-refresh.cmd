@echo off
setlocal
call "%~dp0setup-msvc.cmd"
if errorlevel 1 exit /b 1
cd /d "%~dp0"
if not exist build mkdir build
python generate_scan_test.py
if errorlevel 1 exit /b 1
cl /nologo /EHsc /W4 /WX build\refresh_tests.cpp /Fe:build\refresh_tests.exe /Fo:build\refresh_tests.obj
if errorlevel 1 exit /b 1
build\refresh_tests.exe
exit /b %errorlevel%
