# Strenger KI-Testlauf-Prompt (Isolierter "Neuer PC"-Modus)

> Prüfauftrag, kein Nachweis eines schon bestandenen Neu-PC-Tests. Aktuelle Anleitung: [BUILD.md](BUILD.md); eigene Controllerzuweisung statt mitgelieferter persönlicher Profile.

> **Verwendungszweck:**  
> Kopiere den gesamten Inhalt dieses Dokuments (ab dem Trennbalken) und füge ihn in deinen **neuen Chat** ein.  
> Dieser Prompt erzwingt eine strikte „Air-Gap“-Sperre gegen alle vorhandenen Fertig-Installationen auf deinem Rechner. Die KI muss sich so verhalten, als säße sie an einem **völlig neuen, frisch aufgesetzten PC**, der ausschließlich die für GitHub vorgesehenen Dateien zur Verfügung hat.

---

```markdown
# MISSION: AUTARKER 4K-REMASTER-NEUBAU FÜR TWISTED METAL (PS3)
# MODUS: STRIKTE ISOLATION ("FRISCHER PC" / GITHUB-ONLY)

Du bist ein leitender Systems-Engineer und Low-Level-Grafikprogrammierer.
Deine Aufgabe ist es, den vollständigen 4K-Remaster-Build für "Twisted Metal (PS3, 2012 / BCUS98106 v01.06)" auf RPCS3 von Grund auf sauber neu zu erstellen.

================================================================================
STRENGSTE SPERR-REGELN: AIR-GAP ZU ALLEN LOKALEN BESTANDSBILDS!
================================================================================
Auf diesem System existieren möglicherweise frühere Entwicklungsordner, Backups oder alte Test-Builds. 
FÜR DICH GILT EINE ABSOLUTE SPERRE:
1. VERBOT VON ALTBESTÄNDEN:
   Es ist dir STRENGSTENS UNTERSAGT, auf bestehende RPCS3-Builds, vorkompilierte Binaries oder frühere Ausgabeordner zuzugreifen (z. B. "bin-twisted-metal-4k-stabilitats-test", "bin-twisted-metal-4k-sichtweite-test", "C:\tm4k\bin", "rpcs3-live-4k", "outputs\TwistedMetal4K" etc.).
2. KEIN KOPIEREN VON FERTIGDATEIEN:
   Du darfst KEINE "rpcs3.exe", KEINE DLLs, KEINE Shader-Caches und KEINE Konfigurationsdateien aus fertigen Ordnern herüberkopieren!
3. VOLLSTÄNDIGER NEUER-PC-MODUS:
   Du musst so agieren, als wäre dies ein FRISCHER, NEUER PC eines fremden Spielers, der das Projekt gerade von GitHub heruntergeladen hat.
4. ERLAUBTE DATENQUELLEN (NUR DIESE GITHUB-DATEIEN DÜRFEN GENUTZT WERDEN):
   Du darfst AUSSCHLIESSLICH folgende Quellen verwenden:
   a) Die Quellcode- und Patch-Dateien aus dem Dossier ("03_QUELLCODE"):
      - `tracked-changes.patch`
      - `overlay/rpcs3/Emu/RSX/VK/*` (alle Header und Shader-Dateien)
      - `flags/*.flag` (die 5 Aktivierungsflags)
      - `apply_times_ngp_delta.py` UND `times_ngp_deltas.json`
      - `configs/*` (Game-/Patch-Konfiguration; eigenes Gamepad über Pads zuweisen)
   b) Das saubere, unmodifizierte PS3-Disc-Verzeichnis (`PS3_GAME/USRDIR/`) bzw. ISO.
   c) Das offizielle Sony-Update BCUS98106 v01.06 und die offizielle Firmware `dev_flash`.
   d) Offizielles RPCS3-Git-Repository und Standard-Build-Werkzeuge (MSVC, CMake, Qt, Vulkan SDK, LLVM, Python 3).

Alles andere musst du EIGENSTÄNDIG und AUTONOM von Null auf erzeugen, kompilieren und einrichten!

================================================================================
SCHRITT 1: ARBEITSVERZEICHNIS & ISOLIERTER RPCS3-CHECKOUT
================================================================================
1. Wähle ein frisches, kurzes Arbeitsverzeichnis, um Windows-Pfadlängenprobleme (MAX_PATH = 260 Zeichen) zu vermeiden:
   Basisordner: `C:\tm4k_fresh` (oder `C:\tm4k_clean`)
   (Sicherheitsregel: Falls `C:\tm4k_fresh` von einem früheren Test existiert, lösche es vorab restlos mit `cmd /c rmdir /s /q C:\tm4k_fresh`, damit das Klonen in einen komplett leeren Ordner erfolgt!)
2. Klone das offizielle RPCS3-Repository und setze exakt den getesteten Basis-Commit:
   git clone https://github.com/RPCS3/rpcs3.git C:\tm4k_fresh\rpcs3
   cd C:\tm4k_fresh\rpcs3
   git checkout 1707d7fc883ef48ff21bdcbb0141a3211ae09cb2
3. Initialisiere rekursiv alle Submodule mit Langpfad-Support:
   git -c core.longpaths=true submodule update --init --recursive --jobs 8
4. Validiere, dass alle Submodule vollständig ausgecheckt sind.

================================================================================
SCHRITT 2: QUELLCODE-PATCHING & TM4K-OVERLAY (03_QUELLCODE)
================================================================================
1. Wende den GitHub-Patch an:
   git apply "C:\path\to\mod-repository\03_QUELLCODE\tracked-changes.patch"
2. Kopiere die TM4K-spezifischen Quelldateien und Shader-Header aus dem Dossier in den Vulkan-Quellbaum:
   Copy-Item "C:\path\to\mod-repository\03_QUELLCODE\overlay\rpcs3\Emu\RSX\VK\*" `
             "C:\tm4k_fresh\rpcs3\rpcs3\Emu\RSX\VK\" -Force
3. Sicherheitsprüfung der Präsentationslogik in `VKPresent.cpp`:
   - `tm4k_crop_inset = true` muss global aktiv sein (schützt vor 39x24-Versatz und doppelten Schriften).
   - NIEMALS ein Adress-Gating auf `0xc0556000` einbauen! 
     Twisted Metal wechselt mit jedem Frame zwischen 3D-Szene (0xc0556000) und 2D-Overlay (0xc0010000).
     Beide Puffer müssen durch `if (packet)` laufen, damit der Merge-Shader die 2D-Schrift deckungsgleich
     über die 4K-3D-Szene legt. Ein Aussperren führt zu einem 30-Hz-Stroboskop-Flackern von Flammen und Schrift!
   - 30-Frame-Latch (~500 ms):
     `else if (s_last_fused_image && s_fused_latch_frames < 30)`
     Muss aktiv sein, um Frame-Drops bei schweren Kollisionen abzufangen und den weißen 1-Sekunden-Blitz zu verhindern.
   - Schärfekalibrierung in `TM4KNativeColour.hpp`:
     `clamp(detail, -0.08, 0.08) * 0.45 * geo_mask` (`d_clamp = 0.45`).

================================================================================
SCHRITT 3: CMAKE-KONFIGURATION & COMPILER-LAUF
================================================================================
1. Vollständiger CMake-Aufruf (keine Auslassungen!):
   Pfade:
   $src   = "C:\tm4k_fresh\rpcs3"
   $build = "C:\tm4k_fresh\build"
   $Qt    = "C:\tm4k\workspace\rpcs3-build-tools\Qt\6.11.2\msvc2022_64"
   $LLVM  = "C:\tm4k\workspace\rpcs3-build-tools\llvm"

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

2. Kompilierung mit MSBuild (Ressourcenschonend gegen RAM-Spitzen):
   a. Emulator-Kern:
      & "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\MSBuild\Current\Bin\amd64\MSBuild.exe" `
        "$build\rpcs3\Emu\rpcs3_emu.vcxproj" /p:Configuration=Release /p:Platform=x64 /m:4 /p:CL_MPcount=2
   b. Hauptanwendung linken:
      & "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\MSBuild\Current\Bin\amd64\MSBuild.exe" `
        "$build\rpcs3\rpcs3.vcxproj" /p:Configuration=Release /p:Platform=x64 /m:4 /p:CL_MPcount=2

================================================================================
SCHRITT 4: ZIELVERZEICHNIS EINRICHTEN (DEPLOYMENT OHNE ALTBESTÄNDE)
================================================================================
1. Erstelle das saubere Ausführungsverzeichnis:
   $bin = "C:\tm4k_fresh\bin"
   mkdir $bin -Force
   Copy-Item "$build\bin\rpcs3.exe" "$bin\"

2. Automatisiertes Qt-Deployment mit `windeployqt` (Pflicht für frische PCs):
   & "$Qt\bin\windeployqt.exe" --release --no-translations "$bin\rpcs3.exe"

3. Kopiere die 5 Aktivierungs-Flags aus dem GitHub-Ordner `03_QUELLCODE/flags/`:
   Copy-Item "C:\path\to\mod-repository\03_QUELLCODE\flags\*.flag" "$bin\" -Force
   (Enthält: tm4k-enable.flag, tm4k-lit-c-enable.flag, tm4k-lit01-enable.flag,
             tm4k-micro-e-enable.flag, tm4k-packed-alpha-enable.flag)
   Hinweis: Ohne diese Flags greift der 4K-Shader nicht und das Bild fällt auf 720p zurück!

================================================================================
SCHRITT 5: SPIELSPEZIFISCHE KONFIGURATIONEN
================================================================================
Empfohlener & sicherster Weg: Kopiere die vorbereiteten Konfigurationen direkt aus dem GitHub-Ordner `03_QUELLCODE\configs`:
Copy-Item "C:\path\to\mod-repository\03_QUELLCODE\configs\custom_configs" "$bin\config\custom_configs" -Recurse -Force
Copy-Item "C:\path\to\mod-repository\03_QUELLCODE\configs\input_configs" "$bin\config\input_configs" -Recurse -Force
Copy-Item "C:\path\to\mod-repository\03_QUELLCODE\configs\patch_config.yml" "$bin\config\patch_config.yml" -Force
Copy-Item "C:\path\to\mod-repository\03_QUELLCODE\configs\GuiConfigs" "$bin\GuiConfigs" -Recurse -Force
Copy-Item "C:\path\to\mod-repository\03_QUELLCODE\configs\patches" "$bin\patches" -Recurse -Force
(KRITISCH: "$bin\patches\BCUS98106_patch.yml" enthält die Definition für "PostFX Sharp Fast"! Fehlt dieser Ordner, wird der PostFX-Bypass nicht angewendet: Sweet Tooths Flammen werden cyan/blau und am unteren Bildrand erscheint ein giftgrüner Streifen!)

(Alternativ, falls du die Dateien von Hand erstellen möchtest, verwende exakt die nachfolgenden Spezifikationen):

1. "$bin\config\custom_configs\config_BCUS98106.yml":
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
  Resolution Scale: 300   # 300 = 4K UHD (Standard & Referenz, empfohlen auch für 1080p/1440p via SSAA Downsampling); 200 = 1440p; 150 = 1080p (Performance)
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
(Hinweis: "Adapter: `"`"`" aktiviert die automatische Erkennung der primären dedizierten GPU – funktioniert universal auf NVIDIA GeForce RTX, AMD Radeon RX und Intel Arc!)
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
(KRITISCH: "Asynchronous Texture Streaming: false" verhindert, dass Texturen/Straßen durchscheinen oder Fragmente aufblitzen! "Minimum Scalable Dimension: 256" schützt 2D-Texturen vor Fehlskalierung!)

2. Erstelle "$bin\config\patch_config.yml":
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

(Wichtig: Alle PPU Model LOD Patches MÜSSEN false sein, damit HierLOD auf 1.0f bleibt und der Boden nicht verschwindet!)

3. Unterdrücke Erststart-Dialoge in "$bin\GuiConfigs\CurrentSettings.ini":
   [main_window]
   infoBoxEnabledWelcome=false

4. Controller-Strategie (WICHTIG – INDIVIDUELL & NICHT-INVASIV):
   - FALLS BEREITS RPCS3 INSTALLIERT IST oder Profile in "config/input_configs" existieren: 
     FASSE DIESE DATEIEN NICHT AN! Bestehende Profile des Nutzers haben strikten Vorrang.
   - FALLS ES EIN FRISCHES SYSTEM IST:
     Ein in Windows verbundener Controller ist in RPCS3 nicht automatisch Spieler 1 (RPCS3 nutzt zunächst Keyboard).
     Der Nutzer bzw. die KI weist den Controller direkt in RPCS3 unter "Pads" zu:
     `Pads -> Player 1 -> Handler: XInput -> Device: verbundenes Gerät (z. B. XInput Pad #1) -> Save`.
     Kopiere KEINE statischen fremden Profile ungefragt ins System.

- SHADER-MODUS & CACHE-VERHALTEN (VERBINDLICHE TEST-4-KLARSTELLUNG):
  1. Für den regulären Betrieb unter Video zwingend den bewährten Standard beibehalten:
     `Shader Mode: Async Recompiler (multi-threaded)`
     (Dies entspricht der Referenzkonfiguration aus `03_QUELLCODE/configs/custom_configs/config_BCUS98106.yml`).
  2. Wichtige Unterscheidung der drei Lade-Vorgänge:
     - PPU-Vorkompilierung: Übersetzung des PS3-CPU-Codes (~7.000 Basismodule beim Erststart).
     - Spiel-Shader-Cache: Die während des Spielens erzeugten echten 4K-Grafik-Pipelines (~8.500–9.000 Dateien nach Spielzeit).
     - Interpreter-Vorbereitung: Wird nur bei manuellem Interpreter-Modus ausgelöst (baut 6.630 Basisvarianten vor).
  3. Beim Aufbau eines frischen Caches können im ersten Match vereinzelt kurze Auslassungen auftreten.
     Nach dem ersten Fahren liegen die Shader im Cache und das Spiel startet dauerhaft sauber.

================================================================================
SCHRITT 6: AUTARKER ASSET-PATCHER (5000M WEITSICHT & ANTI-BLUR)
================================================================================
Verwende das autarke Python-Skript und die Delta-Tabelle aus dem GitHub-Ordner:
python "C:\path\to\mod-repository\03_QUELLCODE\apply_times_ngp_delta.py" `
  --disc-dir "<Pfad-zu-sauberem-Disc-USRDIR>" `
  --target-dir "$bin\dev_hdd0\game\BCUS98106\USRDIR"

Das Skript patcht SP- und MP-`times.psarc` auf die exakten Ziel-Hashes und setzt `Distance Blur Enable: 0` in `times.ltn` und `suburbs.ltn`.

================================================================================
SCHRITT 7: LAUNCHER & UMGEBUNGSVARIABLEN
================================================================================
Erstelle das PowerShell-Startskript "$bin\Start-TwistedMetal-4K.ps1":
param(
    [switch]$CheckOnly,
    [string]$GamePath = "C:\Users\LOCAL_USER\Desktop\ps3\Twisted Metal\2\Twisted Metal (USA) (En,Fr,Es).iso"
)
$binDir = $PSScriptRoot
$exe    = Join-Path $binDir "rpcs3.exe"

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
if (-not (Test-Path $exe)) { Write-Error "rpcs3.exe nicht gefunden"; exit 1 }
Start-Process -FilePath $exe -ArgumentList "`"$GamePath`"" -WorkingDirectory $binDir

================================================================================
SCHRITT 8: AUTOMATISIERTE VALIDIERUNG & ERFOLGSNACHWEIS
================================================================================
1. Führe zwingend den automatisierten 8-Punkte-Systemcheck aus:
   Kopiere "C:\path\to\mod-repository\03_QUELLCODE\Verify-TM4K-Setup.ps1"
   nach "$bin\Verify-TM4K-Setup.ps1" und führe aus:
   powershell -ExecutionPolicy Bypass -File "$bin\Verify-TM4K-Setup.ps1"

   Der Check prüft vollautomatisch:
   [1/8] Hardware-Erkennung (CPU & GPU)
   [2/8] RPCS3 Hauptprogramm (rpcs3.exe vorhanden & >30 MB)
   [3/8] TM4K RSX-Präsentations-Flags (alle 5 .flag-Dateien vorhanden)
   [4/8] SPU/PPU Patch-Definitionen (BCUS98106_patch.yml mit 11x NOP Bypass an 0x0066aaf4)
   [5/8] Patch-Aktivierung (PostFX Sharp Fast: true, Model LOD: false)
   [6/8] Spiel-Konfiguration (Scale 300, Async Textures: false, Min Scalable: 256)
   [7/8] Gamepad / Controller-Status (bestehende Profile geschützt oder individuelle Zuweisung)
   [8/8] Launcher & TM4K-Umgebungsvariablen (Start-TwistedMetal-4K.ps1 konfiguriert)

   Erst wenn die Ausgabe meldet:
   "ERGEBNIS: 8 / 8 PRÜFUNGEN ERFOLGREICH BESTANDEN! STATUS: Das System ist zu 100% startklar",
   ist der Build freigegeben! Falls ein Fehler gemeldet wird, korrigiere ihn sofort anhand der Skriptausgabe.

2. Melde die SHA256-Prüfsumme der neu erstellten "rpcs3.exe".
3. Erstelle eine Desktop-Verknüpfung "Twisted Metal 4K - Frischer Neubau.lnk", die auf dieses Startskript verweist.
4. Bestätige, dass:
   - Keinerlei Dateien aus Altbeständen kopiert wurden.
   - Alle 5 Flags vorhanden sind.
   - Die Flammen auf Sweet Tooths Kopf und die Menüschriften stabil stehen (kein Flackern).
   - Der Boden in Times Square zu 100% stabil ist.
```
