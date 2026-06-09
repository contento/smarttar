#!/usr/bin/env pwsh
<#
.SYNOPSIS
  Run a SmartTar build inside DOSBox-X non-interactively (PowerShell / Windows).

.DESCRIPTION
  Equivalent to build.sh for hosts that don't have bash.
  Tested on PowerShell Core 7+ on Windows 11; should also work on
  Windows PowerShell 5.1 and on pwsh on macOS/Linux.

  Usage:   .\build.ps1 [-Force] [-KeepLogInSt] [demo|dbg|eda|prod]
  Default: demo, log at repo root (build.log)

  Only one instance can run at a time (DOSBox-X locks its config / display).

  Always passes HELP=1 so the MAKEFILE's gated help.dat rule fires when
  bin/help.dat is missing or stale. Borland MAKE's dependency tracking
  means this is a no-op when help.dat is up to date.

  The DOSBox-X window opens during the run; there is no fully headless
  mode. The window closes when the build finishes (the final -c "exit"
  terminates DOSBox-X).

.PARAMETER Variant
  Build variant: demo (default) | dbg | eda | prod.

.PARAMETER Force
  Pass -B to Borland MAKE (force rebuild of all targets, ignoring
  timestamps). Alias: -Rebuild.

.PARAMETER KeepLogInSt
  Write the log inside st\ (st\build.log) instead of the repo root.

.NOTES
  Override the dosbox-x.exe location with the DOSBOX_X env var.

.EXAMPLE
  .\build.ps1
  Demo build, log at .\build.log.

.EXAMPLE
  .\build.ps1 prod -Force
  Force-rebuild the production variant.
#>

[CmdletBinding()]
param(
    [Parameter(Position=0)]
    [ValidateSet('demo','dbg','eda','prod')]
    [string]$Variant = 'demo',

    [Alias('Rebuild')]
    [switch]$Force,

    [switch]$KeepLogInSt
)

$ErrorActionPreference = 'Stop'
Set-Location -LiteralPath $PSScriptRoot

# --- Locate dosbox-x.exe -----------------------------------------------------
# Env var override, then PATH, then common Windows install locations.
$dosboxX = $env:DOSBOX_X
if (-not $dosboxX) {
    $cmd = Get-Command dosbox-x.exe -ErrorAction SilentlyContinue
    if ($cmd) { $dosboxX = $cmd.Source }
}
if (-not $dosboxX) {
    foreach ($p in @(
        "$env:ProgramFiles\DOSBox-X\dosbox-x.exe",
        "${env:ProgramFiles(x86)}\DOSBox-X\dosbox-x.exe",
        "$env:LOCALAPPDATA\Programs\DOSBox-X\dosbox-x.exe"
    )) {
        if ($p -and (Test-Path -LiteralPath $p)) { $dosboxX = $p; break }
    }
}
if (-not $dosboxX -or -not (Test-Path -LiteralPath $dosboxX)) {
    Write-Error 'dosbox-x.exe not found on PATH (set $env:DOSBOX_X to override).'
    exit 127
}

# --- Log paths ---------------------------------------------------------------
if ($KeepLogInSt) {
    $log    = 'st\build.log'
    $dosLog = 'C:\ST\build.log'
} else {
    $log    = 'build.log'
    $dosLog = 'C:\build.log'
}

# --- Build args for MAKE -----------------------------------------------------
$makeArgs     = ''
$bannerSuffix = ''
if ($Force) {
    $makeArgs    += ' -B'
    $bannerSuffix = ' (force rebuild)'
}

Write-Host "build: building variant '$Variant'$bannerSuffix (log: $log) ..."
Write-Host 'build: streaming compile output below.'
Write-Host ('-' * 70)

# Build output dirs are gitignored and absent on a fresh checkout. C: is the
# repo mount, so creating them host-side makes them visible inside DOSBox-X.
New-Item -ItemType Directory -Force -Path `
    st/build, st/bin, st/lib, st/util/ini2cfg/obj, st/util/inf2dat/obj | Out-Null

# Truncate or create the log. Remove-Item fails when a previous run's
# FileStream (FileShare::ReadWrite, no FileShare::Delete) is still open.
[IO.File]::WriteAllText((Join-Path (Get-Location) $log), '')

$dosboxArgs = @(
    '-conf',       'dosbox-x.conf',
    '-fastlaunch',
    '-c',          "command /c util\ini2cfg\mk_cfg.bat >> $dosLog",
    '-c',          "command /c util\inf2dat\mk_ph.bat >> $dosLog",
    '-c',          "command /c make$Variant.bat $makeArgs >> $dosLog",
    '-c',          'exit'
)

# Tail build.log in a background runspace (in-process thread).
# A -NoNewWindow subprocess shares the console with DOSBox-X and gets killed by
# any console control event (Ctrl+C/Break) DOSBox-X generates. A runspace lives
# inside this PS process and cannot be killed by those events.
$logAbs   = (Resolve-Path -LiteralPath $log).Path
$runspace = [System.Management.Automation.Runspaces.RunspaceFactory]::CreateRunspace()
$runspace.Open()
$tailPS   = [System.Management.Automation.PowerShell]::Create()
$tailPS.Runspace = $runspace
[void]$tailPS.AddScript({
    param($p)
    $s = New-Object IO.FileStream($p, [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::ReadWrite)
    $r = New-Object IO.StreamReader($s)
    while ($true) {
        $l = $r.ReadLine()
        if ($null -ne $l) { [Console]::WriteLine($l) } else { [Threading.Thread]::Sleep(100) }
    }
}).AddArgument($logAbs)
$tailHandle = $tailPS.BeginInvoke()

# DOSBox-X's own stdout/stderr (crash traces, protection faults, startup errors)
# goes here when MAKE_HEADLESS_DEBUG is set. CI sets this for failure diagnostics.
$dxLog = $null
if ($env:MAKE_HEADLESS_DEBUG) { $dxLog = "$PWD\dosbox-x-build.log" }

# Run DOSBox-X in this thread. Redirect its own output (not the DOS >> build.log
# inside -c commands) so crash/protection-fault messages are captured.
try {
    if ($dxLog) {
        & $dosboxX @dosboxArgs *>> $dxLog
    } else {
        & $dosboxX @dosboxArgs *> $null
    }
} catch {}

# Poll the log for the final marker (DOSBox-X may flush the last lines slightly
# after process exit). Up to 10 s; 200 ms intervals.
$logText = ''
$deadline = (Get-Date).AddSeconds(10)
do {
    Start-Sleep -Milliseconds 200
    $logText = Get-Content -LiteralPath $log -Raw -ErrorAction SilentlyContinue
} until (($logText -match 'Build succeeded\.' -or $logText -match '\*\*\* Error') -or (Get-Date) -ge $deadline)

# Give the runspace one final drain cycle, then shut it down.
Start-Sleep -Milliseconds 1200
try { $tailPS.Stop() } catch {}
$runspace.Close()

Write-Host ('-' * 70)

# --- Verdict ----------------------------------------------------------------
if (-not (Test-Path -LiteralPath $log)) {
    Write-Error "build ${Variant}: NO LOG -- DOSBox-X did not write to the mount."
    exit 1
}

$logText = Get-Content -LiteralPath $log -Raw -ErrorAction SilentlyContinue
if ($logText -match 'Build succeeded\.') {
    Write-Host "build ${Variant}: OK (see $log)"
    exit 0
}

Write-Host "build ${Variant}: FAIL (see $log)"
Write-Host "--- last 30 lines of $log ---"
Get-Content -LiteralPath $log -Tail 30
exit 1
