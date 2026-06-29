param(
    [string]$IdaVersion = "",
    [string]$BuildDir = "build",
    [string]$Config = "Release",
    [string]$Generator = "",
    [switch]$Install
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$root = Resolve-Path (Join-Path $PSScriptRoot "..")
Set-Location $root

if ($IdaVersion -eq "") {
    $manifest = Get-Content -Raw -LiteralPath "ghidradec.targets.json" | ConvertFrom-Json
    $IdaVersion = $manifest.project.defaultIdaSdk
}

python tools/ida_sdk.py ensure --version $IdaVersion | Out-Host

$configure = @(
    "-S", ".",
    "-B", $BuildDir,
    "-DGHIDRADEC_IDA_VERSION=$IdaVersion",
    "-DCMAKE_BUILD_TYPE=$Config"
)
if ($Generator -ne "") {
    $configure = @("-G", $Generator) + $configure
}

cmake @configure
cmake --build $BuildDir --config $Config

if ($Install) {
    cmake --build $BuildDir --config $Config --target install
}
