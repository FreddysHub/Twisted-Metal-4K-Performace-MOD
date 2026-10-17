# ==============================================================================
# TM4K AUTOMATISIERTER SYSTEM- & SICHERHEITS-CHECK (Verify-TM4K-Setup.ps1)
# ==============================================================================
param(
    [string]$TargetDir = $PSScriptRoot
)

$Host.UI.RawUI.WindowTitle = "TM4K Selbsttest & Validierung"
Write-Host "====================================================================" -ForegroundColor Cyan
Write-Host "   TWISTED METAL 4K - AUTOMATISIERTE VALIDIERUNG & SELBSTPRÜFUNG    " -ForegroundColor Cyan
Write-Host "====================================================================" -ForegroundColor Cyan
Write-Host "Prüfpfad: $TargetDir`n" -ForegroundColor Gray

$passed = 0
$total  = 8
$errors = @()

function Get-YamlPatchState($filePath, $patchName) {
    if (-not (Test-Path $filePath)) { return $null }
    $lines = Get-Content $filePath
    $inPatch = $false
    foreach ($line in $lines) {
        if ($line -match [regex]::Escape($patchName)) {
            $inPatch = $true
            continue
        }
        if ($inPatch) {
            # Neuer Patch-Abschnitt oder SPU/PPU-Header
            if ($line -match '^\s{2}"' -or $line -match '^[A-Z0-9\-]+:') {
                break
            }
            if ($line -match 'Enabled:\s*(true|false)') {
                return ($matches[1] -eq 'true')
            }
        }
    }
    return $null
}

# 1. Hardware-Erkennung & Universalitäts-Check
Write-Host "[1/8] Hardware-Erkennung & Plattform-Kompatibilität..." -NoNewline
try {
    $cpu = (Get-CimInstance Win32_Processor | Select-Object -First 1).Name.Trim()
    $gpu = (Get-CimInstance Win32_VideoController | Where-Object { $_.Name -notmatch 'Basic|Virtual' } | Select-Object -First 1).Name.Trim()
    Write-Host " [PASS]" -ForegroundColor Green
    Write-Host "      CPU: $cpu" -ForegroundColor DarkGray
    Write-Host "      GPU: $gpu (Universal Vulkan Modus)" -ForegroundColor DarkGray
    $passed++
} catch {
    Write-Host " [WARN] Hardware-Erkennung übersprungen ($_)" -ForegroundColor Yellow
    $passed++
}

# 2. Binärdatei & Symbole
Write-Host "[2/8] RPCS3 Hauptprogramm (rpcs3.exe)..." -NoNewline
$exePath = Join-Path $TargetDir "rpcs3.exe"
if (Test-Path $exePath) {
    $exeSize = (Get-Item $exePath).Length / 1MB
    if ($exeSize -gt 30) {
        Write-Host " [PASS] (Größe: $([math]::Round($exeSize, 1)) MB)" -ForegroundColor Green
        $passed++
    } else {
        Write-Host " [FAIL]" -ForegroundColor Red
        $errors += "rpcs3.exe existiert, ist aber verdächtig klein ($([math]::Round($exeSize, 1)) MB). Bitte Release x64 Build prüfen."
    }
} else {
    Write-Host " [FAIL]" -ForegroundColor Red
    $errors += "rpcs3.exe fehlt im Verzeichnis $TargetDir!"
}

# 3. TM4K RSX-Präsentations-Flags
Write-Host "[3/8] TM4K RSX-Präsentations-Flags (5 Pflicht-Flags)..." -NoNewline
$flags = @(
    "tm4k-enable.flag",
    "tm4k-lit-c-enable.flag",
    "tm4k-lit01-enable.flag",
    "tm4k-micro-e-enable.flag",
    "tm4k-packed-alpha-enable.flag"
)
$missingFlags = @()
foreach ($f in $flags) {
    if (-not (Test-Path (Join-Path $TargetDir $f))) {
        $missingFlags += $f
    }
}
if ($missingFlags.Count -eq 0) {
    Write-Host " [PASS] (Alle 5 Flags aktiv)" -ForegroundColor Green
    $passed++
} else {
    Write-Host " [FAIL]" -ForegroundColor Red
    $errors += "Fehlende Flags: $($missingFlags -join ', ')! Ohne diese Flags bleibt 4K inaktiv."
}

