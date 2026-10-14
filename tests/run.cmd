@echo off
setlocal
call "%~dp0setup-msvc.cmd"
if errorlevel 1 exit /b 1
cd /d "%~dp0"
if not exist build mkdir build
cl /nologo /EHsc /W4 /WX workflow_tests.cpp ..\src\core\SoundWorkflow.cpp ..\src\ui\Locale.cpp /Fe:build\workflow_tests.exe /Fo:build\
if errorlevel 1 exit /b 1
build\workflow_tests.exe
exit /b %errorlevel%
