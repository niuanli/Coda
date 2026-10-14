@echo off
if defined VSCMD_VER exit /b 0
set "CODA_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%CODA_VSWHERE%" (
 echo MSVC C++ Build Tools are required for host tests. 1>&2
 exit /b 1
)
set "CODA_VS_PATH="
for /f "usebackq delims=" %%I in (`"%CODA_VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "CODA_VS_PATH=%%I"
if not defined CODA_VS_PATH exit /b 1
call "%CODA_VS_PATH%\VC\Auxiliary\Build\vcvars64.bat" >nul
exit /b %errorlevel%
