# Manuelle Schritt-für-Schritt-Anleitung: Twisted Metal (2012) 4K Remaster

> Aktuelle englische Referenz: [BUILD.md](BUILD.md). Voraussetzungen und Grenzen: [Teststatus](docs/REPRODUCTION.md). Lokale Beispielpfade müssen ersetzt werden.

Diese Anleitung führt Entwickler, Modder und RPCS3-Enthusiasten Schritt für Schritt durch den manuellen Nachbau des 4K-Remasters von *Twisted Metal (PS3, 2012)* auf RPCS3 von Grund auf.

---

## 1. Übersicht der benötigten Eingaben & Komponenten

Für einen sauberen Nachbau ohne Abhängigkeit von bestehenden Installationen werden folgende Komponenten benötigt:

| Komponente | Quelle / Beschreibung | Zweck |
|---|---|---|
| **Dossier-Patch & Overlay** | `03_QUELLCODE/tracked-changes.patch` & `overlay/` | Vulkan-Renderpfad, 4K-Fusion, Detail-Shader |
| **Aktivierungs-Flags** | `03_QUELLCODE/flags/*.flag` (5 Dateien) | Schalten die 4K-Pipeline in `VKPresent.cpp` scharf |
| **Asset-Patcher** | `apply_times_ngp_delta.py` UND `times_ngp_deltas.json` | 5000m Objektsichtweite & Anti-Blur in Archiven |
| **Saubere PS3-Disc** | Twisted Metal BCUS98106 (`PS3_GAME/USRDIR/`) | Originale Spieldaten (PSARC, LTN) |
| **Spielupdate 01.06** | Offizielles Update v01.06 für BCUS98106 | Passend zum PPU-Patch `PPU-6bb4db39...` |
| **Sony PS3-Firmware** | Offizielle `PS3UPDAT.PUP` von playstation.com | In RPCS3 installiert (`dev_flash`) |
| **Visual Studio Build Tools** | VS 2026 (oder 2022 v17.10+) mit C++ Workload | MSVC Compiler (x64) & MSBuild |
| **CMake** | CMake >= 4.2 (z. B. 4.3.1) | Generierung des Visual Studio Buildbaums |
| **Qt 6 SDK** | Qt 6.11.2 (msvc2022_64) inkl. `windeployqt.exe` | GUI, Core, Multimedia, SVG |
| **Vulkan SDK** | LunarG Vulkan SDK 1.4.341.1 oder neuer | Vulkan Header, Validierung & Loader |
| **Statische LLVM-Libs** | Vorkompilierte statische RPCS3-LLVM-Bibliotheken (`/MT`) | PPU- und SPU-Recompiler |
| **Python 3** | Python 3.10+ (Standardbibliothek) | Ausführung des autarken Asset-Patchers |

---

## 2. Windows-Sicherheit & Pfadlängenschutz

### A. Windows Smart App Control & SmartScreen
- Unsere Modifikation verändert **nicht** den Windows-Grafikkartentreiber von AMD/NVIDIA (keine Kernel-Ebene betroffen), sondern ausschließlich den lokalen User-Space-Vulkan-Renderer von RPCS3.
- Da `rpcs3.exe` lokal kompiliert wird (unsigniert), kann Windows SmartScreen oder Smart App Control den Start blockieren.
- Keine automatischen Defender-Ausnahmen setzen und keine globalen Schutzfunktionen abschalten. ExecutionPolicy behebt keine Smart-App-Control-Blockade. Den konkreten Startfehler separat untersuchen.

### B. Pfadlängenbegrenzung (MAX_PATH = 260 Zeichen)
- Durch tief verschachtelte Git-Submodule (insbesondere innerhalb von LLVM-Testsuiten) kann der Windows-Standardpfad von 260 Zeichen überschritten werden.
- **Empfehlung:** Verwende ein kurzes Basisverzeichnis wie `C:\tm4k` und aktiviere Langpfad-Unterstützung in Git:
  ```powershell
  git -c core.longpaths=true submodule update --init --recursive --jobs 8
  ```

---

## 3. Quellcode vorbereiten & modifizieren

### Schritt 3.1: RPCS3 klonen & Basis-Commit auschecken
```powershell
mkdir C:\tm4k
cd C:\tm4k
git clone https://github.com/RPCS3/rpcs3.git rpcs3
cd rpcs3
git checkout 1707d7fc883ef48ff21bdcbb0141a3211ae09cb2
git -c core.longpaths=true submodule update --init --recursive --jobs 8
```

