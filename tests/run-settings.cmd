@echo off
setlocal
call "%~dp0setup-msvc.cmd"
if errorlevel 1 exit /b 1
cd /d "%~dp0"
if not exist build mkdir build
python generate_settings_test.py
if errorlevel 1 exit /b 1
cl /nologo /EHsc /W4 /WX build\settings_tests.cpp /Fe:build\settings_tests.exe /Fo:build\settings_tests.obj
if errorlevel 1 exit /b 1
build\settings_tests.exe
exit /b %errorlevel%