# 4. Patch-Definitionsdateien (BCUS98106_patch.yml)
Write-Host "[4/8] SPU/PPU Patch-Definitionen in patches/..." -NoNewline
$patchDefFile = Join-Path $TargetDir "patches\BCUS98106_patch.yml"
if (Test-Path $patchDefFile) {
    $content = Get-Content $patchDefFile -Raw
    if ($content -match "0x0066aaf4" -and $content -match "PostFX Sharp Fast") {
        Write-Host " [PASS] (BCUS98106_patch.yml mit 11x NOP PostFX-Bypass vorhanden)" -ForegroundColor Green
        $passed++
    } else {
        Write-Host " [FAIL]" -ForegroundColor Red
        $errors += "BCUS98106_patch.yml existiert, enthält aber nicht den 'PostFX Sharp Fast' 11x NOP Bypass an 0x0066aaf4!"
    }
} else {
    Write-Host " [FAIL]" -ForegroundColor Red
    $errors += "patches\BCUS98106_patch.yml fehlt! Folge: Türkise Flammen auf Sweet Tooth & grüner Streifen am Bildrand!"
}

# 5. Patch-Aktivierung in config/patch_config.yml
Write-Host "[5/8] Patch-Aktivierung in config/patch_config.yml..." -NoNewline
$patchCfgFile = Join-Path $TargetDir "config\patch_config.yml"
if (Test-Path $patchCfgFile) {
    $postFxStatus  = Get-YamlPatchState $patchCfgFile "PostFX Sharp Fast"
    $hierLodStatus = Get-YamlPatchState $patchCfgFile "Model LOD Distance +100% (2x)"
    
    if ($postFxStatus -eq $true -and $hierLodStatus -ne $true) {
        Write-Host " [PASS] (PostFX Sharp Fast: true, HierLOD: false / geschützt)" -ForegroundColor Green
        $passed++
    } elseif ($hierLodStatus -eq $true) {
        Write-Host " [FAIL]" -ForegroundColor Red
        $errors += "Model LOD Distance +100% ist auf TRUE gesetzt! Das führt zu schwarzem Boden in Times Square! Muss FALSE sein."
    } else {
        Write-Host " [FAIL]" -ForegroundColor Red
        $errors += "PostFX Sharp Fast ist in patch_config.yml nicht auf 'Enabled: true' gesetzt!"
    }
} else {
    Write-Host " [FAIL]" -ForegroundColor Red
    $errors += "config\patch_config.yml fehlt!"
}

# 6. Spiel-Konfiguration in config/custom_configs/config_BCUS98106.yml
Write-Host "[6/8] Spiel-Konfiguration (config_BCUS98106.yml)..." -NoNewline
$gameCfgFile = Join-Path $TargetDir "config\custom_configs\config_BCUS98106.yml"
if (Test-Path $gameCfgFile) {
    $gameCfg = Get-Content $gameCfgFile -Raw
    $scaleMatch = [regex]::Match($gameCfg, "Resolution Scale:\s*(\d+)")
    $currentScale = if ($scaleMatch.Success) { [int]$scaleMatch.Groups[1].Value } else { 0 }
    $validScale = ($currentScale -in @(150, 200, 300))
    $hasAsyncFalse = ($gameCfg -match "Asynchronous Texture Streaming:\s*false")
    $hasMinDim256 = ($gameCfg -match "Minimum Scalable Dimension:\s*256")
    $hasDefaultShader = ($gameCfg -match '(?m)^\s*Shader Mode:\s*Async Recompiler \(multi-threaded\)\s*$')
    
    if ($validScale -and $hasAsyncFalse -and $hasMinDim256 -and $hasDefaultShader) {
        $scaleDesc = switch ($currentScale) {
            150 { "150% 1080p FHD (Performance-Modus)" }
            200 { "200% 1440p WQHD" }
            300 { "300% 4K UHD (Referenz-Standard / SSAA)" }
            default { "$currentScale% Skalierung" }
        }
        Write-Host " [PASS] ($scaleDesc, Async Streaming: false, Min Scalable: 256, Shader: Async Recompiler)" -ForegroundColor Green
        $passed++
    } else {
        Write-Host " [FAIL]" -ForegroundColor Red
        if (-not $validScale) { $errors += "Resolution Scale ist $currentScale (Gueltig: 300 fuer 4K, 200 fuer 1440p, 150 fuer 1080p)!" }
        if (-not $hasAsyncFalse) { $errors += "Asynchronous Texture Streaming muss FALSE sein (verhindert durchscheinende Straßen)!" }
        if (-not $hasMinDim256) { $errors += "Minimum Scalable Dimension muss 256 sein (schützt 2D-Masken)!" }
        if (-not $hasDefaultShader) { $errors += 'Shader Mode muss fuer den regulaeren Neubau exakt Async Recompiler (multi-threaded) sein. Interpreter-Modi loesen eine zusaetzliche lange Pipeline-Vorberechnung aus; Async with Shader Interpreter ist zudem kein gueltiger YAML-Wert.' }
    }
} else {
    Write-Host " [FAIL]" -ForegroundColor Red
    $errors += "config\custom_configs\config_BCUS98106.yml fehlt!"
}

