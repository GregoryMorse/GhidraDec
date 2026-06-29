param(
    [string]$IdaDir = "C:\Program Files\IDA Professional 9.3",
    [string]$Database = "",
    [string]$InputFile = "",
    [string]$GhidraDir = "C:\Users\Gregory\Desktop\Apps\ghidra_12.1.2_PUBLIC",
    [string]$PluginDll = "",
    [string]$PluginName = "ghidradec64",
    [string]$WorkDir = "",
    [string]$Cspec = "",
    [string]$Pspec = "",
    [string]$Sleigh = "",
    [int]$MaxFunctions = 1,
    [int]$StartIndex = 0,
    [string]$TargetEa = "",
    [int]$MinFunctionBytes = 4,
    [int]$MaxFunctionBytes = 512,
    [ValidateSet("IDC", "Python")]
    [string]$ScriptKind = "Python",
    [switch]$InstallPlugin,
    [switch]$InstallUserPlugin,
    [switch]$UseAbsolutePluginPath,
    [switch]$EnableCrashDumps,
    [switch]$Gui,
    [int]$GuiSendEnterSeconds = 0,
    [int]$DialogAutomationSeconds = 0,
    [int]$PostExitDialogAutomationSeconds = 15,
    [switch]$Debugger,
    [string]$DebuggerPath = "",
    [int]$ProcessTimeoutSeconds = 0,
    [int]$PassExitGraceSeconds = 3,
    [switch]$RunParamIdentification,
    [switch]$RefreshWorkDatabase
)

$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
if ([string]::IsNullOrWhiteSpace($WorkDir)) {
    $WorkDir = Join-Path $repoRoot "tests\ida93\work"
}
if ([string]::IsNullOrWhiteSpace($PluginDll)) {
    $PluginDll = Join-Path $repoRoot "x64\Release\ghidradec64.dll"
}
if ([string]::IsNullOrWhiteSpace($InputFile)) {
    $InputFile = [System.IO.Path]::Combine(
        [System.IO.Path]::GetDirectoryName($Database),
        [System.IO.Path]::GetFileNameWithoutExtension($Database))
}

$idaExe = Join-Path $IdaDir $(if ($Gui) { "ida.exe" } else { "idat.exe" })
$pythonScript = Join-Path $PSScriptRoot "ghidradec_ida_smoke.py"
$idcTemplate = Join-Path $PSScriptRoot "ghidradec_ida_smoke.idc.in"

if (!(Test-Path -LiteralPath $idaExe)) {
    throw "IDA text executable was not found: $idaExe"
}
if (!(Test-Path -LiteralPath $Database)) {
    throw "IDA database was not found: $Database"
}
if (!(Test-Path -LiteralPath $InputFile)) {
    throw "Input binary was not found: $InputFile"
}
if (!(Test-Path -LiteralPath $GhidraDir)) {
    throw "Ghidra directory was not found: $GhidraDir"
}
if (!(Test-Path -LiteralPath $PluginDll)) {
    throw "Plugin DLL was not found: $PluginDll"
}

$PluginDll = (Resolve-Path -LiteralPath $PluginDll).Path

New-Item -ItemType Directory -Force -Path $WorkDir | Out-Null

$workDb = Join-Path $WorkDir ([System.IO.Path]::GetFileName($Database))
$workInput = Join-Path $WorkDir ([System.IO.Path]::GetFileName($InputFile))
$log = Join-Path $WorkDir "ida93-smoke.log"
$dumpDir = Join-Path $WorkDir "dumps"
$debugLog = Join-Path $WorkDir "ida93-debugger.log"
$debugDump = Join-Path $dumpDir "ida93-debugger.dmp"

if ($InstallPlugin) {
    $pluginTarget = Join-Path (Join-Path $IdaDir "plugins") "ghidradec64.dll"
    Copy-Item -LiteralPath $PluginDll -Destination $pluginTarget -Force
    Write-Host "Installed plugin to $pluginTarget"
}
if ($InstallUserPlugin) {
    $userPluginDir = Join-Path $env:APPDATA "Hex-Rays\IDA Pro\plugins"
    New-Item -ItemType Directory -Force -Path $userPluginDir | Out-Null
    $pluginTarget = Join-Path $userPluginDir "ghidradec64.dll"
    Copy-Item -LiteralPath $PluginDll -Destination $pluginTarget -Force
    Write-Host "Installed plugin to $pluginTarget"
}

