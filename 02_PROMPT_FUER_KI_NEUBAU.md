# Master-Prompt: Vollständiger KI-Neubau des Twisted Metal 4K Remasters

> Aktuelle öffentliche Bauanleitung und Grenzen: [BUILD.md](BUILD.md), [Teststatus](docs/REPRODUCTION.md). Ein Agent muss alle Voraussetzungen selbst prüfen; die Anleitung ist keine Neu-PC-Erfolgsgarantie.

> **Hinweis zur Verwendung:**  
> Dieser Prompt ist so formuliert, dass er direkt an eine KI (z. B. Gemini, ChatGPT/Codex oder Claude) in einem **frischen Chat** übergeben werden kann. Er enthält alle Erkenntnisse aus den erfolgreichen Builds vom 16. und 17. Oktober 2026 und ermöglicht den vollständigen Neubau von Grund auf, ohne auf bestehende Fertig-Builds zurückgreifen zu müssen.

---

> **Korrektur aus Test 4 vom 17.10.2026:** Für den regulären Neubau gilt
> `Shader Mode: Async Recompiler (multi-threaded)`, wie in den fertigen Tests 1/2 und 3.
> Die früher hier empfohlene Interpreter-Alternative verursachte eine zusätzliche,
> lange Vorberechnung von 6.630 Interpreter-Pipelines. Außerdem war ihr YAML-Wert
> falsch geschrieben. Details und die erforderliche Laufzeitprüfung stehen in Phase 4 und 8.