### Schritt 3.2: Patch anwenden & Overlay kopieren
1. Wende den gesicherten Patch aus dem Dossier an:
   ```powershell
   git apply "C:\path\to\mod-repository\03_QUELLCODE\tracked-changes.patch"
   ```
2. Kopiere die TM4K-spezifischen Quelldateien und Shader-Header in das Vulkan-Verzeichnis:
   ```powershell
   Copy-Item -Path "C:\path\to\mod-repository\03_QUELLCODE\overlay\rpcs3\Emu\RSX\VK\*" `
             -Destination "rpcs3\Emu\RSX\VK\" -Force
   ```
3. **Architektur-Highlights der TM4K-Pipeline:**
   - **`VKPresent.cpp` (Gehärtete, deckungsgleiche Präsentation):**
     - `tm4k_crop_inset = true` ist einheitlich aktiv, damit 3D-Spielszene und 2D-Overlay exakt dieselbe Streckung `(39*scale, 24*scale)` bis `(1241*scale, 696*scale)` auf volle 3840x2160 nutzen. Dies verhindert jeglichen Versatz und eliminiert Bildverdopplungen bei Schriften und Ladeanzeigen.
     - **Kein Adress-Gating:** Beide Framebuffer (`0xc0556000` für 3D-Geometrie und `0xc0010000` für 2D-Overlay/HUD) laufen durch `if (packet)`. Der Merge-Shader blendet die UI-Schicht nahtlos auf die 4K-HDR-Szene ein (`mix(fused, screen.rgb, overlay)`). Dadurch flackern weder das Feuer auf Sweet Tooths Kopf noch die Schriften.
     - **30-Frame 4K-Latch (~500 ms):** Fängt GPU-Lastspitzen, Explosionen und Framedrops zuverlässig ab (`s_fused_latch_frames < 30`). RPCS3 fällt dadurch niemals auf un-tonemappte FP16-Rohpuffer zurück – das weiße Aufblitzen im Match ist als Ziel reduziert; im Spiel zu prüfen.
     - `TM4K_COHORT_WINDOW = 4000`, `max_intra_gap = 2500` für stabile Kohortenbildung.
   - **`TM4KNativeColour.hpp`:** Farbrekonstruktions-Compute-Shader mit `d_clamp = 0.45` für ruhige, rauscharme Wüsten- und Felsenflächen.
   - **`TM4KParticles.hpp`:** Isoliert Partikelpässe (Sweet Tooth Flammen, Mündungsfeuer).

---

## 4. CMake-Konfiguration & Kompilierung mit MSBuild

### Schritt 4.1: Vollständiger CMake-Aufruf
Definiere die Pfade zu Quellen, Build, Qt und LLVM. Setze zwingend `-DLLVM_USE_INTEL_JITEVENTS=ON` (verhindert `LNK2001` beim Linken) und `-DCMAKE_DISABLE_FIND_PACKAGE_SDL3=ON` (verhindert OpenAL-SDL-Konflikte):

```powershell
$src   = "C:\tm4k\rpcs3"
$build = "C:\tm4k\build"
$Qt    = "C:\SDK\Qt\6.11.2\msvc2022_64"
$LLVM  = "C:\SDK\llvm"

cmake -S "$src" -B "$build" -G "Visual Studio 18 2026" -A x64 `
  -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded `
  -DUSE_STATIC_MSVC_RUNTIME=ON `
  -DABSL_MSVC_STATIC_RUNTIME=ON -Dprotobuf_MSVC_STATIC_RUNTIME=ON `
  -DBUILD_LLVM=OFF -DWITH_LLVM=ON -DSTATIC_LINK_LLVM=ON `
  -DLLVM_USE_INTEL_JITEVENTS=ON `
  "-DLLVM_DIR=$LLVM" "-DQt6_DIR=$Qt\lib\cmake\Qt6" `
  "-DCMAKE_PREFIX_PATH=$Qt" -DUSE_PRECOMPILED_HEADERS=ON `
  -DUSE_FAUDIO=OFF -DUSE_SYSTEM_CURL=OFF -DUSE_SYSTEM_ZLIB=OFF `
  -DUSE_SYSTEM_OPENAL=OFF -DUSE_SYSTEM_OPENCV=OFF `
  -DUSE_SYSTEM_SDL=OFF -DCMAKE_DISABLE_FIND_PACKAGE_SDL3=ON