if ($RefreshWorkDatabase -or !(Test-Path -LiteralPath $workDb)) {
    Copy-Item -LiteralPath $Database -Destination $workDb -Force
    Write-Host "Copied test database to $workDb"
}
if ($RefreshWorkDatabase -or !(Test-Path -LiteralPath $workInput)) {
    Copy-Item -LiteralPath $InputFile -Destination $workInput -Force
    Write-Host "Copied test input to $workInput"
}

$workBase = Join-Path $WorkDir ([System.IO.Path]::GetFileNameWithoutExtension($Database))
$sidecarSuffixes = @(".id0", ".id1", ".id2", ".nam", ".til", ".i64.json", ".c", ".dec-backup.i64")
foreach ($suffix in $sidecarSuffixes) {
    $sidecar = $workBase + $suffix
    if (Test-Path -LiteralPath $sidecar) {
        Remove-Item -LiteralPath $sidecar -Force
    }
}
if (Test-Path -LiteralPath $log) {
    Remove-Item -LiteralPath $log -Force
}
if ($EnableCrashDumps) {
    New-Item -ItemType Directory -Force -Path $dumpDir | Out-Null
    foreach ($exeName in @("ida.exe", "idat.exe")) {
        $key = "HKCU:\Software\Microsoft\Windows\Windows Error Reporting\LocalDumps\$exeName"
        New-Item -Path $key -Force | Out-Null
        New-ItemProperty -Path $key -Name DumpFolder -Value $dumpDir -PropertyType ExpandString -Force | Out-Null
        New-ItemProperty -Path $key -Name DumpType -Value 2 -PropertyType DWord -Force | Out-Null
        New-ItemProperty -Path $key -Name DumpCount -Value 10 -PropertyType DWord -Force | Out-Null
    }
    Write-Host "Crash dumps enabled at $dumpDir"
}

$env:GHIDRADEC_TEST_GHIDRA = $GhidraDir
$env:GHIDRADEC_GHIDRA_DIR = $GhidraDir
$env:GHIDRA_INSTALL_DIR = $GhidraDir
$env:GHIDRADEC_TEST_INPUT_PATH = $workInput
$env:GHIDRADEC_TEST_MAX_FUNCS = [string]$MaxFunctions
$env:GHIDRADEC_TEST_FUNC_START = [string]$StartIndex
$env:GHIDRADEC_TEST_TARGET_EA = $TargetEa
$env:GHIDRADEC_TEST_MIN_FUNC_BYTES = [string]$MinFunctionBytes
$env:GHIDRADEC_TEST_MAX_FUNC_BYTES = [string]$MaxFunctionBytes
$env:GHIDRADEC_TEST_OUTPUT_TIMEOUT = [string]$(if ($ProcessTimeoutSeconds -gt 10) { $ProcessTimeoutSeconds - 5 } else { 60 })
$env:GHIDRADEC_TEST_CLEAN_OUTPUT = "1"
$env:GHIDRADEC_TEST_PLUGIN = $(if ($UseAbsolutePluginPath) { $PluginDll } else { $PluginName })
$env:GHIDRADEC_TEST_SKIP_PARAMID = $(if ($RunParamIdentification) { "0" } else { "1" })
if ($ScriptKind -eq "Python") {
    $env:GHIDRADEC_TEST_ASYNC = "1"
}
if (![string]::IsNullOrWhiteSpace($Cspec)) {
    $env:GHIDRADEC_CSPEC = $Cspec
}
if (![string]::IsNullOrWhiteSpace($Pspec)) {
    $env:GHIDRADEC_PSPEC = $Pspec
}
if (![string]::IsNullOrWhiteSpace($Sleigh)) {
    $env:GHIDRADEC_SLEIGH = $Sleigh
}

