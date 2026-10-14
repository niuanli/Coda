param([string]$PythonPath = "python")
$ErrorActionPreference = 'Stop'
& $PythonPath (Join-Path $PSScriptRoot 'studio_assets.py')
if ($LASTEXITCODE -ne 0) { throw 'Studio asset generation failed. Python with Pillow is required.' }
