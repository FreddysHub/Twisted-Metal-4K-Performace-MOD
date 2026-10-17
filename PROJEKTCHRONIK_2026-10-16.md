# Twisted Metal: aktualisierte Projektchronik und technische Errungenschaften

> [!NOTE]
> Historisches Entwicklungsdokument. Pauschale Erfolgs-/Hardware-/Sicherheitsbehauptungen sind keine Abnahme. Aktueller Stand: [Build](BUILD.md), [Testnachweise](docs/REPRODUCTION.md), [Veröffentlichungsprüfung](docs/PUBLICATION_AUDIT.de.md). Lokale Archivpfade sind keine öffentlichen Downloads.

Stand16.Oktober2026. Quellen: erhaltenes Septemberdossier, zentrale Checkpoints,
Originalanalysen, Patches, aktuelle Arbeitsquellen, Launcher und neuere
Gemini-Checkpoints. Die Hinweise auf spätere Gemini-Arbeit sind übernommen,
aber in diesem Sammelschritt nicht als neue Ursachenanalyse nachgemessen.
Der Nutzer berichtet heute, mit Gemini die Version fertiggestellt zu haben.

## 1. Anfang: Spielversion, Emulator und die FPS-Grundlage

Zunächst wurden RPCS3 und God of War III eingerichtet, anschließend Twisted Metal.
Die frühen Twisted-Metal-Läufe hatten schwarze Ladebilder, Abstürze, Stottern
und teilweise langsame Spielbewegung trotz hoher angezeigter FPS. Festgelegt
wurde die **USA-Fassung BCUS98106 mit Update01.06**, PPU
`6bb4db395550102f567fb7b1dbbf81583417150f`. Andere Regionen/Updates dürfen die
versionsgebundenen Adressen nicht ungeprüft übernehmen.

Die entscheidende Performancebasis bestand aus mehreren Maßnahmen, nicht aus
einem einzigen Bloom-Schalter:

- Community-Patch **60 FPS**, illusion zugeschrieben: `be16,0x0065dbac,0x9383`.
  Er hebt das Spielziel an, garantiert keine tatsächlich erreichten60FPS.
- Community-Patch **Disable SPU MLAA**, Whatcookie zugeschrieben, für
  `SPU-82b3399c8e6533ba991eedb0e139bf20c7783bac`. Teure ursprüngliche
  Kantenglättung nicht als eigene Erfindung ausgeben.
- Lokaler **PostFX Sharp Fast v0.5**: elf PPU-Instruktionen durch NOP
  (`0x60000000`) ersetzt:

```text
0066aaf4 0066ab30 0066ab74 0066ab84 0066ab90 0066ab9c
0066aba8 0066abb4 0066abc0 0066ac1c 0066ac38
```

Dieser selektive Bypass spart Motion-Blur-/MLAA-nahe und weitere PostFX-Arbeit.
Bufferinitialisierung, erforderliche Bloom-Vorstufen und finales Tonemapping/
Color-Grading bleiben bewusst aktiv. **Nicht die Anleitung „Bloom komplett aus“
verwenden.** Die konkreten heutigen Patchdefinitionen/Enables sind im
Release-Snapshot gesichert; Originale früherer Fassungen in den Checkpoints.
Das exakte Einsparpotenzial jeder einzelnen Instruktion wurde nicht mit
isolierten Frametime-A/Bs gemessen. „Von etwa20/30 auf flüssige60“ ist die
damalige Nutzerbeobachtung für die Kombination, kein separater Bloom-Beweis.

Die Fassung wurde als SAFE60FPS konserviert. Xbox-Elite/XInput-Zuordnung und
Startverknüpfungen gehörten zur Einrichtung; spätere neue Builds brauchten auch
das passende Profil/Arbeitsverzeichnis. Eine geänderte Auflösung entlastet nicht
automatisch die Emulations-CPU, sondern kann zusätzliche GPU-Arbeit erzeugen.

## 2. Warum 300% allein die Welt nicht scharf machte

HUD/Schrift sahen bereits hochauflösend aus, Teile der eigentlichen Spielwelt
blieben grob. 200/300/400%-Versuche und CAS-Schärfung brachten nicht den
gewünschten Detailgewinn. Qwen und weitere Modelle untersuchten PSARC, Globals,
UI-GFx, PPU-Aufrufe, SPU-PostFX und DMA-Pufferabhängigkeiten. Das war nützliche
Vorarbeit, aber nicht der fertige Renderer-Fix.