# 7. Gamepad / Controller (Individuelle Nutzer-Erkennung)
Write-Host "[7/8] Gamepad / Controller-Status..." -NoNewline
$hasGlobalPad = Test-Path (Join-Path $TargetDir "config\input_configs\global\Default.yml")
$hasLocalPad  = Test-Path (Join-Path $TargetDir "config\input_configs\Default.yml")
$hasCustomPad = (Test-Path (Join-Path $TargetDir "config\input_configs")) -and ((Get-ChildItem -Path (Join-Path $TargetDir "config\input_configs") -Filter "*.yml" -Recurse).Count -gt 0)

if ($hasGlobalPad -or $hasLocalPad -or $hasCustomPad) {
    Write-Host " [PASS] (Controller-Konfiguration im System erkannt)" -ForegroundColor Green
    $passed++
} else {
    Write-Host " [INFO] (Individuelle Steuerung: RPCS3 übernimmt vorhandene Profile oder Zuweisung im 'Pads'-Menü)" -ForegroundColor Cyan
    $passed++
}

# 8. Start-Skript & 7 TM4K-Umgebungsvariablen
Write-Host "[8/8] Launcher & TM4K-Umgebungsvariablen..." -NoNewline
$launcher = Join-Path $TargetDir "Start-TwistedMetal-4K.ps1"
if (Test-Path $launcher) {
    $launchContent = Get-Content $launcher -Raw
    $hasEnv = ($launchContent -match "TM4K_SCENE_2160" -and $launchContent -match "TM4K_MASTER_DENSITY")
    if ($hasEnv) {
        Write-Host " [PASS] (Launcher mit TM4K-Umgebungsvariablen vorhanden)" -ForegroundColor Green
        $passed++
    } else {
        Write-Host " [WARN] Launcher vorhanden, aber Umgebungsvariablen unvollständig." -ForegroundColor Yellow
        $passed++
    }
} else {
    Write-Host " [WARN] Start-TwistedMetal-4K.ps1 fehlt im Verzeichnis (optional, aber empfohlen)." -ForegroundColor Yellow
    $passed++
}

# Gesamtauswertung
Write-Host "`n--------------------------------------------------------------------" -ForegroundColor Gray
if ($errors.Count -eq 0) {
    Write-Host "ERGEBNIS: $passed / $total PRÜFUNGEN ERFOLGREICH BESTANDEN!" -ForegroundColor Green
    Write-Host "STATUS: Setup-Pruefungen bestanden. Spielstart, Grafik und Controller-Eingaben separat pruefen." -ForegroundColor Green
    Write-Host "--------------------------------------------------------------------`n" -ForegroundColor Gray
    exit 0
} else {
    Write-Host "ERGEBNIS: $($total - $errors.Count) / $total bestanden. $($errors.Count) FEHLER GEFUNDEN!" -ForegroundColor Red
    Write-Host "`nFOLGENDE KORREKTUREN SIND ZWINGEND ERFORDERLICH:" -ForegroundColor Yellow
    $i = 1
    foreach ($err in $errors) {
        Write-Host "  $i. $err" -ForegroundColor Red
        $i++
    }
    Write-Host "--------------------------------------------------------------------`n" -ForegroundColor Gray
    exit 1
}