```

### Schritt 4.2: Kompilierung mit MSBuild (Ressourcen-schonend)
Um Speicherüberläufe auf Systemen mit 32 GB RAM zu vermeiden, begrenze die parallele Ausführung (`/m:4` und `CL_MPcount=2`). Kompiliere strikt zuerst die Emulatorbibliothek und danach die EXE:

```powershell
# 1. Emulator-Kern kompilieren
msbuild "$build\rpcs3\Emu\rpcs3_emu.vcxproj" /p:Configuration=Release /p:Platform=x64 /m:4 /p:CL_MPcount=2

# 2. RPCS3 Hauptanwendung linken
msbuild "$build\rpcs3\rpcs3.vcxproj" /p:Configuration=Release /p:Platform=x64 /m:4 /p:CL_MPcount=2
```

### Schritt 4.3: Qt-Bereitstellung mit `windeployqt` (Pflichtschritt)
Erstelle das Zielverzeichnis `$bin` (z. B. `C:\tm4k\bin`), kopiere `rpcs3.exe` hinein und führe `windeployqt` aus:

```powershell
$bin = "C:\tm4k\bin"
mkdir $bin -Force
Copy-Item "$build\bin\rpcs3.exe" "$bin\"

# Alle benötigten Qt-DLLs und Plugins automatisiert bereitstellen
& "$Qt\bin\windeployqt.exe" --release --no-translations "$bin\rpcs3.exe"
```

### Schritt 4.4: TM4K-Aktivierungsflags kopieren
Kopiere die fünf Aktivierungsflags aus `03_QUELLCODE/flags/` direkt neben die `rpcs3.exe`:
```powershell
Copy-Item "C:\path\to\mod-repository\03_QUELLCODE\flags\*.flag" "$bin\" -Force
```
*Ohne diese Flags bleibt die 4K-Fusion inaktiv und das Bild fällt auf 720p zurück!*

---

## 5. Emulator-Konfiguration (`config_BCUS98106.yml`)

Erstelle in `$bin\config\custom_configs\config_BCUS98106.yml` die spielscharfe Konfiguration:

```yaml
Core:
  PPU Decoder: Recompiler (LLVM)
  PPU Debug: false
  Save LLVM logs: false
  SPU Decoder: Recompiler (LLVM)
  Preferred SPU Threads: 0
  SPU loop detection: false
  SPU Block Size: Safe
  SPU XFloat Accuracy: Approximate
  RSX FIFO Fetch Accuracy: "Ordered & Atomic"
  Stub PPU Traps: 1
Video:
  Renderer: Vulkan
  Resolution: 1280x720
  Aspect ratio: 16:9
  Frame limit: 60
  MSAA: Auto
  Shader Mode: Async Recompiler (multi-threaded)
  VSync Mode: Disabled
  Write Color Buffers: false
  Write Depth Buffer: false
  Multithreaded RSX: false
  Relaxed ZCULL Sync: false
  Resolution Scale: 300   # Nur internes 4K unterstuetzt; 1080p/1440p-Profile noch in Planung, derzeit Bildschicht-Versatz.
  Anisotropic Filter Override: 16
  Texture LOD Bias Addend: -0.1
  Minimum Scalable Dimension: 256
  Driver Wake-Up Delay: 0
  Output Scaling Mode: FidelityFX Super Resolution
  FidelityFX CAS Sharpening Intensity: 4
  Renderdoc Compatibility Mode: true
  Vulkan:
    Adapter: ""
    Asynchronous Texture Streaming: false
    Asynchronous Queue Scheduler: Safe
    Use Re-BAR for GPU uploads: true