function ConvertTo-IdcString([string]$Value) {
    '"' + $Value.Replace('\', '\\').Replace('"', '\"') + '"'
}

function Ensure-WindowAutomationType {
    if ("IdaWindowAutomation.NativeMethods" -as [type]) {
        return
    }

    Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
using System.Text;

namespace IdaWindowAutomation {
    public static class NativeMethods {
        public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

        [DllImport("user32.dll")]
        public static extern bool EnumWindows(EnumWindowsProc lpEnumFunc, IntPtr lParam);

        [DllImport("user32.dll")]
        public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint processId);

        [DllImport("user32.dll", CharSet = CharSet.Unicode)]
        public static extern int GetWindowText(IntPtr hWnd, StringBuilder text, int count);

        [DllImport("user32.dll", CharSet = CharSet.Unicode)]
        public static extern int GetClassName(IntPtr hWnd, StringBuilder className, int count);

        [DllImport("user32.dll")]
        public static extern bool IsWindowVisible(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern bool SetForegroundWindow(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern bool PostMessage(IntPtr hWnd, uint msg, IntPtr wParam, IntPtr lParam);

        [DllImport("user32.dll")]
        public static extern IntPtr SendMessage(IntPtr hWnd, uint msg, IntPtr wParam, IntPtr lParam);
    }
}
"@
}

function Get-ProcessWindows([int]$ProcessId = 0) {
    Ensure-WindowAutomationType
    $windows = New-Object System.Collections.Generic.List[object]
    $callback = [IdaWindowAutomation.NativeMethods+EnumWindowsProc]{
        param([IntPtr]$hwnd, [IntPtr]$lparam)

        if (![IdaWindowAutomation.NativeMethods]::IsWindowVisible($hwnd)) {
            return $true
        }

        [uint32]$windowPid = 0
        [void][IdaWindowAutomation.NativeMethods]::GetWindowThreadProcessId($hwnd, [ref]$windowPid)
        if ($ProcessId -ne 0 -and $windowPid -ne [uint32]$ProcessId) {
            return $true
        }

        $titleBuilder = New-Object System.Text.StringBuilder 512
        $classBuilder = New-Object System.Text.StringBuilder 256
        [void][IdaWindowAutomation.NativeMethods]::GetWindowText($hwnd, $titleBuilder, $titleBuilder.Capacity)
        [void][IdaWindowAutomation.NativeMethods]::GetClassName($hwnd, $classBuilder, $classBuilder.Capacity)

        $processName = ""
        try {
            $processName = (Get-Process -Id ([int]$windowPid) -ErrorAction Stop).ProcessName
        }
        catch {
        }

        $windows.Add([pscustomobject]@{
            Handle = $hwnd
            ProcessId = [int]$windowPid
            ProcessName = $processName
            Title = $titleBuilder.ToString()
            ClassName = $classBuilder.ToString()
        })
        return $true
    }

    [void][IdaWindowAutomation.NativeMethods]::EnumWindows($callback, [IntPtr]::Zero)
    return $windows
}

function Invoke-IdaDialogAutomation([Nullable[int]]$ProcessId, [int]$Seconds, [switch]$AllowGlobalCrashWindows) {
    if ($Seconds -le 0) {
        return
    }

    Ensure-WindowAutomationType
    $seen = @{}
    $patterns = @(
        "license",
        "not yet accepted",
        "python 3 is not configured",
        "ida has encountered a problem",
        "encountered a problem",
        "minidump",
        "crash",
        "access violation",
        "application error",
        "unpacked version",
        "restore packed",
        "did not close properly",
        "warning",
        "hex-rays"
    )
    $processPatterns = @("ida", "idat", "werfault")

    for ($i = 0; $i -lt $Seconds; $i++) {
        Start-Sleep -Seconds 1
        $windows = if ($AllowGlobalCrashWindows) {
            Get-ProcessWindows
        }
        elseif ($ProcessId.HasValue) {
            Get-ProcessWindows -ProcessId $ProcessId.Value
        }
        else {
            @()
        }

        foreach ($window in $windows) {
            $title = $window.Title
            $className = $window.ClassName
            $processName = $window.ProcessName
            $haystack = ($title + " " + $className + " " + $processName).ToLowerInvariant()
            $matched = $false
            $processAllowed = $false
            foreach ($pattern in $processPatterns) {
                if ($processName.ToLowerInvariant() -eq $pattern) {
                    $processAllowed = $true
                    break
                }
            }
            foreach ($pattern in $patterns) {
                if ($haystack.Contains($pattern)) {
                    $matched = $true
                    break
                }
            }

            if (!$matched -and !($className -eq "#32770" -and $processAllowed)) {
                continue
            }
            if (!$processAllowed -and !$matched) {
                continue
            }

            $key = $window.Handle.ToInt64()
            if (!$seen.ContainsKey($key)) {
                $seen[$key] = $true
                Write-Host ("Dialog automation: sending Enter/OK to hwnd=0x{0:X} pid={1} proc='{2}' class='{3}' title='{4}'" -f $key, $window.ProcessId, $processName, $className, $title)
            }

            [void][IdaWindowAutomation.NativeMethods]::SetForegroundWindow($window.Handle)
            [void][IdaWindowAutomation.NativeMethods]::SendMessage($window.Handle, 0x0111, [IntPtr]1, [IntPtr]::Zero)
            [void][IdaWindowAutomation.NativeMethods]::PostMessage($window.Handle, 0x0100, [IntPtr]0x0D, [IntPtr]::Zero)
            [void][IdaWindowAutomation.NativeMethods]::PostMessage($window.Handle, 0x0101, [IntPtr]0x0D, [IntPtr]::Zero)
        }
    }
}

function Find-Debugger {
    param([string]$RequestedPath)

    if (![string]::IsNullOrWhiteSpace($RequestedPath)) {
        if (!(Test-Path -LiteralPath $RequestedPath)) {
            throw "DebuggerPath was not found: $RequestedPath"
        }
        return (Resolve-Path -LiteralPath $RequestedPath).Path
    }

    foreach ($name in @("cdb.exe", "windbg.exe")) {
        $cmd = Get-Command $name -ErrorAction SilentlyContinue
        if ($cmd) {
            return $cmd.Source
        }
    }

    $roots = @(
        "C:\Program Files (x86)\Windows Kits\10\Debuggers",
        "C:\Program Files\Windows Kits\10\Debuggers",
        "C:\Program Files\Microsoft Visual Studio\2022"
    )
    foreach ($root in $roots) {
        if (!(Test-Path -LiteralPath $root)) {
            continue
        }
        $candidate = Get-ChildItem -LiteralPath $root -Recurse -File -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -in @("cdb.exe", "windbg.exe") } |
            Select-Object -First 1
        if ($candidate) {
            return $candidate.FullName
        }
    }

    throw "Could not find cdb.exe or windbg.exe. Pass -DebuggerPath <path-to-debugger>."
}

if ($ScriptKind -eq "IDC") {
    $pluginForScript = if ($UseAbsolutePluginPath) { $PluginDll } else { $PluginName }
    $generatedIdc = Join-Path $WorkDir "ghidradec_ida_smoke.generated.idc"
    $idc = Get-Content -LiteralPath $idcTemplate -Raw
    $idc = $idc.Replace("@PLUGIN_DLL@", (ConvertTo-IdcString $pluginForScript))
    $idc = $idc.Replace("@INPUT_FILE@", (ConvertTo-IdcString $workInput))
    $idc = $idc.Replace("@OUTPUT_FILE@", (ConvertTo-IdcString ($workInput + ".c")))
    $idc = $idc.Replace("@MAX_FUNCS@", [string]$MaxFunctions)
    $idc = $idc.Replace("@START_INDEX@", [string]$StartIndex)
    $idc = $idc.Replace("@MIN_FUNC_BYTES@", [string]$MinFunctionBytes)
    $idc = $idc.Replace("@MAX_FUNC_BYTES@", [string]$MaxFunctionBytes)
    Set-Content -LiteralPath $generatedIdc -Value $idc -Encoding ASCII
    $script = $generatedIdc
}
else {
    $script = $pythonScript
}

Write-Host "Running IDA smoke test; log: $log"
$idaArgs = @()
if (!$Gui) {
    $idaArgs += "-A"
}
$idaArgs += "-L$log"
$idaArgs += "-S$script"
$idaArgs += $workDb

if ($Debugger) {
    New-Item -ItemType Directory -Force -Path $dumpDir | Out-Null
    if (Test-Path -LiteralPath $debugLog) {
        Remove-Item -LiteralPath $debugLog -Force
    }
    if (Test-Path -LiteralPath $debugDump) {
        Remove-Item -LiteralPath $debugDump -Force
    }
    $debuggerExe = Find-Debugger -RequestedPath $DebuggerPath
    $debuggerCommands = ".symfix; .sympath+ `"$repoRoot\x64\Release`"; .reload; sxe av; g; .ecxr; kb; lmvm ghidradec64; !analyze -v; .dump /ma `"$debugDump`"; q"
    $debuggerArgs = @("-logo", $debugLog, "-o", "-g", "-G", "-c", $debuggerCommands, $idaExe) + $idaArgs
    Write-Host "Running under debugger: $debuggerExe"
    & $debuggerExe @debuggerArgs
    $exitCode = $LASTEXITCODE
}
elseif ($Gui) {
    $proc = Start-Process -FilePath $idaExe -ArgumentList $idaArgs -PassThru
    if ($DialogAutomationSeconds -gt 0) {
        Invoke-IdaDialogAutomation -ProcessId $proc.Id -Seconds $DialogAutomationSeconds -AllowGlobalCrashWindows
    }
    if ($GuiSendEnterSeconds -gt 0) {
        $shell = New-Object -ComObject WScript.Shell
        for ($i = 0; $i -lt $GuiSendEnterSeconds -and !$proc.HasExited; $i++) {
            Start-Sleep -Seconds 1
            if ($shell.AppActivate($proc.Id)) {
                $shell.SendKeys("{ENTER}")
            }
        }
    }
    if ($ProcessTimeoutSeconds -gt 0) {
        $deadline = (Get-Date).AddSeconds($ProcessTimeoutSeconds)
        $exitCode = $null
        while (!$proc.HasExited -and (Get-Date) -lt $deadline) {
            $loggedPass = (Test-Path -LiteralPath $log) -and
                (Select-String -LiteralPath $log -Pattern "[ghidradec-smoke] PASS:" -SimpleMatch -Quiet)
            if ($loggedPass) {
                if ($PassExitGraceSeconds -gt 0) {
                    Start-Sleep -Seconds $PassExitGraceSeconds
                }
                if (!$proc.HasExited) {
                    Write-Host "IDA smoke script logged PASS; stopping lingering GUI process $($proc.Id)."
                    Stop-Process -Id $proc.Id -Force
                }
                $exitCode = 0
                break
            }
            Start-Sleep -Seconds 1
        }
        if ($null -eq $exitCode) {
            if (!$proc.HasExited) {
                Write-Host "IDA smoke test timed out after $ProcessTimeoutSeconds seconds; stopping process $($proc.Id)"
                Stop-Process -Id $proc.Id -Force
                $exitCode = 124
            }
            else {
                $exitCode = $proc.ExitCode
            }
        }
    }
    else {
        $proc.WaitForExit()
        $exitCode = $proc.ExitCode
    }
    if ($PostExitDialogAutomationSeconds -gt 0) {
        Invoke-IdaDialogAutomation -ProcessId $null -Seconds $PostExitDialogAutomationSeconds -AllowGlobalCrashWindows
    }
}
else {
    & $idaExe @idaArgs
    $exitCode = $LASTEXITCODE
}

if ($exitCode -ne 0) {
    Write-Host "IDA smoke test failed with exit code $exitCode"
    if (Test-Path -LiteralPath $log) {
        Get-Content -LiteralPath $log -Tail 80
    }
    exit $exitCode
}

Write-Host "IDA smoke test passed"
if (Test-Path -LiteralPath $log) {
    Get-Content -LiteralPath $log -Tail 40
}
