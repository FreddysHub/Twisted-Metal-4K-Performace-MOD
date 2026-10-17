# Twisted Metal 4K – Architektur & Komponenten-Zuordnung

**Projekt:** Twisted Metal (2012, PS3 – BCUS98106 v01.06)  
**Dokumenttyp:** Technische Systemarchitektur, Modulzuordnung & Fehlersymptom-Atlas  
**Zielgruppe:** KI-Agenten (Gemini, Claude, Codex), Entwickler, Modder und Endanwender  
**Stand:** 17. Oktober 2026  

---

## 1. Übersicht: Das 5-Säulen-Architekturmodell

Die Rekonstruktion von *Twisted Metal (2012)* in nativem 4K mit 60 FPS und 5.000m Weitsicht auf RPCS3 ist keine einfache Auflösungsskalierung, sondern ein hochgradig verzahntes Zusammenspiel aus **fünf synchronisierten Säulen**. 

Wenn auch nur **eine einzige** Komponente an der falschen Stelle sitzt oder fehlt, bricht das System mit spezifischen Grafik- oder Steuerungsfehlern ein.

```
+-----------------------------------------------------------------------------------+
|                        TWISTED METAL 4K GESAMTSYSTEM                              |
+-----------------------------------------------------------------------------------+
        |                    |                    |                   |
        v                    v                    v                   v
+------------------+ +------------------+ +-----------------+ +------------------+
|   SÄULE 1:       | |   SÄULE 2:       | |   SÄULE 3:      | |   SÄULE 4:       |
| RSX Vulkan Core  | | PS3 Binary-Patch | | Host-Config     | | Level- & Assets  |
| (rpcs3.exe)      | | (BCUS98106_patch)| | (config_BCUS)   | | (times.psarc)    |
| - Bilateral 4K   | | - 11x NOP Bypass | | - 300% Scale    | | - 5000m HierLOD  |
| - HDR-Bloom-Fuse | | - SPU MLAA Off   | | - Async-Tex OFF | | - Anti-Blur      |
| - 5x TM4K Flags  | | - Draw Dist 5km  | | - Universal GPU | | - Delta-Patcher  |
+------------------+ +------------------+ +-----------------+ +------------------+
        |                    |                    |                   |
        +--------------------+--------------------+-------------------+
                                      |
                                      v
                             +------------------+
                             |   SÄULE 5:       |
                             | Input & Launcher |
                             | (Individuell)    |
                             | - Eigene Pad-Wahl|
                             | - 7x TM4K EnvVars|
                             | - Selbst-Prüfung |
                             +------------------+
```

---

## 2. Detaillierte Zuordnung jeder Komponente

