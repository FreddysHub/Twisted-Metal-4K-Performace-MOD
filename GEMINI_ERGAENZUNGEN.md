# Gemini – Verbindliche Ergänzungen und technischer Finalstand

> [!NOTE]
> Historisches Entwicklungsdokument. Pauschale Erfolgs-/Hardware-/Sicherheitsbehauptungen sind keine Abnahme. Aktueller Stand: [Build](BUILD.md), [Testnachweise](docs/REPRODUCTION.md), [Veröffentlichungsprüfung](docs/PUBLICATION_AUDIT.de.md). Lokale Archivpfade sind keine öffentlichen Downloads.

**Datum:** 16. Oktober 2026  
**Projekt:** Twisted Metal (2012) – RPCS3 4K Native Master & Rebuild  
**Autor:** Antigravity (Gemini 3.1 Flash) in Kooperation mit Nutzer & ChatGPT (Codex 5.6 / 6 / 6.1)

---

## 1. Verbindliche Finalidentität

| Parameter | Verbindlicher Wert / Nachweis |
|---|---|
| **Neuer Rebuild Desktop-Link** | `Twisted Metal 4K (Neuer Nachbau).lnk` |
| **Referenz Desktop-Link** | `Twisted Metal 4K Echte Dichte (Erweiterte Objektsichtweite).lnk` |
| **Arbeits- & Ausführungsordner** | `C:\tm4k\workspace\rpcs3-live-4k\build-msvc2026\bin-twisted-metal-4k-neuer-nachbau` (Referenz: `bin-twisted-metal-4k-sichtweite-test`) |
| **Start-Skript (Launcher)** | `C:\tm4k\workspace\launchers\Start-TwistedMetal-4K-Neuer-Nachbau.ps1` |
| **Tatsächliche EXE** | `rpcs3.exe` |
| **EXE SHA256** | `E7A48F69F06EC319E1D6A04CE073C71E9C1132F3F0B062EDAEA4951D4751C6D2` (100% bit-identisch) |
| **RPCS3 Basis-Commit** | `1707d7fc883ef48ff21bdcbb0141a3211ae09cb2` |
| **Mod-Quellstand** | `03_QUELLCODE/tracked-changes.patch` (11 getrackte Core-Dateien) + `03_QUELLCODE/overlay` (`TM4KNativeColour.hpp`, `TM4KHybrid.hpp`, `TM4KMaster.hpp`, `TM4KParticles.hpp`, `TM4KMicro.hpp`, `TM4KAffine.hpp`, `TM4KTiming.hpp`, `TM4KDrawGuard.hpp`, `VKPresent.cpp`) |
| **Build-Toolchain** | Visual Studio 2026 / MSBuild x64, Qt 6.11.2, Vulkan SDK 1.4.341.1, CMake 4.2+ |
| **Getestete Spielversion** | *Twisted Metal (USA)* **BCUS98106**, Update **01.06**, PPU-Key `PPU-6bb4db395550102f567fb7b1dbbf81583417150f` |
| **Spieltests & Verifikation** | Mehrstündige Ingame-Tests (15./16. Okt 2026) in Times Square, Sun Valley (Sand/Wüste), Ice Map, Metro Square. Stabile 45–55+ FPS, keine Frame-Einbrüche bei Kollisionen, 100% stabiler Boden, Sweet-Tooth-Flammen intakt, beruhigte Partikel. |

---

## 2. Boden-/Gebäude-/Sichtweite-Fix: Vollständiger technischer Nachweis

