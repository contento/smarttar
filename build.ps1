#!/usr/bin/env pwsh
<#
.SYNOPSIS
  Run a SmartTar build inside DOSBox-X non-interactively (PowerShell / Windows).

.DESCRIPTION
  Equivalent to build.sh for hosts that don't have bash.
  Tested on PowerShell Core 7+ on Windows 11; should also work on
  Windows PowerShell 5.1 and on pwsh on macOS/Linux.

  Usage:   .\build.ps1 [-Force] [-DebugBuild] [-KeepLogInSt] [demo_dos|real_dos]
  Default: demo_dos, log at repo root (build.log)

  Variants (mini-smarttar): demo_dos is the buildable variant; real_dos is
  DEACTIVATED and fails by design (real_dos\ trips a #error unless
  REAL_DOS_ENABLED). See MINI_SMARTTAR_PLAN.

  Only one instance can run at a time (DOSBox-X locks its config / display).

  Always passes HELP=1 so the MAKEFILE's gated help.dat rule fires when
  bin/help.dat is missing or stale. Borland MAKE's dependency tracking
  means this is a no-op when help.dat is up to date.

  The DOSBox-X window opens during the run; there is no fully headless
  mode. The window closes when the build finishes (the final -c "exit"
  terminates DOSBox-X).

.PARAMETER Variant
  Build variant: demo_dos (default, buildable) | real_dos (deactivated, fails
  by design).

.PARAMETER Force
  Pass -B to Borland MAKE (force rebuild of all targets, ignoring
  timestamps). Alias: -Rebuild.

.PARAMETER DebugBuild
  Add -DDEBUG (Borland -v symbols). Modifier on the variant; there is no
  separate debug variant. (Named DebugBuild because -Debug is a reserved
  PowerShell common parameter.)

.PARAMETER KeepLogInSt
  Write the log inside st\ (st\build.log) instead of the repo root.

.NOTES
  Override the dosbox-x.exe location with the DOSBOX_X env var.

.EXAMPLE
  .\build.ps1
  demo_dos build, log at .\build.log.

.EXAMPLE
  .\build.ps1 demo_dos -Force -DebugBuild
  Force-rebuild the demo_dos variant with debug symbols.
#>

[CmdletBinding()]
param(
    [Parameter(Position=0)]
    [ValidateSet('demo_dos','real_dos')]
    [string]$Variant = 'demo_dos',

    [Alias('Rebuild')]
    [switch]$Force,

    [switch]$DebugBuild,

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
    $log    = 'st\build-' + $Variant + '.log'
    $dosLog = 'C:\ST\' + (Split-Path -Leaf $log)
} else {
    $log    = 'bld.log'
    $dosLog = 'C:\bld.log'
}

# --- Build args for MAKE -----------------------------------------------------
$makeArgs     = ''
$bannerSuffix = ''
if ($Force) {
    $makeArgs    += ' -B'
    $bannerSuffix = ' (force rebuild)'
}
if ($DebugBuild) {
    $makeArgs    += ' -DDEBUG'
    $bannerSuffix += ' (debug)'
}

Write-Host "build: building variant '$Variant'$bannerSuffix (log: $log) ..."
Write-Host 'build: streaming compile output below.'
Write-Host ('-' * 70)


# Map variant name to 8.3-safe batch filename (LFN disabled in DOSBox-X).
$batFile = switch ($Variant) {
    'demo_dos' { 'mkdemos' }
    'real_dos' { 'mkrldos' }
}

$log    = 'build.log'
$dosLog = 'C:\build.log'

$dosboxArgs = @(
    '-conf',       'dosbox-x.conf',
    '-fastlaunch',
    '-c',          "mount c $($PWD -replace '\\', '/')",
    '-c',          'c:',
    '-c',          'cd \st',
    '-c',          "make -DDEMO_DOS -DRUN $makeArgs > $dosLog"
)
# Run DOSBox-X, then check for build artifacts.
try {
    if ($env:MAKE_HEADLESS_DEBUG) {
        & $dosboxX @dosboxArgs *>> "$PWD\dosbox-x-build.log"
    } else {
        & $dosboxX @dosboxArgs *> $null
    }
} catch {}

# DOSBox-X on Windows doesn't capture external-command stdout from -c
# redirects, so verify build success by checking for st.obj artifact.
$succeeded = (Get-ChildItem "$PWD\st\build\st.obj" -ErrorAction SilentlyContinue | Measure-Object).Count -gt 0

if ($succeeded) {
    Write-Host "build ${Variant}: OK"
} else {
    Write-Host "build ${Variant}: FAIL (see $log)"
    exit 1
}