Frühe12-/14-Plane- und Guest-Puffervergrößerungen erzeugten teilweise falsche
Farben, Menübeschnitt oder Speicherfehler. Ein Bericht erklärte12Plane als
„100%sicher“; die Gegenprüfung widerlegte das als aktive sichere Anleitung.
Der1344x756/14Plane-Test meldete explizit VRAM-Allocatorüberschreitung und Crash.
Diese Dateien sind erhalten, aber nicht als Erfolgsrezept übernehmen.

Der entscheidende Nachweis gelang mit **RenderDoc** im echten Match:
hochaufgelöste RSX-Szene3840x2112, anschließend farbrichtiger PostFX-Puffer
1280x704, der per Nearest wieder3x hochgeblittet wurde. Genau dieser späte
Detailverlust erklärte das grobe Weltbild trotz300%ResolutionScale. Offline-
Vergleichsbilder testeten, ob sich die vorhandenen Szenendetails mit korrekten
Farben und originalem HUD verbinden lassen.

Quellen: `01_WORKSPACE/postfx-120min/RENDERDOC_MATCH_FINDINGS_2026-09-28.md`,
`2026-09-28-handoff-verification.md`, Qwen-/PostFX-Berichte und Gegenprüfungen.

## 3. Eigener RPCS3-Vulkan-Pfad statt immer weiterer Spiel-NOPs

Separate RPCS3-Quelle und Windows-Buildwerkzeuge wurden eingerichtet.
Basiscommit: **1707d7fc883ef48ff21bdcbb0141a3211ae09cb2**.
Die Änderungen liegen im Emulator, vor allem im **RSX-/Vulkan-Präsentationspfad**,
nicht im AMD-Windows-Grafiktreiber. Für Upstream wäre es ein RPCS3-Quellpatch.

Erste Live-Fusionen von scharfer Szene und farbrichtiger Fläche waren sichtbar
besser, ließen aber UI/HUD weg oder legten Schichten räumlich falsch übereinander.
Die finale Original-Präsentationsfläche wurde deshalb als getrennte Quelle
erhalten. Der abgeglichene Rechteckbereich und Schutz von UI/Himmel waren nötig,
nicht nur mehr Schärfung.

Numerische Abbildung im damaligen Frame: Szene in final3840x2160 nicht über die
ganze Fläche, sondern innerhalb `(117,72)..(3723,2088)`, Größe3606x2016.
Nach Korrektur stieg im untersuchten Frame die Bildkorrelation von~0,843 auf
~0,986. Nutzerbilder14/15 zeigten zuvor versetzte Konturen,16–18 den verbesserten
Stand. Kamera und UI-Rechteck mussten zusammenpassen.

Eine HDR-Szenenquelle bei RSX`0xc1be0000` lieferte besser geeignete Detailanteile
als die ursprüngliche8Bit-Rohfläche. Lokale Farbabbildung/regularisierte Fits
übernahmen Detail, ohne pauschal falsche HDR-Farben zu zeigen. `Unbenannt20`
wurde als wesentlich besser bestätigt. „Alle Originaltexturen wurden in4K
ersetzt“ wäre dabei falsch: zunächst wurde vorhandene hochauflösende
Szeneninformation im Emulator wieder genutzt, nicht der Assetbestand ersetzt.

## 4. Schwarzer Sicherheitsrand: gemessen, zugeschnitten, korrekt gebaut

Der Rand war Bestandteil des vom Spiel präsentierten Bildes. RPCS3-Vollbild/
Stretch allein entfernte ihn nicht. Der lokale finale Vulkan-Blit nimmt für den
Twisted-Metal-Pfad nur die sichtbare Quellregion, damals1202x672 im1280x720-
Raster, und zieht sie auf die volle Ausgabe. Bei300%: `(117,72)..(3723,2088)`.

