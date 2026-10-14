@echo off
setlocal
call "%~dp0setup-msvc.cmd"
if errorlevel 1 exit /b 1
cd /d "%~dp0"
if not exist build mkdir build
cl /nologo /EHsc /W4 /WX preview_tests.cpp ..\src\core\PreviewDecoder.cpp ..\src\core\ImageBuilder.cpp ..\src\core\SoundWorkflow.cpp /Fe:build\preview_tests.exe /Fo:build\
if errorlevel 1 exit /b 1
build\preview_tests.exe
exit /b %errorlevel%
