#!/usr/bin/env pwsh
<#
.SYNOPSIS
  Bump or set the SmartTar version (PowerShell Core / Windows 11).

.DESCRIPTION
  Equivalent to bump-version.sh.

  Updates:
    - CLAUDE.md      "Current version: **X.Y.Z**"
    - st\versions.txt prepends a [ X.Y.Z ] block with a TODO description stub

  Does NOT commit, tag, or push — see RELEASING.md for the full workflow.

.PARAMETER Action
  One of: patch, minor, major, set.

.PARAMETER Version
  Explicit X.Y.Z string when Action is 'set'. Ignored otherwise.

.PARAMETER DryRun
  Compute the new version and print the plan, but don't write any files.

.EXAMPLE
  .\bump-version.ps1 patch              # 2.34.1 -> 2.34.2

.EXAMPLE
  .\bump-version.ps1 set 2.34.5         # explicit

.EXAMPLE
  .\bump-version.ps1 patch -DryRun
#>

[CmdletBinding()]
param(
    [Parameter(Position=0, Mandatory=$true)]
    [ValidateSet('patch','minor','major','set')]
    [string]$Action,

    [Parameter(Position=1)]
    [string]$Version,

    [switch]$DryRun
)

$ErrorActionPreference = 'Stop'
Set-Location -LiteralPath $PSScriptRoot

# --- Read current version from CLAUDE.md -------------------------------------
$claudeMd = Get-Content -LiteralPath 'CLAUDE.md' -Raw
if ($claudeMd -notmatch 'Current version: \*\*(\d+)\.(\d+)\.(\d+)\*\*') {
    Write-Error "Cannot find 'Current version: **X.Y.Z**' in CLAUDE.md"
    exit 1
}
[int]$major = $Matches[1]
[int]$minor = $Matches[2]
[int]$patch = $Matches[3]
$current = "$major.$minor.$patch"

# --- Compute new version -----------------------------------------------------
switch ($Action) {
    'patch' { $patch++ }
    'minor' { $minor++; $patch = 0 }
    'major' { $major++; $minor = 0; $patch = 0 }
    'set' {
        if (-not $Version -or $Version -notmatch '^(\d+)\.(\d+)\.(\d+)$') {
            Write-Error "Invalid version '$Version' (must be X.Y.Z)"
            exit 1
        }
        [int]$major = $Matches[1]
        [int]$minor = $Matches[2]
        [int]$patch = $Matches[3]
    }
}

$new = "$major.$minor.$patch"
Write-Host "Bumping version: $current -> $new"

if ($current -eq $new) {
    Write-Host 'No change (target equals current).' -ForegroundColor Yellow
    exit 0
}

if ($DryRun) {
    Write-Host '(dry-run; no files written)'
    exit 0
}

# --- Update CLAUDE.md --------------------------------------------------------
$pattern    = "Current version: \*\*$([regex]::Escape($current))\*\*"
$replacement = "Current version: **$new**"
$claudeMd   = [regex]::Replace($claudeMd, $pattern, $replacement)
# Preserve original line endings: Set-Content with -NoNewline + the raw string
# we just rewrote keeps whatever EOL CLAUDE.md had.
Set-Content -LiteralPath 'CLAUDE.md' -Value $claudeMd -NoNewline

# --- Prepend st\versions.txt -------------------------------------------------
$today    = (Get-Date -Format 'MMM dd yyyy')
$existing = Get-Content -LiteralPath 'st\versions.txt' -Raw
# Use LF — st/versions.txt is a host-side text file; .gitattributes leaves
# it CRLF on the DOS side via the *.txt rule, but writing LF here is fine
# because git will normalize on commit.
$header   = "[ $new ]`n - $today : TODO add description.`n `n"
Set-Content -LiteralPath 'st\versions.txt' -Value ($header + $existing) -NoNewline

# --- Report ------------------------------------------------------------------
Write-Host ''
Write-Host 'Updated:'
Write-Host "  CLAUDE.md        (Current version -> $new)"
Write-Host "  st\versions.txt  (new [ $new ] block at top -- please edit the description)"
Write-Host ''
Write-Host 'Next steps (per RELEASING.md):'
Write-Host "  1. Edit st\versions.txt -- replace 'TODO add description.' with the real changelog"
Write-Host '  2. git add CLAUDE.md st\versions.txt'
Write-Host "  3. git commit -m `"Release v$new`: <summary>`""
Write-Host "  4. git tag -a v$new -m `"...`""
Write-Host "  5. git push origin main v$new"