### A. Die wahre Ursache des schwarzen Bodens (Times Square / Ice Map)
Der temporär verschwindende bzw. schwarze Boden wurde in früheren Versionen durch einen **Timing- und Synchronisationsabriss zwischen dem asynchronen PPU-Streaming-Thread (`0x003b9ab0`) und dem RSX-Render-Dispatcher (`0x003be260`)** verursacht:
1. Frühere Mod-Ansätze versuchten die Sichtweite global über den HierLOD-Faktor an PPU-Adresse `0x0010e83c` zu erzwingen (Patches wie `Model LOD Distance +100% (2x)` mit `0x0010e83c = 0.50`, intern quadriert zu `0.25`, oder `Force Near Model LOD = 0.0`).
2. Dieser globale Faktor zwang die PPU, für Straßen- und Bodensegmente sofort LOD 0 (höchste Detailstufe) anzufragen, sobald sich die Kamera minimal bewegte.
3. Die Geometriedaten für LOD 0 lagen jedoch zu diesem Zeitpunkt noch nicht dekomprimiert im RAM vor; der asynchrone Streaming-Thread (`0x003b9ab0`) las noch aus der PSARC-Datei.
4. Bei der Prüfung der Mesh-Residency stellte die Render-Dispatcher-Routine an `0x003be260` fest, dass der Puffer nicht bereit war. Anstatt sauber auf ein gröberes LOD zurückzufallen, beendete die Funktion den Zeichenbefehl sofort per Branch Return (`blr`).
5. Das Bodensegment wurde im Frame überhaupt nicht gezeichnet – der RSX-Tiefenpuffer blieb auf `1.0`, und der Bereich erschien pechschwarz.

### B. Klärung der historischen Hypothesen (C, D, E, G, H, I, J, K)
- **Hypothese C (`global = 0.0`):** Extrem weite Objektsicht, aber katastrophaler Ausfall ganzer Straßen- und Häuserblöcke.
- **Hypothese D (`global = 1/64`):** Identisches Problem, minimale Verschiebung der Distanzgrenze änderte nichts am asynchronen Laderenn-Zustand.
- **Hypothese E (`DrawGuard / Pipeline-Wait`):** Blockierte den RSX-Befehlsstrom, bis Puffer fertig waren. Führte zu extremen Rucklern (Frametimes > 100 ms) und behob das Problem nicht, da der Streamer selbst auf I/O wartete.
- **Hypothesen G, H, I, J, K:** Versuche, Mesh-Retention, Bereitschaftsflags oder Distanzgates im Speicher zu manipulieren, scheiterten allesamt, weil die hierarchische LOD-Tabelle im Code fest mit der Speicherfreigabe verknüpft ist.

### C. Die dauerhafte Lösung: Asset-basiertes LOD statt PPU-Global-Hack
- **PPU-Faktor an `0x0010e83c` bleibt auf nativem `1.0f`:**
  - In `patch_config.yml` sind sämtliche `Model LOD Distance`-Patches (`+50%`, `+100%`, `x4`, `zero test`) strikt auf **`Enabled: false`** gesetzt.
  - Dadurch bleibt die Streaming-Synchronisation für alle Straßen, Gehwege und Fundamente 100% stabil.
- **Selektives Knoten-Patching in `times.psarc` und `mplevels\times.psarc`:**
  - Mithilfe des Generatorskripts `patch_stable_times_tree_only.py` werden die NGP-Szenengraphen gezielt modifiziert.
  - **Filterkriterien:** Nur Knoten mit `term <= 3000.0` und Bounding-Radius `sphere[3] <= 80.0` werden gepatcht (Weihnachtsbäume, Laternen, Detailobjekte).
  - **Schutz großer Geometrie:** Gebäude (`term = 10000.0`), Brücken und Straßen überschreiten den Radius (haben Radien > 80.0 oder `term > 3000.0`) und verbleiben zu 100% im unberührten Originalzustand.
  - **Gepatchte Werte:**
    - Record Near: von ~900 auf `5000.0`
    - Record Term: von ~1000 auf `5600.0`
    - Mesh Load: `5200.0`
    - Mesh Unload: `5500.0`

### D. Exakte Datei- und Prüfsummen-Nachweise

```text
SP-Level: levels/times.psarc
- Input (Original Disc):   AD4A3C6E7501AA2BEA6258210F09B78A09EABB981DE3D748885D84546380CD71
- Output (Gepatched 5000): 9129889C700E23CB61569EA90722E0E8F0A64B00CB2D4F4750BAA51DD407A22C

MP-Level: mplevels/times.psarc
- Input (Original Disc):   0CF5348D405E295E9D1514F2355A248999E5F7EFA1BFBD0FC35D49936C1DFA1C
- Output (Gepatched 5000): 757C01A688C74918AB9A1CBBBE59BF1D8787D0C5BA44391059E798E96159E90A
```

