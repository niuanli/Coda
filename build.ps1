param(
    [string]$SdkPath = $env:CODA_XDK,
    [string]$ArtifactRoot = (Join-Path $PSScriptRoot '..\Coda-artifacts')
)
if (!$SdkPath) { $SdkPath = Join-Path $PSScriptRoot '..\sdk-toolchain\XDK' }
$ErrorActionPreference = 'Stop'
$ArtifactRoot = [System.IO.Path]::GetFullPath($ArtifactRoot)
$sdkRoot = (Resolve-Path -LiteralPath $SdkPath).Path
$toolsRoot = Join-Path $sdkRoot 'bin\win32'
$outputRoot = Join-Path $ArtifactRoot 'build'
$packageRoot = Join-Path $ArtifactRoot 'dist\Coda'
$archivePath = Join-Path $ArtifactRoot 'dist\Coda-0.4.11.zip'
New-Item -ItemType Directory -Force $outputRoot, "$packageRoot\Media", "$packageRoot\Sounds", "$packageRoot\Dumps", "$packageRoot\Imports" | Out-Null
$oldPath = $env:PATH
$oldInclude = $env:INCLUDE
$oldLib = $env:LIB
function Invoke-XdkTool([string]$Name, [string[]]$Arguments) {
    & (Join-Path $toolsRoot $Name) @Arguments
    if ($LASTEXITCODE -ne 0) { throw "$Name failed: $LASTEXITCODE" }
}
Push-Location $PSScriptRoot
try {
    $env:PATH = "$toolsRoot;$oldPath"
    $env:INCLUDE = Join-Path $sdkRoot 'include\xbox'
    $env:LIB = Join-Path $sdkRoot 'lib\xbox'
    Invoke-XdkTool 'xui2bin.exe' @('/NOLOGO', 'assets\skin.xui', "$packageRoot\Media\skin.xur")
    Invoke-XdkTool 'xui2bin.exe' @('/NOLOGO', 'assets\scene.xui', "$packageRoot\Media\scene.xur")
    Copy-Item assets\*.png "$packageRoot\Media" -Force
    Copy-Item assets\font.ttf, assets\FONT-LICENSE.md, assets\SOURCE-SANS-LICENSE.md "$packageRoot\Media" -Force
    Copy-Item research\JRunner.LICENSE "$packageRoot\Media\JRunner-LICENSE.txt" -Force
    Invoke-XdkTool 'lib.exe' @('/nologo', '/machine:PPCBE', '/def:vendor\kernelext.def', "/out:$outputRoot\kernelext.lib")
    $sources = @('src\core\SoundWorkflow.cpp', 'src\core\ImageBuilder.cpp', 'src\core\ImageBuildWorkflow.cpp', 'src\core\PreviewDecoder.cpp', 'src\platform\PreviewPlayer.cpp', 'src\platform\XboxServices.cpp', 'src\ui\Locale.cpp', 'src\ui\StudioScene.cpp', 'src\main.cpp', 'vendor\ISDFlasher.cpp')
    Invoke-XdkTool 'cl.exe' (@('/nologo', '/c', '/W4', '/Ox', '/GL', '/MT', '/GR-', '/GS-', '/D_CRT_SECURE_NO_WARNINGS', '/DNDEBUG', '/D_XBOX', '/DXBOX', "/Fo$outputRoot\") + $sources)
    $objects = @('SoundWorkflow.obj', 'ImageBuilder.obj', 'ImageBuildWorkflow.obj', 'PreviewDecoder.obj', 'PreviewPlayer.obj', 'XboxServices.obj', 'Locale.obj', 'StudioScene.obj', 'main.obj', 'ISDFlasher.obj') | ForEach-Object { Join-Path $outputRoot $_ }
    $libraries = @('xuirunltcg.lib', 'xuirenderltcg.lib', 'd3d9ltcg.lib', 'd3dx9.lib', 'xgraphics.lib', 'xapilib.lib', 'xboxkrnl.lib', 'xbdm.lib', 'xmcoreltcg.lib', 'xnet.lib', 'xaudio2.lib', 'x3daudioltcg.lib')
    Invoke-XdkTool 'link.exe' (@('/nologo', '/machine:PPCBE', '/subsystem:XBOX', '/xex:no', '/ltcg', '/opt:ref', '/opt:icf', '/stack:1048576,262144', "/out:$outputRoot\Coda.exe", "$outputRoot\kernelext.lib") + $objects + $libraries)
    Invoke-XdkTool 'imagexex.exe' @("/in:$outputRoot\Coda.exe", "/out:$packageRoot\Coda.xex", '/config:AppConf.xml')
    if (Test-Path README.md) { Copy-Item README.md "$packageRoot\README.md" -Force }
    if (Test-Path LICENSING.md) { Copy-Item LICENSING.md "$packageRoot\LICENSING.md" -Force }
    if (Test-Path README.ru.md) { Copy-Item README.ru.md "$packageRoot\README.ru.md" -Force }
    if (Test-Path THIRD_PARTY.md) { Copy-Item THIRD_PARTY.md "$packageRoot\THIRD_PARTY.md" -Force }
    Compress-Archive -Path "$packageRoot\*" -DestinationPath $archivePath -Force
    Write-Host "Built: $packageRoot\Coda.xex"
} finally {
    Pop-Location; $env:PATH = $oldPath; $env:INCLUDE = $oldInclude; $env:LIB = $oldLib
}
