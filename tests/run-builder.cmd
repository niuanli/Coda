@echo off
setlocal
call "%~dp0setup-msvc.cmd"
if errorlevel 1 exit /b 1
cd /d "%~dp0"
if not exist build mkdir build
cl /nologo /EHsc /W4 /WX builder_tests.cpp ..\src\core\ImageBuilder.cpp /Fe:build\builder_tests.exe /Fo:build\
if errorlevel 1 exit /b 1
build\builder_tests.exe
if errorlevel 1 exit /b 1
"%WINDIR%\Microsoft.NET\Framework64\v4.0.30319\csc.exe" /nologo /out:build\JRunnerReference.exe JRunnerReference.cs
if errorlevel 1 exit /b 1
python compare_reference.py
exit /b %errorlevel%
