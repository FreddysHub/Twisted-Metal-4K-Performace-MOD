# Belegstatus, historische Fehler und noch zu klärende Stellen

> [!NOTE]
> Historisches Entwicklungsdokument. Pauschale Erfolgs-/Hardware-/Sicherheitsbehauptungen sind keine Abnahme. Aktueller Stand: [Build](BUILD.md), [Testnachweise](docs/REPRODUCTION.md), [Veröffentlichungsprüfung](docs/PUBLICATION_AUDIT.de.md). Lokale Archivpfade sind keine öffentlichen Downloads.

Dieses Blatt verhindert, dass das Sammelarchiv widersprüchliche Zwischenstände
unbemerkt in eine „perfekte“ Bauanleitung verwandelt. Es ist kein neuer Fixauftrag
und verändert die heute vom Nutzer für fertig erklärte Spielfassung nicht.

| Aussage | Einordnung |
|---|---|
| 60FPS/SPU-MLAA/selektiverPostFX als frühe Grundlage | Patchdefinitionen und Nutzerberichte vorhanden; Einzelbeiträge nicht als isolierte Benchmark gemessen |
| Bloom komplett aus gab allein60FPS | So nicht belegt; benötigte Bloom-/Tonemap-Vorstufen blieben erhalten |
| Szene hochaufgelöst, danach nativer704p-Farbpfad | Durch echten GPU-Frame nachgewiesen |
| Rand weg,Bild vollständig | Direkte Nutzerabnahme plus aktiverCrop-Log; aktuelle spätereDisplayprofile separat beachten |
| Alle Originaltexturen haben8294400Texel | Falsch: Ausgabepixel/Prüfmetrik ist keine Texelzahl jeObjekt |
| Früher12Plane garantiertVRAM-sicher | Historische Gegenprüfung/Crash widerlegt pauschale Sicherheitsbehauptung |
| C scharfe Ferndetails,hoheFPS,schwarzerBoden | Nutzerbefund/Captures vorhanden; keine vollständige stabileC-Fassung daraus ableiten |
| G/H/I/J/K hätten Boden behoben | Nutzerfehlschläge dokumentiert; Opcode-/Build-PASS war kein Gameplay-PASS |
| Der AMD-Vulkan-Treiber wurde von uns repariert | Nein: lokaler RPCS3-RSX-/Vulkan-Pfad, kein geänderter AMD-Treibercode |
| NeuereGemini-Fassung ist fertig | Aktuelle Nutzerangabe; exaktes Release/Quelle/Assetrezept bitte imErgänzungsblatt festlegen |

## Konkret in neueren Unterlagen zu ergänzen

1. **Welche EXE ist final?** Desktop hat Sichtweite und Stabilitäts-Test.
   Sichtweite-build-status nennt user_match_verified:true, andere Stände noch
   ready-for-user-test. Im Snapshot errechnete EXE-Hashes als Identität verwenden.
2. **Globaler Faktor:** Stabilitäts-build-status sagt +100%(2x)aktiv, zugehörige
   README/Launchertexte behaupten native1,0. Aktive patch_config-Datei und
   Laufzeit-`PAT: Applied patch` gegeneinander prüfen. Nicht nur Texte vergleichen.
3. **LOD-Knotenzahl:** Berichte nennen179oder181times- und437suburbs-Knoten.
   Kandidatenheuristik ist keine nachgewiesene semantische Zuordnung. Exakte
   geänderte Offsets/Input-/Outputhashes und erhaltene Gebäudekoordinaten beilegen.
4. **Einheiten:** 5000/5600 sind interneDistanzwerte; ohne Kalibrierung nichtMeter.
   `ExtendDrawDistance` nennt an002106f8 einmalCULL,einmalFog. Semantik belegen.
5. **Schärfeprofil:** Operation2D nennt8/-0,20, aktuellesSichtweite-Profil4/-0,10,
   Stabilitäts-Launchmetadaten-0,75. FürFinal genau eine tatsächlicheConfig nennen.
6. **QuellezuEXE:** Aktueller Arbeitsbaum enthält elfgetrackte Änderungen und
   mehrere neueTM4KHeader. Nicht bewiesen,dass jede noch vorhandeneEXE daraus
   gebaut wurde. `source-used` und Buildlogs dienen derZuordnung, CleanBuild folgt.
7. **Latch/Timing:** Bis2Framesalt gehalteneFusion kann720p-Fallback verstecken,
   erzeugt aber möglicherweise Ghosting/Latenz. BreitereTagfenster sind keine
   garantierte korrekte Quellenpaarung. Bewegung/Kollision/UI-Wechsel messen.
8. **Operation2D:** BikubischeInterpolation beseitigtPlateaus, ersetzt aber nicht
   alle 720p-Quellinformationen durch echte4K-Assets. RichtigeNamensgebung behalten.
9. **Leistungsbehauptung:** VollständigeWelt nachFix kann mehrArbeit kosten als
   einFrame mitfehlendenHäusern. Vergleiche mitgleicher Szene nötig.
10. **Archivabdeckung:** BestehendeChatteile sindhistorisch bisEndeSeptember;
    spätereChronik basiert aufCheckpoints/Dateien/Nutzerangaben, nicht behauptetem
    vollständig neu exportiertemChat. AusgelasseneJunctions stehen imManifest.

Ältere Aussagen werden als Quelle aufbewahrt, nicht gelöscht oder heimlich
umschrieben. Öffentliche Schlussfolgerungen sollen sich auf überprüfte aktuelle
Dateien und reproduzierte Tests stützen.
