@echo off
setlocal
for %%T in (run.cmd run-builder.cmd run-build-workflow.cmd run-preview.cmd run-catalogue.cmd run-settings.cmd run-refresh.cmd) do (
 call "%~dp0%%T"
 if errorlevel 1 exit /b 1
)
exit /b 0
