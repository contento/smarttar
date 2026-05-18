#!/usr/bin/env pwsh
<#
.SYNOPSIS
  Run a SmartTar build inside DOSBox-X non-interactively (PowerShell / Windows).

.DESCRIPTION
  Equivalent to make-headless.sh for hosts that don't have bash.
  Tested on PowerShell Core 7+ on Windows 11; should also work on
  Windows PowerShell 5.1 and on pwsh on macOS/Linux.

  Usage:   .\make-headless.ps1 [-Force] [-KeepLogInSt] [demo|dbg|eda|auto|prod]
  Default: demo, log at repo root (build.log)

  Only one instance can run at a time (DOSBox-X locks its config / display).

  Always passes HELP=1 so the MAKEFILE's gated help.dat rule fires when
  bin/help.dat is missing or stale. Borland MAKE's dependency tracking
  means this is a no-op when help.dat is up to date.

  The DOSBox-X window opens during the run; there is no fully headless
  mode. The window closes when the build finishes (the final -c "exit"
  terminates DOSBox-X).

.PARAMETER Variant
  Build variant: demo (default) | dbg | eda | auto | prod.

.PARAMETER Force
  Pass -B to Borland MAKE (force rebuild of all targets, ignoring
  timestamps). Alias: -Rebuild.

.PARAMETER KeepLogInSt
  Write the log inside st\ (st\build.log) instead of the repo root.

.NOTES
  Override the dosbox-x.exe location with the DOSBOX_X env var.

.EXAMPLE
  .\make-headless.ps1
  Demo build, log at .\build.log.

.EXAMPLE
  .\make-headless.ps1 prod -Force
  Force-rebuild the production variant.
#>

[CmdletBinding()]
param(
    [Parameter(Position=0)]
    [ValidateSet('demo','dbg','eda','auto','prod')]
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

if (Test-Path -LiteralPath $log) { Remove-Item -LiteralPath $log -Force }

# --- Build args for MAKE -----------------------------------------------------
$makeArgs     = 'HELP=1'
$bannerSuffix = ''
if ($Force) {
    $makeArgs    += ' -B'
    $bannerSuffix = ' (force rebuild)'
}

Write-Host "make-headless: building variant '$Variant'$bannerSuffix (log: $log) ..."
Write-Host 'make-headless: streaming compile output below; window stays silent.'
Write-Host ('-' * 70)

# Create the log so the tailing job has a file to follow even before
# DOSBox-X starts writing to it.
New-Item -Path $log -ItemType File -Force | Out-Null

# Stream the log live (PowerShell equivalent of `tail -F`).
$tailJob = Start-Job -ArgumentList (Resolve-Path -LiteralPath $log).Path -ScriptBlock {
    param($logPath)
    Get-Content -LiteralPath $logPath -Wait -Tail 0
}

$dosboxArgs = @(
    '-conf',       'dosbox-x.conf',
    '-fastlaunch',
    '-c',          "echo === SmartTar build starting (variant $Variant) ===",
    '-c',          "echo === log: $dosLog (silent until exit) ===",
    '-c',          'echo .',
    '-c',          "command /c make$Variant.bat $makeArgs > $dosLog",
    '-c',          'echo .',
    '-c',          'echo === Build finished ===',
    '-c',          'exit'
)

try {
    if ($env:MAKE_HEADLESS_DEBUG) {
        # Capture DOSBox-X's stdout+stderr to dosbox-x-build.log for CI
        # diagnostics. Default silent path is unchanged for normal use.
        & $dosboxX @dosboxArgs *>&1 | Tee-Object -FilePath 'dosbox-x-build.log'
    } else {
        & $dosboxX @dosboxArgs *> $null
    }
} catch {
    # DOSBox-X exit code is unreliable; we judge success from the log.
}

# DOSBox-X flushes its virtual filesystem to the host only when the file is
# closed (at process exit). Poll until the log has content so we don't read
# an empty file during the brief window between process-exit and OS flush.
$deadline = (Get-Date).AddSeconds(10)
do {
    Start-Sleep -Milliseconds 200
} until ((Test-Path -LiteralPath $log) -and (Get-Item -LiteralPath $log).Length -gt 0 -or (Get-Date) -ge $deadline)

Receive-Job -Job $tailJob -ErrorAction SilentlyContinue
Stop-Job   -Job $tailJob -ErrorAction SilentlyContinue | Out-Null
Remove-Job -Job $tailJob -Force -ErrorAction SilentlyContinue

Write-Host ('-' * 70)

# --- Verdict ----------------------------------------------------------------
if (-not (Test-Path -LiteralPath $log)) {
    Write-Error "build ${Variant}: NO LOG — DOSBox-X did not write to the mount."
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