| Komponente | Datei / Pfad | Aufgabe & Funktion im Gesamtsystem | Fehlersymptom bei Fehlen / Fehler |
| :--- | :--- | :--- | :--- |
| **RSX Vulkan Engine** | `rpcs3.exe`<br>*(aus `tracked-changes.patch`)* | Modifizierter Vulkan-Renderer von RPCS3. Fängt interne RSX-Renderziele (`0xc0010000` etc.) ab, skaliert Geometrie und Partikel separat, entfernt den PS3-Sicherheitsrahmen und fusioniert Bloom. | Spiel läuft in matschigem 720p; Renderziele werden unskaliert gezeichnet; schwarzer Rand. |
| **Compute-Flags** | `tm4k-*.flag`<br>*(5 Dateien im EXE-Ordner)* | Schalten die spezifischen TM4K-Compute-Shader in `VKPresent.cpp` scharf:<br>• `tm4k-enable.flag`<br>• `tm4k-lit-c-enable.flag`<br>• `tm4k-lit01-enable.flag`<br>• `tm4k-micro-e-enable.flag`<br>• `tm4k-packed-alpha-enable.flag` | 4K-Shaderpfad bleibt inaktiv; Spiel startet wie ungemoddetes RPCS3. |
| **SPU PostFX Bypass** | `patches/BCUS98106_patch.yml`<br>& `patch.yml` | Enthält den 11x NOP-Bypass an Offset `0x0066aaf4`. Schaltet die veraltete Sony 720p SPU-PostFX-Pipeline (Farbkorrektur, Downscaling) komplett ab. | **Türkise/blaue Flammen** auf Sweet Tooth!<br>**Giftgrüner / violetter Streifen** am unteren Bildschirmrand aus rohem SPU-Pufferspeicher! |
| **Patch-Aktivierung** | `config/patch_config.yml` | Schaltet `"PostFX Sharp Fast": true`, `"Extend Draw Distance": true` und `"Disable SPU MLAA": true` scharf. Hält Model LOD zwingend auf `false`. | Patches werden trotz Vorhandensein nicht geladen; MLAA zeichnet Kanten unscharf. |
| **HierLOD-Sperre** | `config/patch_config.yml`<br>*(Model LOD Patches)* | Verhindert die Multiplikation des HierLOD-Faktors an `0x0010e83c`. MUSS auf `false` stehen, damit HierLOD bei nativem 1.0f verbleibt. | **Schwarzer Boden in Times Square!** Straßen und Brückensegmente verschwinden während der Fahrt. |
| **4K & Textur-Config** | `config/custom_configs/`<br>`config_BCUS98106.yml` | • `Resolution Scale: 300` (4K UHD)<br>• `Asynchronous Texture Streaming: false`<br>• `Minimum Scalable Dimension: 256`<br>• `Vulkan Adapter: ""` (Universal GPU) | Bei `Async Streaming: true`: **Straßen flackern / scheinen durch**, Texturen ploppen auf.<br>Bei `Min Scalable < 256`: UI/2D-Masken zerrissen. |
| **Controller-Anbindung** | `config/input_configs/`<br>*(Individuell)* | Respektiert vorhandene RPCS3-Profile! Auf frischen PCs wählt der Nutzer oder die KI das Gamepad im RPCS3-Menü unter `Pads` (XInput für Xbox/PC, DualSense/DualShock für PS, SDL). Niemals ungefragt statische Konfigurationen erzwingen! | Falsche Vendor/Product-IDs führen zu Controller-Entkoppelung in Windows; Überschreiben löscht bestehende Lenkrad-/Pad-Setups. |
| **Asset Weitsicht-Mod** | `dev_hdd0/game/BCUS98106/`<br>`USRDIR/levels/times.psarc` | Modifiziert NGP-HierLOD-Knoten im Level-Archiv von 50m–300m auf 5.000m. Modifiziert `times.ltn` und `suburbs.ltn` (`Distance Blur Enable: 0`). | Details am Horizont (Zäune, Schilder, Ampeln, Trümmer) ploppen erst kurz vor dem Wagen auf; trüber Entfernungsnebel. |
| **TM4K Launcher** | `Start-TwistedMetal-4K.ps1` | Setzt 7 systemkritische Umgebungsvariablen (`TM4K_SCENE_2160=1`, `TM4K_MASTER_DENSITY=7`, `TM4K_PARTICLE_RESTORE=1`, etc.) und startet RPCS3. | Shader berechnen Standard-Partikeldichte; HDR-Farben können unausgewogen wirken. |
| **System-Validierer** | `Verify-TM4K-Setup.ps1` | Führt eine automatisierte 8-Punkte-Inspektion der gesamten Installation durch und meldet sofort eventuelle Konfigurationsfehler mit exakter Behebung. | Manuelle Fehlersuche entfällt vollständig. |

---

## 3. Hardware-Kompatibilitäts-Matrix (Universalität)

Das Gesamtsystem wurde so ausgelegt, dass es auf **jeder modernen PC-Hardwarekonfiguration** ohne manuelle Anpassung sofort lauffähig ist:

### 3.1 Grafikkarten (GPU)