Ein erster Versuch linkte nur die EXE, ohne die geänderte Emulatorbibliothek neu
zu übersetzen: Rand blieb. **Zuerst rpcs3_emu, dann rpcs3 neu bauen** löste das.
Log zeigte `crop_enabled=1`; Nutzer bestätigte ausdrücklich **„Rand weg, Bild
vollständig“**. Die belegte damalige EXE hatte SHA256
`060FDAF4ED73DA5568880035B9B5CA5CBFCE734242B42D6E1887B1480B7B2910`.
Spätere Display-Inset-/Default-Display-Arbeiten erforderten weitere Abbildungen.
Nicht blind die frühere fixe Cropzahl mit einem später zurückgesetzten Ingame-
Displayprofil kombinieren; aktuelle Quelle und Default-Display-Checkpoints lesen.

Primärquelle: `01_WORKSPACE/checkpoints/2026-09-30-borderless-4k/README.md`.

## 5. Nach dem ersten Dossier: Farben, Logos, Schärfe und Frame-Zuordnung

Das alte Dossier endete am30.September. Danach folgten diese Arbeitsblöcke:

| Zeit | Arbeit / Beobachtung | Checkpointgruppen unter01_WORKSPACE/checkpoints |
|---|---|---|
| 30.09.–01.10. | Menü-/Video-Farben, hohe Details ohne helle Säume, Packed-Alpha-Interpretation, erneute UI-Transparenz und Straßenlecks hinter Logos | menu-video-investigation, sharp-hdr-menu, packed-alpha, neu5-latch, neu5-ui, source-contract, str2-regression |
| 01.10. | Quellen räumlich/zeitlich paaren, Synchronisation, Default-Displaywerte statt vom Nutzer verschobener Bildbereich, Detail-/Palettevarianten und getrennte erfolgreiche/fehlgeschlagene Tests | frame-pairing, compute-sync, default-display-ingame, display-inset-diagnosis, edge-palette-live, detail-variants, best-detail-only-confirmed |
| 02.10. | Innenraum/Fahrer, Himmel, wechselnde Detailgeneration, bewegte Objekte mit verzögerten Konturen; getrennte Timing-Fassung | cabin-driver-fix, E-sky-fix, timing-generation-test, best-E-motion-logo-capture |
| 03.10. | Original-PostFX/DOF-Gegenprobe entdeckte fehlendes Sweet-Tooth-Feuer; cyan/falsch platziertes Feuer im Capture. Mehrfacher Farb-/Detailpfad und Partikelreparatur | original-postfx-test, fire-analysis, fire-input-test, logo-leak-and-fire, particle-repair, three-scenes-and-fire |
| 03.–04.10. | Direkte hochauflösende Szenenschicht, Quellenkohorten, bewegte Materialien, Affin-/Hybrid-Farbrekonstruktion statt beliebiger globaler Verschiebung | scene-2160, cohort-ten-captures, motion-generation-fix, affine-master-live, hybrid-master-live, colour-detail-repair |
| 05.10. | Zwei-Pass-native Farbreferenz/Mode6 und fünf Masterbilder; GPU/Offline-Vergleich als Implementierungsprüfung | colour-layer-audit, native-colour-live, four-native-masters, final-five-live |
| 07.10. | Weitere Feuer-/Kanten-/Master- und Sichtweitearbeiten, Bildvergleiche/Gemini-Beiträge | final-five-fire-and-edges-live, ultimate-4k-master-live, 4k-sichtweite-live, streaming-lod-test |

Diese Tabelle ist eine Navigation, keine nachträglich erfundene Abnahme jeder
Zwischenfassung. Die Original-READMEs, Quellen, Builds, Checks und Nutzerbefunde
liegen vollständig bei; der CHECKPOINT_INDEX erschließt auch hier nicht
einzeln genannte Checkpoints.

Feuer wurde nicht einfach grün nach orange umgefärbt. Die03.Oktober-Analyse
fand das sichtbare Feuer erst in einem späteren Partikel-Draw im HDR-Ziel,
während die gewählte Guide-Schicht es nicht enthielt. Räumliche Fehlzuordnung
und fehlender Partikelanteil wurden getrennt behandelt. Der reparierte Live-
Lauf wurde vom Nutzer mit **Feuer wird angezeigt** bestätigt; derselbe Lauf
enthielt aber auch einen Guest-SpuPostFX-Fehler. Das war kein Langzeitbeweis.
Native-Partikelisolation und getrennte Komposition blieben wichtige Teile.

