# Neubau von Grund auf – reproduzierbare Reihenfolge und Freigabe

Diese Anleitung bereitet den vom Nutzer geplanten sauberen Neubau vor. Sie
behauptet NICHT, dass ein zweiter Clean-Build heute bereits gelaufen ist.
Gemini ergänzt zuerst die konkrete finale Releasezuordnung im Ergänzungsblatt.

## A. Ein eindeutiges Referenz-Release festlegen

Die alten September- und Oktober-Fassungen nicht vermischen. Maßgeblich sind:

1. Ausgewählter Desktop-Link und dessen aktueller Launcher.
2. Zu genau diesem Launcher gehörende EXE/DLLs, Flags, Spielprofil,
   Patchdefinitionen UND Patch-Enables.
3. Benutzte privaten SP-/MP-Leveldateien einschließlich ihrer Input-/Outputhashes.
4. Dazu passender C++-/Shader-Quellstand und Buildprotokoll.
5. Alle prozesslokalen TM4K-Variablen und gegebenenfalls HDR-Profilierung.

`04_RELEASE_SNAPSHOTS/candidate-hashes.json` enthält die beim Sammeln wirklich
errechneten EXE-Hashes. Ältere `build-status.json`-Hashes dürfen davon abweichen,
wenn Gemini denselben Pfad später neu gebaut hat. Abweichung erklären, nicht
einfach den jüngsten Fließtext für wahr erklären. Die gemeinsame Arbeitsquelle
ist nicht zwangsläufig exakt die Quelle jeder älteren noch vorhandenen EXE.

## B. Saubere Spiel- und Emulatorbasis

- Eigene Twisted-Metal-USA-Kopie **BCUS98106**, Update **01.06**, zum PPU-Key
  `PPU-6bb4db395550102f567fb7b1dbbf81583417150f` passend.
- Historisches ISO-Identifikationsmaß:13465026560Bytes, SHA256
  `7AEDAE6FE8DB73F4F8376437276EC06E35804BCEF4FCC7F8F567C24005DA5231`.
  Nicht in diesem Sammelschritt neu über die gesamte ISO nachgerechnet.
- Eigene passende PS3-Firmware installieren. Kein ISO/Firmware-Upload.
- Erst unveränderten Spielstart auf sauberem RPCS3-Profil bestätigen, dann neue
  private Mod-Fassung anlegen. Keinen alten Save-/Displayzustand unbemerkt erben.
  Ingame-Screen-Border-Werte müssen mit dem verwendeten Cropmodell übereinstimmen.

## C. Quellstand exakt herstellen

Basiskommit und Submodulstände stehen unter `03_QUELLCODE`:

```text
1707d7fc883ef48ff21bdcbb0141a3211ae09cb2
```

Auf sauberem Checkout dieser Basis die dokumentierten Submodule herstellen.
Dann **eine** der folgenden Methoden benutzen:

- Die Dateien aus `03_QUELLCODE/overlay` unter Erhalt ihrer relativen Pfade in
  die saubere Quelle kopieren. Das sind geänderte getrackte Dateien UND lokale
  neue TM4K-Header. Dies ist die einfachste vollständige Overlay-Methode.
- Oder `tracked-changes.patch` anwenden und anschließend nur die in Git zuvor
  ungetrackten neuen TM4K-Dateien ergänzen. Nicht denselben Diff doppelt auf das
  bereits vollständig kopierte Overlay anwenden.

`git-status.txt`/`diff-stat.txt` dokumentieren, welche Änderungen dazu gehören.
Die elf getrackten Änderungen umfassen CMake, RSXDrawCommands, Interpreter-/
Fragmentprolog, rsx_utils und mehrere VK-Dateien. Neue Header wie
TM4KNativeColour/Hybrid/Timing/Particles/Affine/Master/Micro/DrawGuard ebenfalls
mitnehmen; nur VKPresent.cpp allein reicht NICHT. Eine `.bak` ist Archiv, nicht
automatisch eine aktive Compilequelle.

Historische frühere Quellen zum Rückvergleich liegen in den Checkpoints. Wenn
die finale EXE einen anderen Quellstand benutzt, zuerst diesen Diff ergänzen,
nicht stillschweigend „aktueller Arbeitsbaum = finale Quelle“ voraussetzen.

## D. Buildumgebung und Reihenfolge

Erhaltenes RPCS3-BUILDING.md und lokale CMakeCache/Projektdateien sind beigefügt.
Dokumentierte hiesige Umgebung: VisualStudio2026/BuildTools, Qt6.11.2,
VulkanSDK1.4.341.1; VS2026 benötigt laut mitgelieferter Builddoku CMake4.2+.
Tatsächliche lokalen Abhängigkeitspfade aus CMakeCache lesen und auf dem neuen
PC neu konfigurieren, nicht fremde absolute Pfade blind übernehmen.