(Hinweis: "Adapter: `"`"`" aktiviert die automatische GPU-Erkennung und vermeidet einen fest eingetragenen GPU-Namen; gewählte GPU im Log prüfen!)
Audio:
  Renderer: Cubeb
  Audio Format: Stereo
  Enable Buffering: true
  Desired Audio Buffer Duration: 60
  Enable Time Stretching: true
System:
  Language: English (US)
  Console Region: North America
Miscellaneous:
  Start games in fullscreen mode: true
  Prevent display sleep while running games: true
```
*(Wichtig: `Asynchronous Texture Streaming: false` schützt vor durchscheinenden Straßen/Texturen und Fragmenten! `Minimum Scalable Dimension: 256` verhindert Fehlskalierung von 2D-Texturen.)*

### Dialoge & Controller-Setup:
1. **Willkommensdialog ausschalten:** In `$bin\GuiConfigs\CurrentSettings.ini` unter `[main_window]` den Eintrag `infoBoxEnabledWelcome=false` setzen.
2. **Controller-Einrichtung (Individuell & Schützend):**
   - *Bestehende Installationen:* Falls du bereits RPCS3 auf deinem PC nutzt, rühre den Ordner `config\input_configs\` nicht an – deine vorhandenen Profile (Xbox, PS5, Lenkrad) bleiben unverändert aktiv.
   - *Erstinstallation:* Öffne RPCS3 und klicke oben auf **Pads**. Wähle für Spieler 1 dein Eingabegerät aus:
     - Xbox-Controller & gängige PC-Pads: Handler `XInput`, Device `XInput Pad #1`.
     - PlayStation 4 / 5 Controller: Handler `DualShock 4` oder `DualSense` (unterstützt sogar Motion-Controls).
     - Sonstige Controller: Handler `SDL` oder `DirectInput`.
     *(Wichtig: Niemals ungefragt statische Konfigurationsdateien mit festen fremden Vendor-/Product-IDs ins System kopieren – dies kann unter Windows zu Bluetooth-Entkoppelungen führen!)*
3. **Erststart-Verhalten & Shader-Cache (7.000 vs. 9.000 Dateien):**
   - Beim allerersten Start kompiliert RPCS3 im Hintergrund die PPU-LLVM-Module (ca. 7.000 Basismodule) und legt den Savegame-Container an (kann ca. 10 Sekunden ein weißes/helles Bild zeigen, bevor das Menü lädt).
   - Auf einem frischen Ordner ist der Vulkan-Shader-Cache noch leer. In den ersten 3–5 Minuten Fahrzeit kompiliert RPCS3 im Hintergrund über 1.500 Vulkan-Pipelines, bis der Cache ca. 8.500 bis 9.000 Dateien umfasst.
   - Im bewährten Standardmodus `Shader Mode: Async Recompiler (multi-threaded)` werden noch unkompilierte Shader für 1–2 Frames übersprungen (kann beim allerersten Match zu kurzen Effektauslassungen führen). Nach diesem einmaligen Einspielen liegen alle Shader dauerhaft im Cache und das Spiel läuft ab Start 2 absolut flüssig und stabil!

---

## 6. PPU- und SPU-Patches (`patches/` & `patch_config.yml`)

1. **Patch-Definitionsdateien kopieren (PFLICHT):**  
   Kopiere `03_QUELLCODE\configs\patches` nach `$bin\patches\`.  
   *(Enthält `BCUS98106_patch.yml` und `patch.yml`. Ohne diese Dateien kann RPCS3 die Patches nicht finden: Sweet Tooths Flammen werden cyan/blau und am unteren Bildrand entsteht ein giftgrüner Balken!)*

2. **Patches aktivieren in `$bin\config\patch_config.yml`:**  
   In `$bin\config\patch_config.yml` für PPU-6bb4db395550102f567fb7b1dbbf81583417150f und SPU-82b3399c8e6533ba991eedb0e139bf20c7783bac aktivieren:

```yaml
PPU-6bb4db395550102f567fb7b1dbbf81583417150f:
  "60 FPS":
    Twisted Metal:
      BCUS98106:
        01.06:
          Enabled: true
  "PostFX Sharp Fast":
    Twisted Metal:
      BCUS98106:
        01.06:
          Enabled: true
  "Raw 4K Scene":
    Twisted Metal:
      BCUS98106:
        01.06:
          Enabled: false
  "Extend Draw Distance":
    Twisted Metal:
      BCUS98106:
        01.06:
          Enabled: true
  "Model LOD Distance +50%":
    Twisted Metal:
      BCUS98106:
        01.06:
          Enabled: false
  "Model LOD Distance +100% (2x)":
    Twisted Metal:
      BCUS98106:
        01.06:
          Enabled: false
  "Model LOD Distance x4":
    Twisted Metal:
      BCUS98106:
        01.06:
          Enabled: false
  "Force Near Model LOD - distance zero test":
    Twisted Metal:
      BCUS98106:
        01.06:
          Enabled: false

SPU-82b3399c8e6533ba991eedb0e139bf20c7783bac:
  "Disable SPU MLAA - Twisted Metal, Resident Evil: Operation Raccoon City":
    All:
      All:
        All:
          Enabled: true
```

> [!CRITICAL]
> `Model LOD Distance` MUSS auf `false` stehen. Das Überschreiben des HierLOD-Faktors an `0x0010e83c` desynchronisiert das asynchrone Mesh-Streaming und führt dazu, dass der Boden schwarz wird!

---

## 7. Asset-Patching (5000m Weitsicht & Anti-Blur)

Kopiere `apply_times_ngp_delta.py` und `times_ngp_deltas.json` aus `03_QUELLCODE/` in ein Arbeitsverzeichnis und führe aus:

```powershell
python apply_times_ngp_delta.py `
  --disc-dir "<Pfad-zu-deiner-Disc>\PS3_GAME\USRDIR" `
  --target-dir "$bin\dev_hdd0\game\BCUS98106\USRDIR"
```

Das Skript erledigt in ~2,5 Sekunden bit-exakt:
- SP: `levels/times.psarc` (gepatchter SHA256: `9129889C700E23CB61569EA90722E0E8F0A64B00CB2D4F4750BAA51DD407A22C`)
- MP: `mplevels/times.psarc` (gepatchter SHA256: `757C01A688C74918AB9A1CBBBE59BF1D8787D0C5BA44391059E798E96159E90A`)
- `levels/times.ltn` & `levels/suburbs.ltn`: `Distance Blur Enable: 0` (verhindert trüben Entfernungsnebel).

---

## 8. Launcher-Skript & Ausführung

Erstelle `Start-TwistedMetal-4K.ps1` im Binärverzeichnis:

```powershell
param([switch]$CheckOnly)

$binDir = $PSScriptRoot
$exe    = Join-Path $binDir "rpcs3.exe"
$iso    = "<Pfad-zu-deiner-Twisted-Metal.iso>"

# TM4K Laufzeit-Pipeline konfigurieren
$env:TM4K_SCENE_2160       = '1'
$env:TM4K_GEOMETRY_LAG1    = '0'
$env:TM4K_COHORT_WINDOW    = '4000'
$env:TM4K_INTRA_GAP        = '2500'
$env:TM4K_MASTER_DENSITY   = '7'
$env:TM4K_PARTICLE_RESTORE = '1'
$env:TM4K_DETAIL_PALETTE   = '1'
$env:RPCS3_CONFIG_DIR      = "$binDir\"

if ($CheckOnly) {
    $checker = Join-Path $binDir "Verify-TM4K-Setup.ps1"
    if (Test-Path $checker) {
        & $checker -TargetDir $binDir
    } else {
        Write-Host "PASS: Konfiguration und Umgebung sind startklar!" -ForegroundColor Green
    }
    return
}

# Spiel starten (ohne --no-gui, damit Windows-Controller beim Beenden sauber freigegeben werden)
Start-Process -FilePath $exe -ArgumentList "`"$iso`"" -WorkingDirectory $binDir
```

---

## 9. Automatisierte Selbstprüfung & Testfahrt

### 9.1 Automatisierter 8-Punkte-Systemcheck (Pflicht vor Erststart)
Führe das mitgelieferte Validierungsskript aus, um alle Dateien und Einstellungen auf Korrektheit zu scannen:
```powershell
powershell -ExecutionPolicy Bypass -File "$bin\Verify-TM4K-Setup.ps1"
```
Das Skript prüft Hardware, Binärgrößen, alle 5 `.flag`-Dateien, die Patch-Definition an `0x0066aaf4`, die HierLOD-Sperre, 300% 4K-Skalierung und das XInput-Controller-Profil. Erst wenn `8 / 8 PRÜFUNGEN BESTANDEN` gemeldet wird, ist das System startklar.

### 9.2 Testfahrt & Verifikation
1. **Hauptmenü:** Schriften glasklar, Clownmaske und Feuer scharf (kein Türkis!), keine schwarzen Trauerränder, keine doppelten Schriften.
2. **Times Square & Ice Map:** Straßentexturen und Schnee bleiben bei schnellen Kurvenfahrten und Boosts zu 100% stabil (kein schwarzer Boden).
3. **Sun Valley (Sandlevel):** Wüstensand und Felsen grieseln nicht; Geometriekanten bleiben ruhig.
4. **Kollisionen:** Bei Vollgas-Rammen von Wänden und Gegnern bricht das Bild nicht ab und bleibt in nativer 4K-Präsentation.
5. **Controller:** Xbox- oder PC-Gamepad steuert sofort ohne manuelle Zuweisung.