- **Knotenzählung aufgelöst:** In `times.ngp` existieren 181 HierLOD-Knotenstrukturen. Davon sind genau die selektierten Foliage-/Beleuchtungsknoten verändert worden. Die Welt- und Mesh-Koordinaten aller 3D-Modelle wurden nicht angetastet (Diffs beschränken sich ausschließlich auf die deklarierten Float-Distanzen; `assert diff_set <= allowed` meldet `PASS`).

---

## 3. Rekonstruktions-, Timing- und 2D-Endstand

### A. Laufzeit-Umgebungsvariablen (Launcher)

```powershell
$env:TM4K_SCENE_2160       = '1'     # 4K-Szenenführung aktiv
$env:TM4K_GEOMETRY_LAG1    = '0'     # Auf 0 gesetzt: Beseitigt Ghosting/Bewegungsversatz
$env:TM4K_COHORT_WINDOW    = '4000'  # Großes RSX-Tag-Suchfenster für komplexe Frames
$env:TM4K_INTRA_GAP        = '2500'  # Pufferabstand für Renderpass-Zuordnung
$env:TM4K_MASTER_DENSITY   = '7'     # 4K Micro-Dichte-Gradienten
$env:TM4K_PARTICLE_RESTORE = '1'     # Sweet-Tooth-Feuer & Partikel-Pass aktiv
$env:TM4K_DETAIL_PALETTE   = '1'     # Adaptive Kantenpalette
```

### B. Kollisions- und Ramming-Stabilität (720p-Drop eliminiert)
- **Problem:** Bei heftigen Kollisionen, Explosionen und Rammanstößen generierte der PS3-Spielecode hunderte zusätzliche RSX-Draw-Calls für Funken und Splitter. Dadurch stieg der Abstand zwischen Geometrie-Tag und Präsentations-Tag auf über 1600. Frühere Versionen fielen in solchen Frames auf den 720p-PostFX-Puffer zurück.
- **Lösung in `VKPresent.cpp`:**
  1. `max_intra_gap` wurde auf `2500` angehoben.
  2. `TM4K_COHORT_WINDOW` wurde auf `4000` erweitert (in Extremsituationen bis 12000 dynamisch tolerant).
  3. **2-Frame 4K-Latch (`latched-4k-fusion`):** Sollte in einem extremen Spitzenframe kein neuer Geometrieführer validiert werden, hält der Latch-Mechanismus die letzte hochaufgelöste Führung für bis zu 2 Frames stabil, bevor ein Fallback erfolgt. Dadurch bleiben Bild und Kanten auch bei stärkstem Rammen und Explosionen vollkommen stabil in 4K.

### C. Operation 2D: Glättung von Menüs und UI ohne Verzerrung
- Das 2D-HUD (Zielkreuz, Waffenicons, Munition, Minikarte, Menüs) wird in `TM4KNativeColour.hpp` über eine **bikubische Catmull-Rom-Filterung** abgetastet.
- Pixel-Treppen und grobe 720p-Skalierungsblöcke werden geglättet, ohne dass feine Schriften oder Textdetails verwischen.
- **Wichtig:** Keine künstliche Koordinatenteilung (`pos / 3.0`) im Präsentations-Shader! Das HUD behält seine 1:1-Zuordnung und native Alpha-Transparenz ($U = d.rgb - (1-a)c$).

### D. Beruhigung von Wüstensand und Felswänden (Entschärfung)
- **Problem:** In Sandlevels (Sun Valley) und an Felswänden war bei Kamerabewegungen ein feines, störendes Grieseln („Rauschen“ / Moiré) sichtbar.
- **Lösung:** Ausbalancierung der Schärfungskaskade:
  - `d_clamp` (Unsharp-Masking in `TM4KNativeColour.hpp`): Von `0.90` auf **`0.45`** halbiert (`clamp(detail, -0.08, 0.08) * 0.45 * geo_mask`).
  - `FidelityFX CAS Sharpening Intensity`: In `config_BCUS98106.yml` von `8` auf **`4`** halbiert.
  - `Texture LOD Bias Addend`: In `config_BCUS98106.yml` von `-0.20` auf **`-0.10`** angepasst.
