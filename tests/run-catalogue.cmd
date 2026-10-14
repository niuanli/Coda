@echo off
setlocal
call "%~dp0setup-msvc.cmd"
if errorlevel 1 exit /b 1
cd /d "%~dp0"
if not exist build mkdir build
cl /nologo /EHsc /W4 /WX catalogue_tests.cpp /Fe:build\catalogue_tests.exe /Fo:build\catalogue_tests.obj
if errorlevel 1 exit /b 1
build\catalogue_tests.exe
exit /b %errorlevel%
