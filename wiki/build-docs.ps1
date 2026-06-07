#!/usr/bin/env pwsh
<#
.SYNOPSIS
  Regenerate SmartTar documentation outputs from the Obsidian vault.

.DESCRIPTION
  The vault (es/, en/, *.md) is the single source of truth for user-facing
  documentation. This script produces, into _build/:
    - the four .docx manuals + the STC doc (pandoc, Markdown -> docx)

  NOTE: st/res/help.txt is a source asset for the app (consumed by genhelp),
  NOT a wiki output. It lives in st/res/ and is edited directly.

  Outputs land in _build/ rather than overwriting tracked files so you can
  diff before promoting.

.PARAMETER OutDir
  Override the output directory. Defaults to _build/ inside the wiki folder.

.EXAMPLE
  ./build-docs.ps1
  ./build-docs.ps1 -OutDir ./my-output
#>
[CmdletBinding()]
param(
    [string]$OutDir
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$WikiDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Manifest = Join-Path $WikiDir 'manifest.json'

if (-not (Test-Path $Manifest)) {
    Write-Error "Manifest not found: $Manifest"
    exit 1
}

$manifestData = Get-Content $Manifest -Raw | ConvertFrom-Json

if (-not $OutDir) {
    $OutDir = Join-Path $WikiDir $manifestData.outputs_dir
}
New-Item -ItemType Directory -Path $OutDir -Force | Out-Null

# Check pandoc
if (-not (Get-Command pandoc -ErrorAction SilentlyContinue)) {
    Write-Error "pandoc not found. Install via: winget install pandoc  # or: brew install pandoc"
    exit 1
}

function Strip-Frontmatter {
    <#
    .SYNOPSIS
    Remove leading YAML frontmatter (--- ... ---) from a markdown file.
    #>
    param([string]$Path)
    $lines = Get-Content $Path -Encoding UTF8
    $inFrontmatter = $false
    $startLine = 0

    if ($lines.Count -gt 0 -and $lines[0].Trim() -eq '---') {
        $inFrontmatter = $true
        $startLine = 1
        for ($i = 1; $i -lt $lines.Count; $i++) {
            if ($lines[$i].Trim() -eq '---') {
                $startLine = $i + 1
                $inFrontmatter = $false
                break
            }
        }
    }

    if ($startLine -lt $lines.Count) {
        $lines[$startLine..($lines.Count - 1)] -join "`n"
    }
}

Write-Host "==> Building manuals (Markdown -> docx)"

foreach ($manual in $manifestData.manuals) {
    $src = Join-Path $WikiDir $manual.src_dir
    $outFile = Join-Path $OutDir $manual.output

    # Build the combined markdown with title page
    $parts = @()
    $parts += "# $($manual.title)`n"
    $parts += "**$($manual.subtitle)**\\"
    $parts += "$($manual.vendor)\\"
    $parts += "$($manual.copyright)`n"
    $parts += "---`n"

    # Collect chapter files (NN-*.md pattern)
    $chapters = Get-ChildItem -Path $src -Filter '[0-9]*.md' -ErrorAction SilentlyContinue |
        Sort-Object Name

    if ($chapters.Count -gt 0) {
        foreach ($ch in $chapters) {
            $content = Strip-Frontmatter -Path $ch.FullName
            $parts += $content
            $parts += "`n"
        }
    }
    else {
        # Single-page docs (e.g. STC)
        $indexMd = Join-Path $src 'index.md'
        if (Test-Path $indexMd) {
            $content = Strip-Frontmatter -Path $indexMd
            $parts += $content
        }
    }

    # Write temp file and run pandoc
    $tmpFile = [System.IO.Path]::GetTempFileName() + '.md'
    try {
        ($parts -join "`n") | Set-Content -Path $tmpFile -Encoding UTF8 -NoNewline
        & pandoc $tmpFile -f gfm -o $outFile
        if ($LASTEXITCODE -ne 0) {
            Write-Error "pandoc failed for $($manual.output) (exit code $LASTEXITCODE)"
            exit 1
        }
        Write-Host "    $($manual.output)"
    }
    finally {
        Remove-Item -Path $tmpFile -Force -ErrorAction SilentlyContinue
    }
}

Write-Host "==> Done. Outputs in $OutDir"