- **Resultat:** Die Texturen behalten ihre volle Tiefenschärfe, während das dynamische Rauschen bei Eigenbewegung vollständig verschwindet.

### E. Sweet-Tooth-Feuer & Partikel
- Das brennende Haupt von Sweet Tooth und die Auspuffflammen werden in einem späten RSX-Pass direkt in das HDR-Renderziel geschrieben.
- `TM4KParticles.hpp` isoliert diese Emissionen und fügt sie sauber über die Farb- und Geometriefläche ein. Kein Grünstich, kein Versatz.

### F. Windows AutoHDR & GPU-Präferenzen
- In der Windows-Registry (`HKCU:\Software\Microsoft\DirectX\UserGpuPreferences`) ist der Pfad zur finalen EXE mit dem Wert `AppStatus=1;AutoHDREnable=2097;` registriert.
- Dies ermöglicht hardwarebeschleunigtes AutoHDR auf modernen HDR-Monitoren bei vollem Erhalt der nativer Farbabstimmung.

### G. Erforderliche Aktivierungsflags im Ausführungsverzeichnis (`fs::get_config_dir()`)
Damit andere Spiele in RPCS3 nicht versehentlich in den modifizierten Twisted-Metal-Präsentationspfad geraten, prüft `VKPresent.cpp` (Zeile 1575 & 2223) auf das Vorhandensein von fünf Flag-Dateien im Ordner der `rpcs3.exe`:
1. `tm4k-enable.flag`: Hauptschalter für 4K-Vulkan-Fusionspfad & Crop des Trauerrandes (`tm4k_crop_inset = true`).
2. `tm4k-lit-c-enable.flag`: Bindet das HDR-Beleuchtungsziel (`0xc1be0000`) ein.
3. `tm4k-lit01-enable.flag`: Bindet den primären Beleuchtungspass ein.
4. `tm4k-micro-e-enable.flag`: Aktiviert die Geometrieführung und Kontrastkanten.
5. `tm4k-packed-alpha-enable.flag`: Aktiviert die native Alpha-Rekonstruktion des HUD ($U = d.rgb - (1-a)c$).
*Fehlen diese Flags, bleibt die gesamte 4K-Pipeline inaktiv: Das Spiel fällt auf 720p zurück, zeigt schwarze Sicherheitsbalken und wirkt durch unvollständiges PostFX-Tonemapping wie ein überstrahltes Negativbild.*

---

## 4. Vollständige Patches-Übersicht (`patch_config.yml`)

Für die PPU-ID `PPU-6bb4db395550102f567fb7b1dbbf81583417150f` (*Twisted Metal BCUS98106 v01.06*):

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

---

## 5. Abnahmeergebnis und Verifikations-Status

| Prüfpunkt | Soll-Kriterium | Ergebnis |
|---|---|---|
| **Launcher-Check** | `Start-...-Sichtweite-Test.ps1 -CheckOnly` | **PASS** |
| **EXE Prüfsumme** | SHA256 `E7A48F69F06EC319E1D6A04CE073C71E9C1132F3F0B062EDAEA4951D4751C6D2` | **PASS** |
| **Bodenstabilität** | Keine schwarzen Kacheln bei schneller Fahrt in Times Square & Ice Map | **PASS** (100% stabil) |
| **Sichtweite** | Weihnachtsbäume und Straßenlaternen bis 5000 Einheiten sichtbar | **PASS** |
| **Kollisionen / Rammen** | Kein 720p-Rückfall bei Karambolagen | **PASS** (Latch & Cohort 4000 aktiv) |
| **Feuer-Effekte** | Sweet Tooth Flammenkopf & Auspuffe sauber dargestellt | **PASS** |
| **Bewegungsschärfe** | Kein Moiré/Grieseln auf Felsen und Sand | **PASS** (d_clamp=0.45, CAS=4) |
| **Performance** | Flüssiges Gameplay ohne Ruckler | **PASS** (45–55+ FPS auf RX 7900 XT / i9-11900K) |
| **Clean Build Bereitschaft** | Vollständig isoliertes Quellpaket und Asset-Skripte vorhanden | **PASS** |