Bei den späteren Farben-Mastern wurde eine Zwei-Pass-Log-RGB-Anpassung mit
kantenbewusstem Guide und nativer Farbreferenz umgesetzt, statt rohe HDR-Farben
direkt als Endbild zu benutzen. Das Mode6-Checkpoint protokolliert GPU-Abgleich
von8294400Ausgabepixeln innerhalb einer8Bit-Farbstufe gegen Offlinebilder.
Das ist eine Reproduktionsmetrik, **kein Regler „8,29MioTexel je Fahrer/Rücklicht“**.

Timing war entscheidend: passend abgeschlossene Geometrie-/HDR-/Farbflächen
statt unabhängig jeweils neuester Fläche. GeometryLag1 wurde zeitweise genutzt,
später von Gemini auf0 gesetzt; genaue Vor-/Nach-Generation ist versionsgebunden.
Eine ältere scharfe Szene über neuere Bewegung zu legen kann Ghosting erzeugen.

## 6. Fernobjekte und der schwarze Boden: Erfolge und erfolglose Reparaturen

Die C-/Nahdetails-Fassung setzte den globalen Modell-LOD-Resetfaktor an
`0x0010e83c` auf0. Der Nutzer sah extrem gute Ferndetails und flüssige Fahrt,
aber Straßen und ganze Häuserblöcke fehlten zeitweise. Die Fehleraufnahme3505
zeigte in untersuchten schwarzen Bereichen gelöschte aktuelle Tiefe1.0.
Vorhandene Materialfarbe allein war kein aktueller Geometriebeweis.

| Versuch | Wesentliches Ergebnis |
|---|---|
| D: global1/64 statt0 | Schwarze Straße blieb |
| E: DrawGuard/Pipeline-Abwarten | Schwarze Straße blieb, zusätzliche starke Ruckler |
| G: Accurate RSX reservation access | Ferndetails gut, Weltfehler blieb |
| H: zweites Mesh-Distanzgate an Nahwahl anpassen | Richtungsabhängiger Fehler blieb |
| I: Freigabealter/Anforderungen jüngst berührter Meshes | Livecodes nachgewiesen, Fehler blieb |
| J: Bereitschaft auch bei gleichem LOD prüfen | Mehr geladene Records, fehlende Welt blieb; GPU-Capture nurPresent/Compute |
| K: ursprüngliche Requests + Retention/Bereitschaft | Fehler blieb |
| F: global1 zurück | Welt gut, ferne Leuchtbäume wieder grob |
| L/M: begrenzte Asset-Detailgruppen | Welt gut, Zielzweige noch nicht ausreichend |
| Q: global0,5 plus HostGPU-Labels | Nutzer etwa halbe FPS; Beiträge nicht getrennt gemessen, verworfen |

Aus diesem Verlauf folgte nicht der Beweis einer bestimmten defekten
Grafiktreibereinstellung. Die Gast-LOD-Auswahl, tatsächliche Mesh-Residency,
Anforderung/Freigabe und Sichtbarkeit müssen konsistent bleiben. Der perfekte
Mechanismus hinter jedem Ausfall war dadurch noch nicht abschließend bewiesen.

Die gezielte Alternative identifizierte tatsächliche Weihnachtsbaum-Lampen-
Geometrie über GPU-Positionsbytes in times.ngp. Es wurden nicht nur vermutete
Texturbezeichnungen umgeschaltet. Zwei SP-LOD-Zweige43f8/6588, passende
MP-Zweige4018/61a8, Floatfelder SP441c/442c/65ac/65bc und
MP403c/404c/61cc/61dc wurden kontrolliert erweitert:

```text
N: Nah900 / Ende1000
O: Nah1350 / Ende1500
R: Nah1800 / Ende2000
S: Nah5000 / Ende5600
```

R wurde vom Nutzer als super/flüssig bestätigt. S war ein größerer gezielter
Versuch, nicht dieselbe breite globale Fernwirkung wie C. Diese Zahlen sind
Spiel-Distanzeinheiten, **nicht nachgewiesene Meter**. Holzschilder/Häuser sind
nicht automatisch dieselben Baumzweige. Übergeordnete Sichtgrenzen können bleiben.

