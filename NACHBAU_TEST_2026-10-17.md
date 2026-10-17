# Tatsächlicher Nachbau-Test & Fehleranalyse – 17. Oktober 2026

> [!NOTE]
> Historisches Entwicklungsdokument. Pauschale Erfolgs-/Hardware-/Sicherheitsbehauptungen sind keine Abnahme. Aktueller Stand: [Build](BUILD.md), [Testnachweise](docs/REPRODUCTION.md), [Veröffentlichungsprüfung](docs/PUBLICATION_AUDIT.de.md). Lokale Archivpfade sind keine öffentlichen Downloads.

## 1. Übersicht & Status

Ein neuer RPCS3-Build wurde unabhängig kompiliert und im Zielverzeichnis installiert:
`C:\Users\box3\Documents\ChatGPT\ps3\rpcs3-live-4k\build-msvc2026\bin-twisted-metal-4k-anleitungs-nachbau`

Desktop-Verknüpfung: **Twisted Metal 4K - Anleitungs-Nachbau**  
Zusätzlich: **Twisted Metal 4K - Anleitungs-Nachbau - RenderDoc**

---

## 2. Gemeldete Symptome beim ersten Spieltest

Nach dem ersten Bauversuch durch ChatGPT/Codex traten im praktischen Spieltest drei spezifische Bildfehler auf:
1. **Negativbild / Invertierte, überstrahlte Farben:** Beim Spielstart und kurz vor Beginn des Matches wirkten die Ladebildschirme und Übergänge wie ein farblich umgedrehtes, grelles Negativbild.
2. **Kurzes weißes Aufblitzen (Flash-Frame):** Mitten im Match trat bei Lastspitzen oder Explosionen für einen einzelnen Frame ein blendend weißer Bildschirm auf.
3. **Verspätetes Einblenden der Straße:** Beim Laden des Levels war die Fahrbahn für einen kurzen Moment unsichtbar und blendete erst mit Verzögerung ein.
4. **Asset-Patcher:** Das ursprüngliche Skript besaß fest verdrahtete Pfade zu alten Testverzeichnissen und ließ die Distance-Blur-Einstellung der Level-Konfigurationen (`.ltn`) aus.

---

## 3. Technische Ursachenanalyse (Root Causes)

### A. Negativbild & beschnittene 2D-Menüs
- **Mechanismus:** Twisted Metal nutzt zwei getrennte Präsentationsadressen:
  - `0xc0010000`: 2D-Hauptmenüs, Video-Intros und Ladebildschirme (native 720p/1080p UI-Ebene).
  - `0xc0556000`: Die eigentliche 3D-Spielszene (wird auf 3840x2160 hochskaliert; enthält 1202x672 Bildinhalt im Sicherheitsrahmen).
- **Fehler:** Der PPU-Patch `PostFX Sharp Fast` schaltet das spielinterne Tonemapping ab. Zuvor war `VKPresent.cpp` so aufgebaut, dass bei *jeder* Präsentationsadresse der Sicherheitsrand-Zuschnitt (`tm4k_crop_inset = true`) aktiviert und der 4K-Fusionspfad versucht wurde. Traf dies auf `0xc0010000` (Ladebildschirm) zu, wurde das 2D-Bild beschnitten und mit veralteten linearen 3D-Puffern verschmolzen. Das Resultat war ein ungetonemapptes, überstrahltes "Negativbild".

### B. Weißes Aufblitzen mitten im Match
- **Mechanismus:** Wenn während heftiger Explosionen oder Framedrops kein frisches Fusionspaket im Ringpuffer vorlag (`packet == nullptr`), griff die Latch-Logik.
- **Fehler:** Das Latching war hart auf `< 2` Frames (ca. 33 ms) begrenzt. Dauerten Spitzen länger als 2 Frames, fiel der Präsentationscode auf den un-tonemappten linearen FP16-Rohpuffer zurück – was zu einem blendend weißen Flash-Frame führte.

### C. Unsichtbare Straße beim Start
- **Fehler:** Der Modus `Shader Mode: Async Recompiler (multi-threaded)` ohne Shader Interpreter verwirft RSX-Drawcalls für Geometrie, deren Vulkan-Pipeline noch im Hintergrund kompiliert wird. Straßen- und Landschaftsobjekte wurden daher erst gezeichnet, als der Hintergrund-Compiler fertig war.

---

## 4. Erkenntnisse, Experimente & endgültige Bereinigung