Ein Quellverzeichniswechsel erfordert eine frische CMake-/VS-Konfiguration:
lokale vcxproj/CMakeCache sind Nachweise, keine portable Konfiguration.
Abhängigkeiten gemäß dem festgehaltenen Commit/BUILDING.md herstellen.

Reihenfolge des hiesigen inkrementellen Builds als Referenz:

```powershell
& $msbuild "$build\rpcs3\Emu\rpcs3_emu.vcxproj" /p:Configuration=Release /p:Platform=x64 /m
& $msbuild "$build\rpcs3\rpcs3.vcxproj" /p:Configuration=Release /p:Platform=x64 /m
```

`$msbuild` und `$build` auf die neu eingerichtete Umgebung setzen. Das ist kein
vollständiger frischer CMake-Konfigurationsbefehl; den für die Umgebung aus der
beigefügten Builddoku übernehmen. Den Randfix-Fehler nicht wiederholen: nur EXE
linken, während die geänderte Emulatorbibliothek alt bleibt. Beide Projektstufen
und vorhandene Shader-Validatoren ausführen; Buildlogs/SHA256 speichern.

Die04-Releases sind Referenzbinaries, nicht Ersatz für den Clean-Build. Eine
unsignierte lokale EXE kann von Windows blockiert werden. Keine Sicherheits-
Deaktivierung automatisieren und ein Signaturproblem nicht als Grafikcrash deuten.

## E. Performance-Patches in richtiger Version installieren

Definitionen aus der ausgewählten Referenzfassung übernehmen; dazu passend
Patch-Enables installieren. Voraussetzung: richtige Spiel-/PPU-/SPU-Version.

Grundlage laut gesicherter aktiver Sichtweite-Konfiguration:

- `60 FPS`: an; offizieller Patch in `patch.yml`, illusion zuschreiben.
- `Disable SPU MLAA - Twisted Metal, Resident Evil: Operation Raccoon City`: an;
  Whatcookie-Beitrag, genauer SPU-Key aus Konfiguration.
- `PostFX Sharp Fast v0.5`: an, elf NOP-Adressen aus Chronik/BCUS98106_patch.yml.
- `Raw 4K Scene`: aus, nicht zusätzlich die drei verbliebenen Farb-/Bloom-
  Callsites als vermeintlichen Performancegewinn entfernen.
- Keine frühen Guest-Puffer-/12Plane-/14Plane-Patches aus Analyseentwürfen erben.
- Kein aktivierter globaler Nahmodellfaktor0 als Teil der ursprünglichen
  gefixten Standardfassung, sofern das Final-Release nicht ausdrücklich anders
  belegt wurde. Im gesicherten aktuellen Sichtweite-Profil sind0/0,25/0,5 aus.

## F. PostFX-Rekonstruktion, UI, Rand, Feuer und Timing vollständig übernehmen

Nicht nur ein Offline-Masterbild als Wandtextur einsetzen. Die Pipeline arbeitet
pro Frame mit hochaufgelöster Szene, nativer Farbreferenz, originaler UI und
Partikeln. Alle relevanten Header/Quellen, Bindungen und Flags übernehmen.
Register-/Resource-IDs einzelner Captures sind Diagnosewerte, keine für jeden
Frame gültigen Konstanten. Runtime-Gates/Title-ID-Prüfungen aus der Quelle erhalten.

Aktueller Sichtweite-/Stabilitäts-Launcher setzt:

```text
TM4K_SCENE_2160=1
TM4K_GEOMETRY_LAG1=0
TM4K_COHORT_WINDOW=4000
TM4K_INTRA_GAP=2500
TM4K_MASTER_DENSITY=7
TM4K_PARTICLE_RESTORE=1
TM4K_DETAIL_PALETTE=1
```

Frühere getestete C/R/S-Launcher verwendeten GeometryLag1=1 und Cohort1600.
Nicht versehentlich diese alten Werte in die neue Variante mischen. Diagnose-
Environment und Vulkan-RenderDoc-Variablen für normalen Start sauber behandeln.
Für RenderDoc dieselbe EXE und dieselbe Spielumgebung benutzen; nachweisen,
dass renderdoc.dll geladen wurde. F12 ist eine Frameaufnahme, nicht die ganze Fahrt.

Rand: Titel-/Auflösungs-/Ingame-Displayabhängige Quellregion aus finaler Quelle
beibehalten. HUD/Logos und Pausenbild nicht aus der bloßen3D-Fläche rekonstruieren.
Feuer: native Partikel-/HDR-Isolation und räumliche Zuordnung erhalten.
Operation2D: gehört zu späteren Rekonstruktionsänderungen; finalen Headerstand
übernehmen, nicht nur die ältere September-Fusion mit neustem Launcher starten.

## G. Level-Assets und Sichtweite reproduzierbar erzeugen

Native Details in der Ferne sind eine getrennte Baustelle von4K-Ausgabe. Ein
Spielmodell kann weit entfernt einen anderen LOD/Mip verwenden. Beides nicht
mit8294400Framebuffer-Pixeln verwechseln.

