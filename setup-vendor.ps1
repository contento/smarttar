#!/usr/bin/env pwsh
<#
.SYNOPSIS
  Clone the smarttar-vendor private repository into .\vendor\

.DESCRIPTION
  The vendor\ directory contains proprietary toolchain binaries required to
  build SmartTar:

    vendor\bc\      Borland C++ 3.1 (compiler, debugger, TASM, TLIB, etc.)
    vendor\pharlap\ Pharlap 286 DOS extender (BCC286, BIND286, CFIG286, ...)
    vendor\zinc\    Zinc 3.5 UI framework (headers, libs, GENHELP.EXE, ...)
    vendor\util\    DOS utilities (Norton Commander, QEdit, PKWARE, etc.)

  These are NOT redistributable and live in a separate private repository
  to avoid copyright issues in the main smarttar repo.

.PARAMETER Force
  Remove existing vendor\ and re-clone.

.EXAMPLE
  .\setup-vendor.ps1
  .\setup-vendor.ps1 -Force
#>

[CmdletBinding()]
param(
    [switch]$Force
)

$ErrorActionPreference = 'Stop'
Set-Location -LiteralPath $PSScriptRoot

$VendorRepo = 'git@github.com:contento/smarttar-vendor.git'
$VendorDir  = 'vendor'

# --- Check if vendor\ already exists -----------------------------------------
if (Test-Path -LiteralPath $VendorDir) {
    if ($Force) {
        Write-Host "setup-vendor: removing existing $VendorDir\ (-Force)"
        Remove-Item -Recurse -Force -LiteralPath $VendorDir
    } else {
        Write-Host "setup-vendor: $VendorDir\ already exists. Use -Force to re-clone."
        exit 0
    }
}

# --- Check for git -----------------------------------------------------------
$git = Get-Command git -ErrorAction SilentlyContinue
if (-not $git) {
    Write-Error 'setup-vendor: git not found on PATH'
    exit 127
}

# --- Clone -------------------------------------------------------------------
Write-Host "setup-vendor: cloning $VendorRepo -> $VendorDir\ ..."
try {
    & git clone $VendorRepo $VendorDir
    Write-Host 'setup-vendor: done. vendor\ is ready.'
    Write-Host ''
    Write-Host 'Contents:'
    Get-ChildItem -LiteralPath $VendorDir -Name
} catch {
    Write-Host ''
    Write-Host 'setup-vendor: clone FAILED. Possible causes:' -ForegroundColor Red
    Write-Host '  1. You don''t have SSH access to github.com/contento/smarttar-vendor'
    Write-Host '  2. The repository is private and your SSH key isn''t configured'
    Write-Host '  3. Network connectivity issues'
    Write-Host ''
    Write-Host 'To fix SSH access:'
    Write-Host '  ssh-add -l                     # check if key is loaded'
    Write-Host '  ssh -T git@github.com          # test GitHub connectivity'
    Write-Host ''
    Write-Host 'Alternatively, you can obtain the toolchain components manually:'
    Write-Host '  - Borland C++ 3.1  (bc\)'
    Write-Host '  - Pharlap 286      (pharlap\)'
    Write-Host '  - Zinc 3.5         (zinc\)'
    Write-Host '  See VENDOR_SETUP.md for details.'
    exit 1
}