| GPU-Hersteller / Serie | Konfiguration in `config_BCUS98106.yml` | Verhalten / Besonderheiten |
| :--- | :--- | :--- |
| **NVIDIA GeForce**<br>*(RTX 20, 30, 40, 50 Serie)* | `Adapter: ""` | RPCS3 wählt über den Vulkan-Backend-Scorer automatisch die stärkste GeForce-GPU. Re-BAR (`Use Re-BAR for GPU uploads: true`) wird von RTX 30/40/50 voll ausgenutzt. |
| **AMD Radeon**<br>*(RX 6000, 7000, 8000 Serie)* | `Adapter: ""` | Native AMD Vulkan-Treiber-Nutzung inkl. Smart Access Memory (SAM). |
| **Intel Arc**<br>*(A750, A770, Battlemage)* | `Adapter: ""` | Vollständige Vulkan 1.3 Unterstützung; automatische Zuweisung. |

> [!TIP]
> **Warum `Adapter: ""` (leerer String)?**  
> Wenn ein fester GPU-Name (z. B. `AMD Radeon RX 7900 XT`) eingetragen ist, erzeugt RPCS3 auf einem PC mit NVIDIA-Grafikkarte einen Fallback-Fehler. Ein leerer String `""` weist RPCS3 an, die schnellste dedizierte Grafikkarte im System vollautomatisch zu wählen.

### 3.2 Prozessoren (CPU)

| CPU-Familie | Einstellungen in `config_BCUS98106.yml` | Funktion & Optimierung |
| :--- | :--- | :--- |
| **AMD Ryzen**<br>*(Zen 2, 3, 4, 5 / X3D-CPUs)* | `Preferred SPU Threads: 0`<br>`SPU Decoder: Recompiler (LLVM)` | Automatische Thread-Verteilung über CCX-Grenzen hinweg. Nutzt auf Zen 4/5 AVX-512 zur SPU-Beschleunigung. |
| **Intel Core**<br>*(10. bis 14. Gen / Core Ultra)* | `Preferred SPU Threads: 0`<br>`SPU Decoder: Recompiler (LLVM)` | RPCS3 weist SPU-Recompiler-Threads automatisch den Performance-Cores (P-Cores) zu. |

---

## 4. Fehlersymptom-Atlas (Diagnose-Leitfaden für KI & Mensch)

Tritt beim Testen ein unerwartetes Verhalten auf, zeigt diese Tabelle die exakte Ursache und die 1-Klick-Lösung:

```
+----------------------------------------------------------------------------------------------------+
| SYMPTOM                            | URSACHE                           | SOFORTIGE LÖSUNG          |
+------------------------------------+-----------------------------------+---------------------------+
| Flammen von Sweet Tooth sind       | BCUS98106_patch.yml fehlt im      | Kopiere 03_QUELLCODE/     |
| türkis / cyan; giftgrüner Balken   | Ordner bin\patches\ (Sony SPU     | patches\* nach            |
| am unteren Bildschirmrand.         | PostFX läuft parallel mit).       | bin\patches\              |
+------------------------------------+-----------------------------------+---------------------------+
| Schwarzer Asphalt / Boden fehlt    | "Model LOD Distance +100%" steht  | In config\patch_config.yml|
| in Times Square bei hoher Fahrt.   | in patch_config.yml auf TRUE.     | Model LOD auf FALSE       |
|                                    | (HierLOD desynchronisiert Stream) | setzen!                   |
+------------------------------------+-----------------------------------+---------------------------+
| Straßen flackern / scheinen durch; | "Asynchronous Texture Streaming"  | In config_BCUS98106.yml   |
| Texturen blitzen kurz auf.         | steht auf TRUE.                   | Asynchronous Texture      |
|                                    |                                   | Streaming auf FALSE!      |
+------------------------------------+-----------------------------------+---------------------------+
| Weiße Fragmente im Gameplay;       | KALTSTART / LEERER SHADER-CACHE:  | Nach ca. 3–5 Minuten      |
| Straße blendet kurz verzögert ein; | Cache hat nur ~800 Dateien statt  | Fahren (1 Match) sind die |
| weißes Aufblitzen bei Explosionen. | 8.500-9.000. 'Async Recompiler'   | Pipelines kompiliert.     |
|                                    | überspringt Geometrie während des | Ab Start 2 ist das Bild   |
|                                    | ersten Hintergrund-Kompilierens.  | dauerhaft fehlerfrei!     |
+------------------------------------+-----------------------------------+---------------------------+
| Controller entkoppelt sich dauernd | Phantom-Devnodes (CM_PROB_PHANTOM)| 1. RPCS3 stets regulär beenden    |
| oder schaltet sich nach 3s aus     | im Kernel durch unvollständigen   |    (kein --no-gui verwenden).     |
| (z. B. Xbox Wireless Adapter).     | Shutdown / --no-gui. Radio-       | 2. Ohne Reboot: In PowerShell     |
|                                    | Handshake schlägt fehl -> Sleep.  |    'Restart-Service XboxGipSvc,   |
|                                    |                                   |    GameInputRedistService' ausf.  |
+------------------------------------+-----------------------------------+---------------------------+
| Controller reagiert nicht im Spiel | Controller ist in Windows da, aber| In RPCS3 unter 'Pads' ->  |
| (fährt nicht, kein Menü).          | in RPCS3 nicht Spieler 1 zugewiesen| Player 1 -> Handler:      |
|                                    | (RPCS3 nutzt zunächst Keyboard).  | XInput -> Device wählen!  |
+------------------------------------+-----------------------------------+---------------------------+
| Weißes / helles Fenster für 10s    | Normales Verhalten bei Start 1:   | Nichts tun! Ab Start 2    |
| beim allerersten Start.            | LLVM kompiliert PPU-Cache und     | startet das Spiel sofort  |
|                                    | initialisiert Savegame-Container. | ohne Verzögerung.         |
+------------------------------------+-----------------------------------+---------------------------+
| Bild bleibt in 720p matschig;      | Die 5 .flag-Dateien fehlen im     | Kopiere 03_QUELLCODE\     |
| keine 4K-Schärfe.                  | Hauptordner von rpcs3.exe.        | flags\*.flag direkt neben |
|                                    |                                   | rpcs3.exe!                |
+------------------------------------+-----------------------------------+---------------------------+
```

### 4.1 Der Shader-Cache & Kaltstart (Die drei verschiedenen Lade-Arten)

Aus den gemeinsamen Messungen von Gemini und Codex (Test 4) ergibt sich eine präzise Differenzierung der Ladevorgänge:

1. **PPU-Vorkompilierung (~7.000 Basismodule):**  
   Beim Erststart übersetzt RPCS3 den PowerPC-Code des Spiels über LLVM in Maschinencode. Das dauert nur beim ersten Mal ca. 10–20 Sekunden und ist ab dem zweiten Start sofort geladen.
2. **Spiel-Shader-Cache (Vulkan-Grafikpipelines):**  
   Während des Spielens erzeugt die GPU für jede Explosion, jeden Funken und jede Scheinwerferreflexion einen optimierten Vulkan-Shader. Auf einem eingespielten System wächst dieser Cache auf **8.500 bis 9.000 Dateien** an.  
   *Auf einem frischen PC ist dieser Cache anfangs leer.* Im empfohlenen Standardmodus `Async Recompiler (multi-threaded)` überspringt die GPU Geometrie für 1–2 Frames, während der Shader im Hintergrundthread kompiliert wird. Sobald ein Level einmal durchfahren wurde (ca. 3–5 Minuten), sind alle Shader auf der Festplatte gespeichert – ab Start 2 läuft alles absolut sauber.
3. **Shader-Interpreter-Vorberechnung (6.630 Varianten – HINWEIS):**  
   Der Interpreter-Modus (`Async Recompiler with Shader Interpreter`) berechnet beim Start 6.630 generische Vulkan-Interpreter-Pipelines vor. Dies führt zu einer spürbar längeren Ladezeit beim ersten Booten und ist für den regulären 4K-Genuss **nicht** erforderlich. Für den Standard-Neubau gilt daher verbindlich:
   ```yaml
   Video:
     Shader Mode: Async Recompiler (multi-threaded)
   ```

---

## 5. Automatisierte Endkontrolle

Um vor dem Start sicherzustellen, dass alle fünf Säulen intakt sind, wird das Validierungsskript ausgeführt:

```powershell
powershell -ExecutionPolicy Bypass -File "Verify-TM4K-Setup.ps1"
```

Das Skript scannt CPU, GPU, Binärgrößen, Patch-Offsets (`0x0066aaf4`), Konfigurationseinträge und XInput-Profile. Erst wenn `8 / 8 PRÜFUNGEN BESTANDEN` angezeigt wird, ist die Installation abgenommen und startbereit.

---

## 6. Auflösungs-Flexibilität: 4K (300%), 1440p (200%) und 1080p (150%)

> [!NOTE]
> **Release-Fokus v1.0:**  
> Der vorliegende v1.0-Release liefert das zu 100% verifizierte **4K UHD Master-System** (`Resolution Scale: 300`). Native 1080p- und 1440p-Spezialprofile (für stromsparende Handhelds wie Steam Deck / ROG Ally) befinden sich in aktiver Erprobung und werden über zukünftige Roadmap-Updates nachgereicht. Auf Desktop-PCs wird für 1080p/1440p Monitore die 4K-Renderauflösung (SSAA) empfohlen!

Spieler haben unterschiedliche Monitore und unterschiedlich starke Grafikkarten. Das System unterstützt alle drei Hauptauflösungen vollautomatisch.

### 6.1 Der wichtigste Grundsatz: Monitor-Auflösung vs. Render-Auflösung (Super-Sampling / SSAA)
Viele Nutzer nehmen an: *"Ich habe einen 1080p- oder 1440p-Monitor, also muss ich im Emulator auch 1080p oder 1440p einstellen."*  
**Das ist ein weit verbreiteter Irrtum!**

* `Resolution Scale` in RPCS3 steuert die **interne 3D-Berechnung**.
* Wenn `Resolution Scale: 300` (4K) gewählt wird, berechnet die Grafikkarte Geometrie und Shader in 3840×2160.
* Gibt man dieses 4K-Bild auf einem 1080p- oder 1440p-Monitor aus, rechnet RPCS3 / die GPU das Bild hardwarebeschleunigt auf die Bildschirmauflösung herunter (**Super-Sampling Anti-Aliasing / SSAA**).
* **Das Ergebnis auf 1080p/1440p-Monitoren:** Ein unvergleichlich ruhiges, knackscharfes Bild ohne jegliches Treppenflimmern an Zäunen, Strommasten oder Kanten – mit vollem Erhalt aller TM4K-Shader-Effekte!

### 6.2 Übersicht der Auflösungsstufen

| Resolution Scale | Interne Renderauflösung | GPU-Anforderung | Empfohlen für | Verhalten im TM4K-Code |
| :---: | :---: | :---: | :---: | :--- |
| **300%** *(Standard)* | **3840×2160 (4K UHD)** | Ab RTX 2070 / RX 6700 XT | **Alle Monitore (1080p, 1440p, 4K)** für maximale Bildqualität via SSAA | Volle TM4K-Compute-Shader-Fusion aktiv; Trauerrand-Crop (1202×672 -> Vollbild) aktiv. |
| **200%** | **2560×1440 (WQHD)** | Ab GTX 1070 / RX 5600 XT | Mittelklasse-PCs / WQHD-Monitore | Flüssige 60 FPS; TM4K-Shader schalten auf Standard-RPCS3 zurück. |
| **150%** | **1920×1080 (Full HD)** | Ab GTX 1060 / RX 580 / Laptops | Schwächere Hardware / Handhelds (Performance-Modus) | Maximale Framerate-Stabilität; Standard-RPCS3-Rendering. |

### 6.3 So wechselt man die Auflösung
In der Datei `config/custom_configs/config_BCUS98106.yml` (oder in RPCS3 via Rechtsklick auf das Spiel -> *Change Custom Configuration* -> Reiter *Video*):

```yaml
Video:
  # 300 = 4K (Standard), 200 = 1440p, 150 = 1080p
  Resolution Scale: 150
```
Das Validierungsskript `Verify-TM4K-Setup.ps1` erkennt `150%`, `200%` und `300%` automatisch als gültige Konfigurationen an.

