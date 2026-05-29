#!/usr/bin/env pwsh
<#
.SYNOPSIS
  Launch SmartTar inside DOSBox-X. Closes DOSBox-X when the app exits.

.DESCRIPTION
  PowerShell counterpart to run.sh. The trailing -c "exit"
  fires the moment st.exe returns control to COMMAND.COM, so closing
  the SmartTar app also closes the DOSBox-X window. Pass -KeepOpen to
  drop into the DOS prompt instead.

  Usage:   .\run.ps1 [-KeepOpen] [-Log [file]] [-- <args>]

  Anything after `--` is forwarded as positional args to st.exe (same
  pass-through as st\run.bat).

  Only one DOSBox-X instance at a time (it locks its config / display).
  Override the dosbox-x.exe path with the DOSBOX_X env var.

.PARAMETER KeepOpen
  After st.exe exits, drop into the DOS prompt instead of closing
  DOSBox-X. Useful when poking at log files or RX.* state without
  relaunching the extender.

.PARAMETER Log
  Capture DOSBox-X output (LOG: messages, E_Exit, etc.) to a file and
  also stream it to the terminal. Defaults to run.log when -Log is given
  without a path. Example: -Log run.log

.PARAMETER StArgs
  Arguments forwarded verbatim to st.exe. PowerShell convention: any
  args after `--` land here.

.NOTES
  Tested on PowerShell Core 7+ on Windows 11; should also work on
  Windows PowerShell 5.1 and on pwsh on macOS/Linux.

.EXAMPLE
  .\run.ps1
  Launch SmartTar; DOSBox-X closes when the app exits (logs to run.log).

.EXAMPLE
  .\run.ps1 -KeepOpen
  Launch SmartTar; leave the DOS prompt up after exit.

.EXAMPLE
  .\run.ps1 -Log custom.log
  Launch SmartTar and capture debug output to custom.log.
#>

[CmdletBinding()]
param(
    [switch]$KeepOpen,
    [string]$Log = 'run.log',

    [Parameter(ValueFromRemainingArguments=$true)]
    [string[]]$StArgs
)

$ErrorActionPreference = 'Stop'
Set-Location -LiteralPath $PSScriptRoot

# --- Locate dosbox-x.exe -----------------------------------------------------
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

if (-not (Test-Path -LiteralPath 'st\bin\st.exe')) {
    Write-Error 'st\bin\st.exe not found. Build first: .\build.ps1 [variant]'
    exit 1
}

# Strip a leading `--` separator if present (PowerShell doesn't consume it).
if ($StArgs -and $StArgs.Count -gt 0 -and $StArgs[0] -eq '--') {
    $StArgs = $StArgs[1..($StArgs.Count - 1)]
}
$stArgString = if ($StArgs) { ($StArgs -join ' ') } else { '' }

# dosbox-x.conf autoexec mounts C: and cd's into ST. Queue: cd bin ->
# run st -> exit. With -KeepOpen, drop the exit so the DOS prompt stays.
$dosboxArgs = @(
    '-conf',       'dosbox-x.conf',
    '-fastlaunch',
    '-c',          'cd bin',
    '-c',          ("st $stArgString").TrimEnd()
)
if (-not $KeepOpen) {
    $dosboxArgs += @('-c', 'exit')
}

if ($Log) {
    Write-Host "Logging to: $Log" -ForegroundColor Cyan
    & $dosboxX @dosboxArgs 2>&1 | Tee-Object -FilePath $Log -Append
    exit $LASTEXITCODE
} else {
    & $dosboxX @dosboxArgs
    exit $LASTEXITCODE
}
