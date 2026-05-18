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

# Launch DOSBox-X in a background job so we can stream the log in this thread.
# Start-Process -ArgumentList just joins with spaces (no per-element quoting),
# so the -c "command /c ..." arg breaks. Use a job instead: PS splatting inside
# the job uses the native argument-passing path which quotes correctly.
# Serialize the args array as JSON to carry it across the job boundary.
$argsJson = $dosboxArgs | ConvertTo-Json -Compress
$dosboxJob = Start-Job -ScriptBlock {
    param($exe, $json, $dir)
    Set-Location -LiteralPath $dir
    $a = $json | ConvertFrom-Json
    & $exe @a *> $null
} -ArgumentList $dosboxX, $argsJson, $PSScriptRoot

# Tail build.log in real time (equivalent to bash's `tail -F $log`).
# DOSBox-X writes lines incrementally; after the job exits keep draining
# for up to 10 seconds to catch any late-flushed lines.
$stream = [System.IO.FileStream]::new(
    (Resolve-Path -LiteralPath $log).Path,
    [System.IO.FileMode]::Open,
    [System.IO.FileAccess]::Read,
    [System.IO.FileShare]::ReadWrite
)
$reader = [System.IO.StreamReader]::new($stream)
$exitedAt = $null
try {
    for (;;) {
        $line = $reader.ReadLine()
        if ($null -ne $line) {
            Write-Host $line
            if ($line -match 'Build succeeded\.' -or $line -match '\*\*\* Error') { break }
        } elseif ($dosboxJob.State -in 'Completed','Failed','Stopped') {
            if ($null -eq $exitedAt) { $exitedAt = Get-Date }
            if (((Get-Date) - $exitedAt).TotalSeconds -ge 10) { break }
            Start-Sleep -Milliseconds 100
        } else {
            Start-Sleep -Milliseconds 100
        }
    }
} finally {
    $reader.Dispose()
    $stream.Dispose()
}

Remove-Job $dosboxJob -Force -ErrorAction SilentlyContinue

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
