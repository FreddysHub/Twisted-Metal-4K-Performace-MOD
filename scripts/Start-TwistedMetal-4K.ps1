[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$BinDir,
    [Parameter(Mandatory=$true)][string]$GamePath,
    [switch]$CheckOnly
)
$ErrorActionPreference = 'Stop'
$tmBin = (Resolve-Path -LiteralPath $BinDir).Path
$tmGame = (Resolve-Path -LiteralPath $GamePath).Path
$tmExe = Join-Path $tmBin 'rpcs3.exe'
if (-not (Test-Path -LiteralPath $tmExe -PathType Leaf)) { throw 'rpcs3.exe is missing.' }
foreach ($tmFlag in @('tm4k-enable.flag','tm4k-lit-c-enable.flag','tm4k-lit01-enable.flag','tm4k-micro-e-enable.flag','tm4k-packed-alpha-enable.flag')) {
    if (-not (Test-Path -LiteralPath (Join-Path $tmBin $tmFlag))) { throw "Missing flag: $tmFlag" }
}
if ($tmGame.Contains('"')) { throw 'Unsupported quotation mark in game path.' }
if ($CheckOnly) {
    Write-Output 'Paths and flags checked. No process started; no gameplay validation.'
    exit 0
}
if (Get-Process -Name rpcs3 -ErrorAction SilentlyContinue) { throw 'Close other RPCS3 instances first.' }
$tmEnvironment = @{
    TM4K_SCENE_2160='1'; TM4K_GEOMETRY_LAG1='0'; TM4K_COHORT_WINDOW='4000'
    TM4K_INTRA_GAP='2500'; TM4K_MASTER_DENSITY='7'; TM4K_PARTICLE_RESTORE='1'
    TM4K_DETAIL_PALETTE='1'; TM4K_DRAW_GUARD=$null
}
$tmOld = @{}
try {
    foreach ($tmName in $tmEnvironment.Keys) {
        $tmOld[$tmName] = [Environment]::GetEnvironmentVariable($tmName,'Process')
        [Environment]::SetEnvironmentVariable($tmName,$tmEnvironment[$tmName],'Process')
    }
    Start-Process -FilePath $tmExe -ArgumentList ('"' + $tmGame + '"') -WorkingDirectory $tmBin
} finally {
    foreach ($tmName in $tmOld.Keys) {
        [Environment]::SetEnvironmentVariable($tmName,$tmOld[$tmName],'Process')
    }
}
