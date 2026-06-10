Remove-Item -Recurse -Force st\build -ErrorAction SilentlyContinue
Remove-Item -Force st\bin\st.exe -ErrorAction SilentlyContinue

$f = [DateTime]::Now.Ticks.ToString() + ".log"
$cmd = "make -DDEMO_DOS -DRUN -B > C:\" + $f
Write-Host "Log: $f"
Write-Host "Cmd: $cmd"
& "C:\DOSBox-X\dosbox-x.exe" -conf dosbox-x.conf -fastlaunch -c $cmd -c "exit" 2>&1
Start-Sleep 30
if (Test-Path $f) {
    Write-Host ("OK: " + (Get-Item $f).Length + " bytes")
    Get-Content -Path $f -TotalCount 5
} else {
    Write-Host "NO LOG"
}