1. **Behebung des Feuer- & Schrift-Flackerns (30 Hz Stroboskop-Effekt):**
   - **Das Problem:** Twisted Metal wechselt in schnellem Rhythmus zwischen `0xc0556000` (3D-Spielszene) und `0xc0010000` (2D-Overlay/HUD/Menü). Wenn die 4K-Fusion künstlich auf `0xc0556000` begrenzt wird, zeigt Frame A die 3D-Szene (Feuer sichtbar, aber keine Schrift) und Frame B das 2D-Overlay (Schrift sichtbar, aber kein 3D-Feuer). Bei 60 FPS entsteht ein 30 Hz Stroboskop: Das Feuer auf Sweet Tooths Kopf und die Schriften flackern extrem.
   - **Die saubere Lösung:** Beide Framebuffer laufen regulär durch `if (packet)`. Der TM4K-Merge-Shader (`mix(fused, screen.rgb, overlay)`) blendet das 2D-Interface nativ auf die 4K-HDR-Szene ein. Beide Ebenen bleiben auf jedem Frame deckungsgleich und stabil sichtbar – **null Flackern**.
   - **Gleichzeitiger Schutz vor Doppelbildern:** `tm4k_crop_inset = true` bleibt global und einheitlich aktiv! Sowohl 2D-Menüs als auch die 3D-Spielszene nutzen denselben Zuschnitt `(39*scale, 24*scale)` bis `(1241*scale, 696*scale)`. Dadurch gibt es weder Bildverdopplung noch Versatz.

2. **Beseitigung des weißen Aufblitzens (White Flash):**
   - **Das Problem:** Bei heftigen Explosionen, Kollisionen oder Lastspitzen bricht für wenige Frames die Zuleitung frischer 4K-Pakete ab (`packet == nullptr`). Der alte Latch war auf nur 2 Frames (< 33 ms) begrenzt. Dauerten Ruckler länger, fiel RPCS3 auf den un-tonemappten linearen FP16-Rohpuffer zurück – was zu einem blendend weißen Flash führte.
   - **Die Lösung:** Der Latch wurde gehärtet auf `s_fused_latch_frames < 30` (~500 ms). Während GPU-Spitzen hält der Emulator nahtlos das letzte gültige 4K-Frame, statt auf den weißen Rohpuffer abzustürzen.

3. **Vulkan Shader-Modus & Ladezeiten:**
   - Mit vorhandenem Cache (über 4.100 Dateien in `cache/BCUS98106`) und `Shader Mode: Async Recompiler (multi-threaded)` startet das Spiel binnen Sekunden ohne Geometrieverlust.

4. **100% autarker Asset-Patcher (`apply_times_ngp_delta.py` & `times_ngp_deltas.json`):**
   - Das Skript und die zugehörige Delta-Tabelle in `03_QUELLCODE` sind autark und benötigen nur die Python 3 Standardbibliothek.
   - Patcht SP- und MP-`times.psarc` auf die exakten Ziel-Hashes und setzt in `times.ltn` sowie `suburbs.ltn` den Wert `Distance Blur Enable: 0`.

---

## 5. Endstand & Binärprüfung (Test-Neubau 17.10.2026)

- **Neu kompilierte & gehärtete `rpcs3.exe` für den Test-Neubau:**
  - Build-Pfad: `C:\tm4k\build\bin\rpcs3.exe` -> bereitgestellt in `C:\tm4k\bin\rpcs3.exe`
  - **SHA256:** `10F9BDB0F829B0F7ABF13B5B3E4929029424A2955D1B284041D805556A768766`
  - Desktop-Verknüpfung: **Twisted Metal 4K - Test-Neubau.lnk**
- **Launcher-Selbsttest (`-CheckOnly`):** `PASS` (Konfiguration und Umgebung startklar).
- **Quellcode-Synchronisation:**
  - `C:\Users\box3\Desktop\Twisted Metal - Gesamtdossier\03_QUELLCODE\overlay\rpcs3\Emu\RSX\VK\VKPresent.cpp`
  - `C:\Users\box3\Desktop\Twisted Metal - Gesamtdossier\03_QUELLCODE\tracked-changes.patch`
  - Beide enthalten die flackerfreie Fusionslogik, die 30-Frame-Latch-Härtung und den einheitlichen Zuschnitt.
- **Bereit für Endanwender-Test:** Über das Desktop-Icon `Twisted Metal 4K - Test-Neubau.lnk` direkt spielbar.