Die frühe sichere gezielte Lampenfolge N/O/R/S ist mit Original-/Zielbytes,
Archive-Hashes und Validatoren im09.Oktober-Checkpoint vollständig vorhanden.
Die spätere Gemini-Fassung erweitert weitere times/suburbs-Knoten. Die Scripts
liegen u.a. im kopierten Desktopordner `twisted metal gemini` und Checkpoints.
Ein Teil dieser Scripts patcht konkrete absolute Pfade IN PLACE und ist daher
kein gefahrloser Installer: zuerst eigene private Inputkopie, erwarteten
Inputhash und vorhandenen Stand prüfen; dann einmalig reproduzierbar erzeugen.

Pflicht für die finale Ergänzung durch Gemini:

1. Unveränderter Input proSP/MP-Levelarchiv und dessen SHA256.
2. Genaue Node-/Float-/Mesh-Liste, semantische Zuordnung/Beleggrad.
3. Abhängige Nah-/Fern-/Endgrenzen und reale Lade-/Freigabegrenzen konsistent.
4. Alle übrigen Archivmitglieder/Koordinaten unverändert, besonders Gebäude.
5. Outputhash und passend aktivierte/deaktivierte PPU-Patches festhalten.

Faktor0,5 all global plus hohe Assetgrenzen kann mehr Arbeit und Weltfehler
erzeugen. Das im Bericht erwähnte „native1,0 + asset5000“ muss mit tatsächlichem
patch_config.yml übereinstimmen. Kein float5000 automatisch als5000Meter bezeichnen.
Die02Desktop-Rezepte sind historische Werkzeuge, nicht alle nacheinander ausführen.

## H. Profile, Ausgabe und AutoHDR

Gesichertes aktuelles Sichtweite-Spielprofil: Vulkan, Basis1280x720,300%,Limit60,
AsyncRecompiler,VSyncDisabled,TextureLODBias-0,1,CAS4. Diese Angaben stammen aus
dem tatsächlich gelesenen Profil, nicht aus einem älteren Konfigurationsbericht.
Andere Releaseprofile können abweichen: vor Freigabe konkret vergleichen.

Originalnebel und abgeschaltetes Blur getrennt behandeln. Windows-AutoHDR,
Display-HDR und Emulatorfarben sind verschiedene Dinge. Vorhandene per-EXE-
Windows-GPU-/AutoHDR-Einstellungen sind externe Profilabhängigkeiten; ein neuer
EXE-Pfad kann eine neue Zuordnung brauchen. Keine komplette private Registry
übertragen. Aktuelle Werte/Abwesenheit im Zusatz-Snapshot dokumentieren.
Controllerprofil und Save-/Displayzustand explizit sichern oder sauber initialisieren.

## I. Endgültiger Neustart-/Spieltest und Vergleich

1. EXE--version/Loader; Shader- und Opcode-/Assetvalidatoren; Versionshashes.
2. Kaltstart bis Warntext,Sony,SantaMonica,Deckel: nichts darf durchsichtig sein,
   Straße durchscheinen oder schwarz flackern.
3. OnePlayer/Story-Balken, Fahrzeugauswahl inkl.Motorradfahrer-Gesicht und
   Kopf-Feuer; Menüschattierung, UI-Grenzen und Autos prüfen.
4. Helles Dorf und dunkleArena/TimesSquare; gleiche Standpunkte wie Referenz.
   Auto/Fahrer/Rücklichter, Straße/Häuser, Himmel, Baumkronen, Xmas-Lampen,
   Geländer, Nebel und Haut-/Schattenkanten vergleichen.
5. Beide Fahrtrichtungen und rasche Drehungen auf problematischer Straße;
   ganze Häuserblöcke/Boden durchgehend sichtbar. Mehrminütig, nicht nurStart.
6. Kollisionen, Raketen, driftende Gegner, Pendelfallen: keine groben720p-
   Rückfälle; kein Nachziehen/Geisterbild durch überalterte oder gehaltene Frames.
7. Storyvideo vor/nach Level, Zurück-ins-Menü, Pause, erneuterMatch, Neustart.
8. Warm/Kalt-Frametimes undFPS für mehrereLevel; Auflösung,GPU-Treiber,Hardware,
   Laufdauer und genaue Szene mitloggen. Anzeige60 alleine reicht nicht.
9. Bei Fehler gezielte Captureaufnahme und Livezustand festhalten. Shutdown-
   SpuPostFX-Fehler nicht ohne Zeitbezug als Ursache einer vorherigen Störung werten.

Abnahme in eine neue STATUS/RESULT-Datei schreiben: **Build PASS**, **GPU-Replay
PASS**, **Nutzer-Spieltest PASS** und **langer Benchmark PASS** getrennt markieren.
Nur wenn der frische Nachbau dieselben sichtbaren Ergebnisse erzeugt, ist die
Anleitung praktisch nachgebaut. Danach erst sauberes öffentliches Patchpaket ableiten.
