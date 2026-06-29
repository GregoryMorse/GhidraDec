param(
    [string]$DumpDir = ""
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
if ([string]::IsNullOrWhiteSpace($DumpDir)) {
    $DumpDir = Join-Path $repoRoot "Tests\ida93\work\dumps"
}

New-Item -ItemType Directory -Force -Path $DumpDir | Out-Null

foreach ($exeName in @("ida.exe", "idat.exe")) {
    $key = "HKCU:\Software\Microsoft\Windows\Windows Error Reporting\LocalDumps\$exeName"
    New-Item -Path $key -Force | Out-Null
    New-ItemProperty -Path $key -Name DumpFolder -Value $DumpDir -PropertyType ExpandString -Force | Out-Null
    New-ItemProperty -Path $key -Name DumpType -Value 2 -PropertyType DWord -Force | Out-Null
    New-ItemProperty -Path $key -Name DumpCount -Value 10 -PropertyType DWord -Force | Out-Null
}

Write-Host "IDA crash dumps will be written to $DumpDir"