Am15.Oktober wurden auf Wunsch32alte Laufzeitfassungen und36Links in den
Papierkorb verschoben, C/R/S behalten, gemeinsame Daten gesichert und
Verzeichnisverweise umgestellt. Keine Shader-/Spielreparatur dadurch. Der
Frisch-Capture4753 mit erneut schwarzem Boden wurde geprüft und Gemini samt
3505/4453/J3299, bisherigen Fehlschlägen und Werkzeugen übergeben.

## 7. Geminis Fortsetzung am15./16.Oktober: belegte Dateien, offene Zuordnung

Neu vorhanden sind Echte-Dichte-/Sichtweite-/Stabilitäts-/Operation2D-Checkpoints.
Sie und die heutigen Launcher/Snapshots werden mitgesichert, nicht durch die
älteren C/R/S-Unterlagen ersetzt. Dokumentiert wurden u.a.:

- Weitere Level-Asset-Reichweitenänderungen in times/suburbs statt allein
  globaler Distanzfaktoren; Berichte nennen teils179/181times- und437suburbs-
  Kandidaten. Exakte gültige Endzählung/Abhängigkeiten muss Gemini bestätigen.
- Aktuelle Sichtweite-Konfiguration hat60FPS, PostFXSharpFast und erweiterten
  DrawDistance-Patch an; Raw4K und globale0/0,25/0,5-LOD-Patches aus. Der
  Distanzpatch enthältFloatwerte10000 bei002106d4 und16000 bei002106f8.
  Die ältere Bezeichnung CULL versus heutige Fog-Notes ist nicht abschließend
  semantisch gegengeprüft; nicht bloß den Namen als Beweis übernehmen.
- Verkleinerte Bewegungsverzögerung: Launcher GEOMETRY_LAG1=0.
- Für Kollisions-/Partikelbursts: INTRA_GAP2500, COHORT_WINDOW4000,
  Quellcode enthält eine weitere Auswahl bisTagabstand12000 und einen
  bis2Frames gehaltenen4K-Fusionspuffer. Das kann native720p-Rückfälle abfangen,
  benötigt aber Ghosting-/Stale-Frame-Tests; „physikalisch ausgeschlossen“ wäre
  als generelle Qualitätsgarantie zu stark.
- Operation2D: native3x3-Pixelplateaus von Overlays über rasterbewusste
  Catmull-Rom/Bikubik-Abtastung glätten. Das ist eine Rekonstruktion vorhandener
  2D-Information, kein Beweis neu vorhandener4K-Originaltexturdetails.
- Sichtweite-Launcher/Profile derzeit CAS4 und LOD-Bias-0,10. Ein älterer
  Operation2D-Bericht nennt8/-0,20, ein Stabilitäts-Logtext-0,75. Diese Werte
  sind nicht gleichzeitig die eine finale Fassung; tatsächliches Profil sichern.

Im neuesten Sichtweite-build-status steht `user_match_verified:true`; Nutzer
berichtet heute Fertigstellung. Andere neuere Berichte stehen noch auf
`ready-for-user-test`. Die Final-EXE und genau dazugehörige Quelle sollen
deshalb von Gemini im Ergänzungsblatt freigegeben werden. In diesem
Sammelvorgang wurden weder neue Gameplaytests noch ein sauberer Neubau gemacht.

## 8. Was zur Veröffentlichung als unsere Arbeit beschrieben werden kann

Die belegbare Leistung ist die systematische Aufdeckung des spät nativen
PostFX-Flaschenhalses, titelgebundene GPU-Detail-/Farbrekonstruktion, korrekte
UI-/Partikel-Komposition, Rand-/Raum-/Zeitzuordnung und gezielte Asset-/LOD-
Arbeiten, gestützt durch echte GPU-Aufnahmen und private Tests.

Community-Patches, RPCS3 und Beiträge anderer Modelle/Personen getrennt nennen.
Qwen/Antigravity/Gemini lieferten relevante Vorarbeiten, Analysen, Varianten
und neuere Integrationen; ChatGPT lieferte u.a. Pipelineanalysen, Live-Pass-
Entwicklung, Vergleichsbilder, Untersuchungswerkzeuge und Sicherungen.
Ein Weltrekord, universelle60FPS oder „als Erste überhaupt“ wurde nicht
unabhängig nachgewiesen. Für eine belastbare Community-Veröffentlichung zählt
der nachbaubare Quellstand mit ehrlichen Messungen, nicht solche Superlative.
