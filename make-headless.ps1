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
Write-Host 'make-headless: streaming compile output below.'
Write-Host ('-' * 70)

# Create the log before launching DOSBox-X.
New-Item -Path $log -ItemType File -Force | Out-Null

$dosboxArgs = @(
    '-conf',       'dosbox-x.conf',
    '-fastlaunch',
    '-c',          "command /c make$Variant.bat $makeArgs > $dosLog",
    '-c',          'exit'
)

# Stream build.log to the terminal via a background process that shares our
# console -- the PS equivalent of bash's `tail -F $log &`. -NoNewWindow makes
# the subprocess inherit our stdout, so its output appears inline.
$pwshExe  = [System.Diagnostics.Process]::GetCurrentProcess().MainModule.FileName
$logAbs   = (Resolve-Path -LiteralPath $log).Path

# Build a StreamReader loop as a base64-encoded command to sidestep quoting.
# Get-Content -Wait can exit early on Windows; a StreamReader loop never does.
$tailSrc = @"
`$s = New-Object IO.FileStream('$logAbs', [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::ReadWrite)
`$r = New-Object IO.StreamReader(`$s)
while (`$true) {
    `$l = `$r.ReadLine()
    if (`$null -ne `$l) { Write-Host `$l } else { [Threading.Thread]::Sleep(100) }
}
"@
$tailEnc  = [Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($tailSrc))
$tailProc = Start-Process -FilePath $pwshExe -ArgumentList "-NoProfile -NonInteractive -EncodedCommand $tailEnc" -PassThru -NoNewWindow

# Run DOSBox-X in this thread -- & uses PS's native arg-passing (correct quoting).
try { & $dosboxX @dosboxArgs *> $null } catch {}

# Poll the log for the final marker (DOSBox-X may flush the last lines slightly
# after process exit). Up to 10 s; 200 ms intervals.
$logText = ''
$deadline = (Get-Date).AddSeconds(10)
do {
    Start-Sleep -Milliseconds 200
    $logText = Get-Content -LiteralPath $log -Raw -ErrorAction SilentlyContinue
} until (($logText -match 'Build succeeded\.' -or $logText -match '\*\*\* Error') -or (Get-Date) -ge $deadline)

# Give the tail process one more poll cycle to print any late-flushed lines.
Start-Sleep -Milliseconds 1200
$tailProc | Stop-Process -Force -ErrorAction SilentlyContinue

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
