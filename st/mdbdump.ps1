<#
.SYNOPSIS
    Dump MiniDB .db contents from the st/ directory.
.PARAMETER Options
    Passed through to mdbdump.py (e.g. -b for brief mode).
.DESCRIPTION
    Defaults to st/bin/RX.db. For a different database, run bin/mdbdump.py directly.
#>

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Db = Join-Path $ScriptDir "bin" "RX.db"
$Tool = Join-Path $ScriptDir "util" "lsmdb" "mdbdump.py"

if (-not (Test-Path $Tool)) {
    $Tool = Join-Path $ScriptDir "bin" "mdbdump.py"
}

if (-not (Test-Path $Tool)) {
    Write-Error "mdbdump.py not found (looked in util/lsmdb/ and bin/)"
    exit 1
}
$python = if (Get-Command python3 -ErrorAction SilentlyContinue) { "python3" }
          elseif (Get-Command python -ErrorAction SilentlyContinue) { "python" }
          else { "py" }

if ($Args.Count -gt 0 -and (Test-Path $Args[0])) {
    # First arg is a file path — use it as DB, pass rest as options
    $Db = $Args[0]
    $toolArgs = $Args[1..$Args.Count]
} else {
    $toolArgs = $Args
}

& $python $Tool @toolArgs $Db

