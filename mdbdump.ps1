<#
.SYNOPSIS
    Dump MiniDB .db contents from the st/ directory.
.PARAMETER Options
    Passed through to mdbdump.py (e.g. -b for brief mode).
.DESCRIPTION
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Db = Join-Path $ScriptDir "st" "bin" "RX.db"
$Tool = Join-Path $ScriptDir "st" "util" "lsmdb" "mdbdump.py"

if (-not (Test-Path $Tool)) {
    $Tool = Join-Path $ScriptDir "st" "bin" "mdbdump.py"
}

if (-not (Test-Path $Tool)) {
    Write-Error "mdbdump.py not found (looked in st/util/lsmdb/ and st/bin/)"
    exit 1
}

if (-not (Test-Path $Db)) {
    Write-Error "database not found: $Db"
    Write-Error "Run the app first to generate a database."
    exit 1
}
$python = if (Get-Command python3 -ErrorAction SilentlyContinue) { "python3" }
          elseif (Get-Command python -ErrorAction SilentlyContinue) { "python" }
          else { "py" }

if ($Args.Count -gt 0 -and (Test-Path $Args[0])) {
    $Db = $Args[0]
    $toolArgs = $Args[1..$Args.Count]
} else {
    $toolArgs = $Args
}

& $python $Tool @toolArgs $Db