```markdown
Du bist ein erfahrener Systems-Engineer und Low-Level-Grafikprogrammierer. Deine Aufgabe ist es, den vollständigen 4K-Remaster-Build für "Twisted Metal (PS3, 2012 / BCUS98106)" auf RPCS3 von Grund auf sauber neu zu erstellen.

STRIKTE SPERR-REGEL (ISOLIERTER "NEUER PC"-MODUS):
Es ist dir STRENGSTENS UNTERSAGT, auf vorhandene, fertige oder frühere Versionen auf diesem PC zuzugreifen (z. B. "bin-twisted-metal-4k-stabilitats-test", "bin-twisted-metal-4k-sichtweite-test", "C:\tm4k\bin", "rpcs3-live-4k", "outputs\TwistedMetal4K" etc.).
Du darfst KEINE "rpcs3.exe", DLLs oder Konfigurationen aus Altordnern kopieren. Du musst agieren wie ein NEUER PC, der ausschließlich Zugriff auf die GitHub-Dateien aus "03_QUELLCODE" (Patch, Overlay, Patcher, Flags), die saubere Original-Disc und die Standard-Build-Werkzeuge hat!

================================================================================
EINGABEN & UMGEBUNGSVORAUSSETZUNGEN
================================================================================
Bevor du beginnst, stelle sicher, dass folgende Pfade und Werkzeuge definiert sind:

| Komponente | Erforderlicher Inhalt / Pfad |
|---|---|
| Quellcode-Dossier | `tracked-changes.patch`, Header-Overlay, 5 Aktivierungs-Flags |
| Asset-Patcher | `apply_times_ngp_delta.py` UND `times_ngp_deltas.json` (im selben Ordner!) |
| Disc-Dateien | Sauberes PS3-Disc-Verzeichnis (`PS3_GAME/USRDIR/`) |
| Spielupdate | BCUS98106 Update v01.06 (PPU-Hash: 6bb4db395550102f567fb7b1dbbf81583417150f) |
| PS3-Firmware | Installierte offizielle PS3-Firmware in RPCS3 (`dev_flash`) |
| Build-Tools | Visual Studio Build Tools 2026 (MSVC v143/v144 x64), CMake >= 4.2 |
| Qt 6 | Qt 6.11.2 (msvc2022_64) mit `bin/windeployqt.exe` und `lib/cmake/Qt6` |
| Vulkan SDK | Vulkan SDK 1.4.341.1 (Umgebungsvariable `VULKAN_SDK`) |
| LLVM | Statische RPCS3-LLVM-Bibliotheken (`/MT`), z. B. aus custom-build-win-22.1.8 |
| Python | Python 3.10+ (Standardbibliothek genügt für den Asset-Patcher) |

Windows-Sicherheit:
Keine globalen Sicherheitsfunktionen abschalten oder Defender-Ausnahmen automatisieren.
ExecutionPolicy ist keine Lösung für Smart App Control. Bei blockierter EXE den
konkreten Grund klären und die sicherheitsrelevante Entscheidung dem Nutzer lassen.

================================================================================
PHASE 1: RPCS3 REPOSITORY CHECKOUT & SUBMODULES (PFADLÄNGEN-SCHUTZ)
================================================================================
1. Wähle ein kurzes Arbeitsverzeichnis (z. B. `C:\tm4k` oder `C:\build\rpcs3`), um Probleme mit der Windows-Pfadlängenbegrenzung (MAX_PATH = 260 Zeichen) bei tief verschachtelten Submodulen zu vermeiden.
2. Klone das RPCS3-Repository und setze den Quellbaum exakt auf den verifizierten Basis-Commit zurück:
   git clone https://github.com/RPCS3/rpcs3.git rpcs3
   cd rpcs3
   git checkout 1707d7fc883ef48ff21bdcbb0141a3211ae09cb2
3. Initialisiere alle Git-Submodule rekursiv mit aktiviertem Langpfad-Support:
   git -c core.longpaths=true submodule update --init --recursive --jobs 8
4. Prüfe, ob alle Submodule fehlerfrei ausgecheckt wurden (nicht nur HEAD prüfen, sondern Verzeichnisinhalte validieren).

================================================================================
PHASE 2: QUELLCODE- UND SHADER-INTEGRATION (RPCS3-LIVE-4K)
================================================================================
Wende die im Dossier unter "03_QUELLCODE" gesicherten Modifikationen an:
1. Wende den Patch "tracked-changes.patch" auf den Quellbaum an:
   git apply tracked-changes.patch
   (Betrifft: CMakeLists.txt, RSXDrawCommands.cpp, rsx_utils.h, VKDraw.cpp, 
    VKGSRender.cpp/.h, VKPresent.cpp, VKRenderTargets.cpp/.h, Fragment-Interpreter/Prolog)
2. Kopiere alle TM4K-spezifischen Dateien und Header aus "03_QUELLCODE/overlay/rpcs3/Emu/RSX/VK/" 
   in das Zielverzeichnis "rpcs3/Emu/RSX/VK/":
   - TM4KNativeColour.hpp, TM4KHybrid.hpp, TM4KMaster.hpp, TM4KParticles.hpp
   - TM4KMicro.hpp, TM4KAffine.hpp, TM4KTiming.hpp, TM4KDrawGuard.hpp
   - VKPresent.cpp, VKDraw.cpp, VKGSRender.cpp/.h, VKRenderTargets.cpp/.h
3. Stelle sicher, dass in "TM4KNativeColour.hpp" die beruhigte Schärfekalibrierung aktiv ist:
   clamp(detail, -0.08, 0.08) * 0.45 * geo_mask;  // d_clamp = 0.45
4. Stelle sicher, dass in "VKPresent.cpp" die gehärtete Präsentationslogik aktiv ist:
   - tm4k_crop_inset = true (KRITISCH: muss einheitlich aktiv sein, damit 3D-Spielszene und 2D-HUD/Ladeanzeigen 
     exakt dieselbe Skalierung (117,72)..(3723,2088) nutzen und es zu KEINERLEI Bildverdopplung oder Versatz kommt!)
   - Referenz-Präsentationspfad: kein Gating auf Framebuffer-Adressen (nicht auf 0xc0556000 einschränken!):
     - Twisted Metal wechselt zwischen 0xc0556000 (3D-Szene) und 0xc0010000 (2D-Overlay/HUD/Menü).
     - Der Fusions-Shader blendet das 2D-Overlay (screen.rgb) auf die 4K-HDR-Szene (fused) ein:
       mix(fused, screen.rgb, overlay).
     - Beide Adressen müssen regulär über "if (packet)" laufen. Wird 0xc0010000 ausgeschlossen, wechseln 3D-Szene und 2D-HUD 
       mit jedem Frame ab (30 Hz Stroboskop) -> das Feuer auf Sweet Tooths Kopf und die Schrift flackern extrem!
   - 30-Frame-Latch (~500 ms):
     - else if (s_last_fused_image && s_fused_latch_frames < 30)
     - Hält bei GPU-Spitzen, Explosionen und Framedrops das letzte gültige 4K-Bild stabil und verhindert, 
       dass jemals auf un-tonemappte FP16-Rohpuffer zurückgefallen wird -> soll Rohpuffer-Fallbacks vermeiden; Verhalten live prüfen.
   - TM4K_COHORT_WINDOW = 4000
   - max_intra_gap = 2500
   - Sicherheitsrand-Crop: Quellregion (39*scale, 24*scale) bis (1241*scale, 696*scale) auf volle 3840x2160 gestreckt.

================================================================================
PHASE 3: CMAKE-KONFIGURATION & COMPILIERUNG MIT MSBUILD
================================================================================
1. Vollständiger CMake-Aufruf (ohne Auslassungspunkte):
   Definiere die Pfade zu Quellen ($src), Build ($build), Qt ($Qt) und LLVM ($LLVM).
   Wichtig: Setze zwingend -DLLVM_USE_INTEL_JITEVENTS=ON (verhindert LNK2001-Fehler bei IntelJITEventListener)
   und -DCMAKE_DISABLE_FIND_PACKAGE_SDL3=ON (verhindert Target-Konflikte mit internem SDL).

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

2. Kompilierung mit MSBuild:
   Um Speicherüberläufe auf Systemen mit <= 32 GB RAM zu vermeiden, begrenze die Parallelität (/m:4 /p:CL_MPcount=2):
   a. Emulator-Kern:
      msbuild "$build\rpcs3\Emu\rpcs3_emu.vcxproj" /p:Configuration=Release /p:Platform=x64 /m:4
   b. Hauptanwendung:
      msbuild "$build\rpcs3\rpcs3.vcxproj" /p:Configuration=Release /p:Platform=x64 /m:4

3. Zielverzeichnis & Qt-Bereitstellung (Pflichtschritt):
   Erstelle das Ausführungsverzeichnis (z. B. "$bin") und kopiere die erzeugte "rpcs3.exe" hinein.
   Führe zwingend windeployqt aus, um alle erforderlichen Qt-Bibliotheken, Plugins und QML-Module bereitzustellen:
   & "$Qt\bin\windeployqt.exe" --release --no-translations "$bin\rpcs3.exe"

4. Kopiere die 5 Aktivierungs-Flags aus "03_QUELLCODE/flags/*.flag" direkt in das Verzeichnis von "rpcs3.exe":
   - tm4k-enable.flag
   - tm4k-lit-c-enable.flag
   - tm4k-lit01-enable.flag
   - tm4k-micro-e-enable.flag
   - tm4k-packed-alpha-enable.flag
   (Wichtig: VKPresent.cpp liest diese Flags aus fs::get_config_dir(). Ohne sie bleibt der 
    4K-Fusionspfad inaktiv, der Rahmen ungeschnitten und das Bild in 720p!)

================================================================================
PHASE 4: EMULATOR-KONFIGURATION (CONFIG_BCUS98106.YML)
================================================================================
Erstelle oder kopiere die spielscharfe Konfiguration aus "03_QUELLCODE\configs\custom_configs"
nach "<bin-folder>\config\custom_configs\config_BCUS98106.yml" (oder verwende folgende Parameter):
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
(Hinweis: "Adapter: `"`"`" aktiviert die automatische Erkennung der primären dedizierten GPU – ist kein Hardware-Kompatibilitätsnachweis; das tatsächlich gewählte Gerät prüfen!)
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
(KRITISCH: "Asynchronous Texture Streaming: false" verhindert durchscheinende Texturen/Straßen und Artefakte! "Minimum Scalable Dimension: 256" schützt 2D-Masken!)

RPCS3-Dialoge & Controller:
- In "GuiConfigs/CurrentSettings.ini" unter [main_window] den Parameter "infoBoxEnabledWelcome=false" setzen,
  damit der RPCS3-Erststart-Dialog nicht blockiert.
- Controller-Strategie (WICHTIG – INDIVIDUELL & NICHT-INVASIV):
  1. Falls auf dem System bereits RPCS3 installiert ist oder bereits Controller-Konfigurationen in 
     "<bin-folder>/config/input_configs" existieren: FASSE DIESE DATEIEN NICHT AN! 
     Bestehende Profile des Nutzers (Xbox, DualSense, Lenkrad) haben strikten Vorrang.
  2. Falls es sich um eine völlig frische Installation ohne Controller-Dateien handelt:
     Der Nutzer konfiguriert seinen Controller direkt im RPCS3-Menü unter "Pads" (XInput für Xbox/PC, 
     DualSense/DualShock für PlayStation, SDL für Drittanbieter-Pads).
     Kopiere KEINE statische Default.yml mit festen fremden Vendor-/Product-IDs ungefragt ins System, 
     sondern verwende eine zum tatsächlich angeschlossenen Gerät passende Zuordnung.
     Ein automatisches Entkoppeln unter Windows durch solche Profildateien wurde in Test 4 nicht belegt.
  3. Windows-Verbindung und RPCS3-Zuweisung getrennt prüfen:
     Ein bereits in Windows verbundener Controller ist in einer frischen RPCS3-Installation
     nicht automatisch Spieler 1. Ohne Eingabeprofil verwendet dieser Quellstand zunächst Keyboard.
     In Test 4 war ein Xbox-Wireless-Controller bereits über den Windows-Adapter verbunden;
     fehlend war ausschließlich die RPCS3-Zuordnung. Unter Pads -> Player 1 -> Handlers:
     XInput und unter Devices das tatsächlich verbundene Gerät wählen (hier XInput Pad #1),
     Belegung prüfen und Save drücken. Diese Nummer ist ein lokales Beispiel und kein
     universeller Wert für andere PCs. Vorhandene funktionierende Profile erhalten.
     Das Laufzeitlog muss die geladene Profil-Datei und eine verbundene XInput-Instanz zeigen;
     abschließend den Nutzer Menü und Fahrzeug tatsächlich steuern lassen.
     Keine Windows-Neukopplung veranlassen, wenn das Gerät dort bereits verbunden ist.

- Shader-Modus, Kaltstart und Cache – verbindliche Test-4-Korrektur:
  1. Für den regulären Neubau unter Video exakt setzen:
     Shader Mode: Async Recompiler (multi-threaded)
     Dies entspricht den verglichenen fertigen Tests 1/2 und 3 sowie der mitgelieferten
     Datei 03_QUELLCODE/configs/custom_configs/config_BCUS98106.yml.
     Diese Einstellung nicht automatisch durch einen Interpreter-Modus ersetzen.
  2. Drei verschiedene Dinge unterscheiden:
     - PPU-Vorkompilierung: Übersetzung des PS3-CPU-Codes mit LLVM.
     - Spiel-Shader-Cache: während des Spielens entstehende Grafikprogramme/Pipelines.
     - Interpreter-Vorberechnung: zusätzliche allgemeine Vulkan-Pipelines des Shader-
       Interpreters. In Test 4 lautete die Anzeige "Precompiling shader interpreter
       variants" / "Building base variant ... of 6630". Dieser Schritt war die Ursache
       der beanstandeten langen Wartezeit, nicht das normale Laden des Spiel-Shader-Caches.
     Dateizahlen wie 7.000 oder 9.000 vermischen möglicherweise mehrere Cache-Arten und
     sind weder ein verlässlicher Fortschrittsmaßstab noch ein Qualitätsnachweis.
  3. Nur bei ausdrücklich gewünschtem Interpreter-Betrieb ist der gültige YAML-Wert:
     Shader Mode: Async Recompiler with Shader Interpreter
     ACHTUNG: "Async with Shader Interpreter" ist in diesem Basis-Commit ungültig!
     RPCS3 meldet dann "invalid enum or integer" und behält den vorherigen bzw.
     Standardwert. Der Standard dieses Quellstands ist der Interpreter-Modus.
     Auch mit dem korrekt geschriebenen Interpreter-Wert wird dessen zusätzliche
     Vorberechnung ausgelöst; die Schreibkorrektur allein behebt die lange Wartezeit nicht.
     Startdauer hängt unter anderem von GPU, Treiber und Cachezustand ab.
     Der Interpreter ist kein CPU-Software-Renderer: Er führt PS3-Shader über auf der
     GPU laufende Interpreterprogramme aus. Er kann fehlende kompilierte Spiel-Shader
     überbrücken, garantiert aber weder fehlerfreie Bilder noch einen schnellen Erststart.
  4. Mit dem normalen Async-Recompiler können beim Aufbau eines frischen Spiel-Shader-
     Caches vorübergehende Auslassungen oder Ruckler auftreten. Menü und die vorgesehenen
     Levels praktisch prüfen und anschließend einen zweiten Start kontrollieren.
     Keine fremden fertigen Caches übernehmen und keine pauschale Fehlerfreiheit nach
     einer bestimmten Minutenzahl oder Dateizahl behaupten.

================================================================================
PHASE 5: PPU- UND SPU-PATCHES (PATCHES-ORDNER & PATCH_CONFIG.YML)
================================================================================
1. Kopiere zwingend die Patch-Definitionsdateien:
   Kopiere "03_QUELLCODE/configs/patches" nach "<bin-folder>/patches"
   (Enthält "BCUS98106_patch.yml" und "patch.yml". KRITISCH: Ohne diese Definitionsdateien kann RPCS3 den PostFX-Bypass nicht anwenden – Sweet Tooths Flammen werden dann cyan/blau dargestellt und am unteren Bildrand erscheint ein giftgrüner Streifen!)

2. In "<bin-folder>\config\patch_config.yml" für PPU-6bb4db395550102f567fb7b1dbbf81583417150f 
und SPU-82b3399c8e6533ba991eedb0e139bf20c7783bac aktivieren:
- "60 FPS": true
- "PostFX Sharp Fast": true (11 NOP-Bypasses)
- "Raw 4K Scene": false (wichtig: nicht aktivieren!)
- "Extend Draw Distance": true (10k Far Clip / 16k Fog)
- "Model LOD Distance +50%": false
- "Model LOD Distance +100% (2x)": false (KRITISCH: muss FALSE sein!)
- "Model LOD Distance x4": false
- "Force Near Model LOD - distance zero test": false
- "Disable SPU MLAA - Twisted Metal, Resident Evil: Operation Raccoon City": true
(Hintergrund: HierLOD-Faktor an 0x0010e83c MUSS auf nativem 1.0f verbleiben, 
um asynchrones Mesh-Streaming nicht zu desynchronisieren!)

================================================================================
PHASE 6: ASSET-LOD-PATCHING (5000M WEITSICHT & ANTI-BLUR)
================================================================================
1. Platziere "apply_times_ngp_delta.py" und "times_ngp_deltas.json" GEMEINSAM im selben Arbeitsordner.
2. Führe den autarken Asset-Patcher aus:
   python apply_times_ngp_delta.py --disc-dir "<Disc-USRDIR>" --target-dir "<Ziel-USRDIR>"

Das Skript erledigt vollautomatisch:
1. SP-Archiv: "levels/times.psarc"
   - Clean Disc SHA256: AD4A3C6E7501AA2BEA6258210F09B78A09EABB981DE3D748885D84546380CD71
   - Ziel-Pfad: "dev_hdd0/game/BCUS98106/USRDIR/levels/times.psarc"
   - Gepatchter SHA256: 9129889C700E23CB61569EA90722E0E8F0A64B00CB2D4F4750BAA51DD407A22C
2. MP-Archiv: "mplevels/times.psarc"
   - Clean Disc SHA256: 0CF5348D405E295E9D1514F2355A248999E5F7EFA1BFBD0FC35D49936C1DFA1C
   - Ziel-Pfad: "dev_hdd0/game/BCUS98106/USRDIR/mplevels/times.psarc"
   - Gepatchter SHA256: 757C01A688C74918AB9A1CBBBE59BF1D8787D0C5BA44391059E798E96159E90A
3. Level-Konfigurationen: "levels/times.ltn" & "levels/suburbs.ltn"
   - Setzt "Distance Blur Enable: 0" (verhindert trüben Entfernungsnebel über den 5000m-Objekten).
Filterregel: Modifiziere nur NGP-HierLOD-Knoten mit term <= 3000.0 und sphere[3] <= 80.0. 
Setze Near=5000.0, Term=5600.0, Load=5200.0, Unload=5500.0. 
Große Geometrien (Gebäude mit term=10000, Straßen) bleiben unangetastet.

================================================================================
PHASE 7: LAUNCHER & PROZESS-UMGEBUNG
================================================================================
Erstelle das Startskript "Start-TwistedMetal-4K.ps1".
Setze vor dem Prozessstart von rpcs3.exe zwingend folgende Umgebungsvariablen:
$env:TM4K_SCENE_2160       = '1'
$env:TM4K_GEOMETRY_LAG1    = '0'
$env:TM4K_COHORT_WINDOW    = '4000'
$env:TM4K_INTRA_GAP        = '2500'
$env:TM4K_MASTER_DENSITY   = '7'
$env:TM4K_PARTICLE_RESTORE = '1'
$env:TM4K_DETAIL_PALETTE   = '1'
$env:RPCS3_CONFIG_DIR      = "$PSScriptRoot\"

Starte RPCS3 über das PowerShell-Skript (oder eine Desktop-Verknüpfung auf dieses Skript),
damit die Umgebungsvariablen und der Konfigurationspfad zuverlässig übergeben werden.

================================================================================
PHASE 8: AUTOMATISIERTE VALIDIERUNG & ABSCHLUSSPRÜFUNG
================================================================================
1. Führe zwingend den automatisierten 8-Punkte-Systemcheck aus:
   Kopiere "03_QUELLCODE/Verify-TM4K-Setup.ps1" nach "<bin-folder>/Verify-TM4K-Setup.ps1" und führe aus:
   powershell -ExecutionPolicy Bypass -File "<bin-folder>\Verify-TM4K-Setup.ps1"

   Der Check prüft vollautomatisch:
   [1/8] Hardware-Erkennung (CPU & GPU)
   [2/8] RPCS3 Hauptprogramm (rpcs3.exe vorhanden & >30 MB)
   [3/8] TM4K RSX-Präsentations-Flags (alle 5 .flag-Dateien vorhanden)
   [4/8] SPU/PPU Patch-Definitionen (BCUS98106_patch.yml mit 11x NOP Bypass an 0x0066aaf4)
   [5/8] Patch-Aktivierung (PostFX Sharp Fast: true, Model LOD: false)
   [6/8] Spiel-Konfiguration (Scale 300, Async Textures: false, Min Scalable: 256,
         Shader Mode exakt: Async Recompiler (multi-threaded))
   [7/8] Gamepad / Controller-Status (bestehende Profile geschützt oder individuelle Zuweisung)
   [8/8] Launcher & TM4K-Umgebungsvariablen (Start-TwistedMetal-4K.ps1 konfiguriert)

   Die Ausgabe "ERGEBNIS: 8 / 8 PRÜFUNGEN ERFOLGREICH BESTANDEN!" bestätigt nur die
   im Skript implementierten Setup-Prüfungen. Sie ist keine vollständige Funktionsfreigabe.
   Insbesondere kann Punkt 7 auch ohne Controllerprofil bestehen; das bestätigt keine
   funktionierende Gamepad-Eingabe. Spielstart, effektiv geladene Konfiguration, Controller
   und Grafik müssen zusätzlich praktisch geprüft werden. Fehler anhand der Ausgabe beheben.

2. Prüfe die Prüfsummen der erzeugten rpcs3.exe und der gepatchten Archive.
3. Führe einen Probelauf durch:
   - Nach dem Beenden im frischen Laufzeitlog "<bin-folder>/log/RPCS3.log" prüfen:
     Der Abschnitt "Used configuration" muss "Shader Mode: Async Recompiler (multi-threaded)"
     enthalten. Keine "invalid enum or integer"-Meldung zu diesem Wert akzeptieren.
     Die Prüfung allein anhand der YAML-Datei reicht nicht aus.
   - Bei "Precompiling shader interpreter variants" / "... of 6630" zuerst den effektiv
     geladenen Shader-Modus und den benutzten Konfigurationspfad prüfen. Nicht mit einem
     anderen Emulator-Build oder kopierten Alt-Caches verdecken.
   - Menü: Texturen, Schriften und Maske glasklar, ohne schwarze Balken, keine doppelten Schriften.
   - Gameplay: Fahre Times Square und Sun Valley (Sand-Level).
   - Prüfe: 100% stabiler Boden, keine schwarzen Kacheln, kein Rauschen auf Sand/Felsen,
     glasklare Kanten auch beim Rammen von Wänden/Gegnern, Sweet Tooth Flammen intakt.
4. Beachten beim Beenden:
   - Eine interne Assertion-Meldung in `vm::close` beim Stoppen des Emulators ist ein bekannter
     RPCS3-Shutdown-Hinweis und beeinträchtigt das eigentliche Spielgeschehen nicht.

5. Nutzerbefund aus Test 4 nach den Korrekturen:
   Der Nutzer hat das Spiel mit seinem Controller selbst ausprobiert. Am Anfang traten
   kleine Grafikfehler auf, danach lief es nach seiner Aussage normal. Ein Zusammenhang
   mit dem ersten Shader-/Pipeline-Cache-Aufbau ist eine Vermutung, keine nachgewiesene
   Ursache. Zur Unterscheidung dieselbe Szene bei einem zweiten Start erneut prüfen.
   Bleiben die Fehler bestehen oder treten sie reproduzierbar wieder auf, separat untersuchen.
   Daraus keine allgemeine Fehlerfreiheit oder vollständige Abnahme beider Testlevels ableiten.
```
