# Twisted Metal – Arbeitsstand und Rückwege

## 2026-10-16: Gesamtdossier vom Anfang bis Gemini-Finalarbeit gesammelt

Neuer Desktop-Ordner „Twisted Metal - Gesamtdossier“. Altes Veröffentlichungs-
Dossier(30.September) gefunden/unverändert erhalten, neue Projektchronik,
Neubau-/Abnahme-Anleitung, Belegstatus und Gemini-Ergänzungsblatt hinzugefügt.
Sämtliche80Checkpointordner und ermittelten Analyse-/Desktop-Sammlungen,
insgesamt20501Quelldateien/41RDC-Dateien SHA256-geprüft gesammelt. Logische
Größe169506027128Bytes; große historische Dateien perNTFS-Hardlink statt
erneuterVollkopie, veränderlicheDokumente und aktuelleRelease-Snapshots kopiert.
Kompletter getrackterSource-Diff gegen1707d7fc und lokaleHeader/Overlay,
Buildreferenzen und3neuereGeminiRelease-Snapshots inklusiveConfigs/Patches/
vorhandenenAuswahl-Levelarchiven beigefügt. EigeneISO/Firmware nicht gesammelt.
Nutzer meldet Fertigstellung mitGemini; konkrete finaleReleasezuordnung und
Widersprüche(LOD-Aktivierung,Knotenzahlen,Profilwerte) müssen Gemini ergänzen.
Kein Spielstart,keineParameteränderung undkeinCleanBuild beiZusammenstellung.
Das Paket ist privat,nicht alsGanzes veröffentlichen. Prüfsummen/Zuordnungen
unter05_PRUEFSUMMEN; Wiedernachbau ist der nächste separateTest.

## 2026-10-15: Frischen C-Fehlerframe4753 zum Gemini-Auftrag hinzugefügt

captures/tree-lights-stable-renderdoc/2026-10-15_01-48-44-355-a5db98f6/
tm-tree-lights-stable_frame4753.rdc,1586428893Bytes,SHA256
D5703E7F4A429052A65C2BAF7C19DC5F05C51C72E6395A8AC825F012DB597256.
Nutzer meldet gleichen Fehler, bessere Aufnahme. Datei stabil/Hash geprüft,
Startprotokoll bestätigt C-warm-async/Faktor0/RenderDoc geladen. Noch KEIN Replay
oder Draw-/Bildnachweis dieses Frames. Gemini soll4753 zuerst auswerten,
3505-Messungen nur als Referenz, keine ResourceIds blind übertragen. Auftrag,
STATUS und captures.json aktualisiert, nun4RDCs. Kein Spiel/Build verändert.

## 2026-10-15: Neuer Gemini-Auftrag – C/Nahdetails vollständig reparieren

Nutzer bestätigt heute richtige C-Version, möchte deren breite scharfe Fern-
Details/flüssige Fahrt erhalten, schwarze Straßen/fehlende Häuser beseitigen.
Auftrag unter Desktop/Aufgabenzettel/GEMINI_2026-10-15_NAHDETAILS_SCHWARZER_BODEN,
START_HIER.md plus STATUS/evidence/scripts/captures.json. C3505(847335316Bytes),
gute4453(2528406870), spätereJ3299(387239226) existieren/SHA256 geprüft. J3299
hat nur finalen Compute-/Present-Teil, nicht volle Geometrie. Original-ELF und
Replay-Tool vorhanden. Keine Gigabyte-Duplikate, absolute Captureverweise.
D/E/G/H/I/J/K-Fehlschläge und Q-FPS-Verlust dokumentiert, R/S begrenzter
Lampenerfolg ausdrücklich nicht als Ersatz für volle C-Reparatur akzeptieren.
Neue Analyse/Implementierung durch Gemini noch OFFEN. Keine C/R/S-Änderung,
kein Spielstart durch Erstellung des Auftrags. Keine bewiesene Ursache behaupten.

## 2026-10-15: Auf Nutzerwunsch nur C/R/S als private Testfassungen behalten

C/Nahdetails-Test, R/BäumeReichweite1800/2000, S/Bäume5000 bleiben samt je
RenderDoc- und normalem Desktop-Link.32 übrige bin-*-Testordner und36 Links
in Papierkorb (nicht geleert). Andere alte Rückwege unten sind HISTORISCH,
nicht mehr an ihren aktiven Pfaden vorhanden; Checkpoints/Quellen erhalten.
Gemeinsame Firmware-/Spieldaten separat in shared-three-versions:29Verzeichnisse,
1378Dateien hashgleich; Retained-Junctions direkt dorthin. C-Historien-Startcheck
auf gesicherte baseline-witness-Dateien umgeleitet, keine Grafikänderung.
C/R/S CheckOnly nach Aufräumen PASS; kein Spielstart. Cleanup-README/Manifeste
unter checkpoints/2026-10-15-cleanup. Shared-Daten NICHT entfernen.

## 2026-10-09: R vom Nutzer bestätigt; S5000 gezielt vorbereitet

R1800/2000 laut Nutzer super, Performance wieder vernünftig, Randlampen näher
am Zaun detailliert, etwas fernere Nachbarn weiter Fern-LOD. Kein schwarzer
Boden in diesem Befund. R geschützt, USER-RESULT.md hinzugefügt. Holzschild
an fernem Haus poppt: nicht nachweislich Lampenmaterial/kein Automatismus.
S private F-Kopie + R-Archive mit NUR Lampen-Enden5000/5600. Global1,
Host-Labels aus. Je4Floats/10Bytes; keine allgemeine Welt/Streaming/Grafikänderung.
S eigener Desktop-Link Bäume 5000 Test (+RenderDoc), noch kein Spieltest.
S114 statische Checks PASS; S/R -CheckOnly PASS, Linkziele rückgelesen.
Aktuelles R-Spiel nicht beendet oder live umgestellt. Höhere Lampenreichweite
kann trotzdem Arbeit kosten, andere Sichtgrenzen bleiben möglich.

## 2026-10-09: Q verliert laut Nutzer etwa halbe FPS; R wieder nur Lampen

Nutzer Q: circa50% FPS-Verlust, einige ferne Lampen besser, äußerste Randobjekte
weiter wechselnd. P-Sync nicht einzeln abgenommen; Ursachenanteile ungemessen.
R entfernt BEIDE Q-Deltas: global0.5 wieder1.0, Host-GPU-Labels wieder aus
wie F/O. Nur identifizierte zwei Lampen-HierLODs pro SP/MP-times.ngp erweitert:
O1350/1500 -> R1800/2000 (+1/3). Je4 Floats/8 Bytes. Keine allgemeinen
Modell-/Welt-/Streaming-/CULL-Änderungen. Kronenflackern dadurch nicht bewiesen
behoben; Q bleibt als verworfener Versuch dokumentiert, nicht starten.
O/N erhalten. R eigene private F-Kopie und überprüfte Archive; EXE/Config/Patches
identisch F. Launcher Start-TreeRangeOnly-Test.ps1; Desktop Bäume Reichweite
Test (+RenderDoc). README/Generator/Validator unter selective-xmas-lod.
Noch kein R-Spielstart/Gameplay-Erfolg; FPS/Weltsichtbarkeit vom Nutzer prüfen.
R114 unabhängige statische Checks PASS; R/O -CheckOnly PASS. Zwei neue
Desktop-Links erzeugt und rückgelesen. Neuer Übergabeordner Baeume_ohne_global.

## 2026-10-09: P/Q – neue Kronenbeobachtung, begrenzter globaler Zwischenwerttest

Nutzer: O beste Fassung, aber ferne Laternen/Randbäume poppen, normale Kronen
verlieren teils Blätter auch nah während Fahrt. Das ist nicht durch Abstand
allein erklärt; keine gemeinsame Texturzuordnung behaupten.
P=O-Archive plus Allow Host GPU Labels:true, Faktor1. Q=P plus EIN literal0.5
bei0010e83c (d/2), global für betroffene Modelle; kein0/0.25 und kein H/I/J/K.
O1350/1500/N900/1000 unverändert als bestätigte Rückwege. Q NICHT als Fix abnehmen.
Offizieller RPCS3 PR11568 motiviert Sync-Versuch, Issue14202 zeigt Grenzen.
Laternenprobe lieferte bisher keine sichere Geometrie-/Assetzuordnung, keine
geratenen Laternen-Bytes gepatcht. Q-environment-preload/README.md enthält Details.
P70/Q71 statische Checks PASS; P/Q/O CheckOnly PASS. Q normal+RenderDoc und
P normal als Desktop-Links erstellt/Argumente rückgelesen. Noch KEIN Spielstart
dieser Fassungen in diesem Turn. Getrennte private Archive bytegleich O.

## 2026-10-09: O laut Nutzer sehr gut; äußerste Randbäume weiter offen

Nutzer bestätigt O als sehr gute Version. Äußerste unbefahrbare Randbäume
weiter nicht durchgehend detailliert; weitere Randobjekte nur Nutzervermutung.
O1350/1500 und N900/1000 unverändert erhalten. Keine neue Änderung/Spielstart.
Vergleich alter schwarzer-Boden-Test C: GLOBALER Reset-Faktor0.0 (Original1.0),
nicht eine endliche große Baumgrenze. Belegt in C-last-launch/README/Patch.
O-USER-RESULT.md unter O-lamp-range enthält aktuellen Befund und Abgrenzung.

## 2026-10-09: N vom Nutzer BESTÄTIGT – Welt sichtbar und Lampenbäume detailliert

Nutzer bestätigt gleichzeitig dargestellte Welt und vorgeladene beleuchtete
Bäume. Nahende900/Terminal1000 behalten, N nicht überschreiben.
Kein Nachweis für alle Level/Langzeit-FPS. Weitere entfernte Randbäume wechseln
noch ab Fahrgrenze. Neuer separater O-Reichweitentest1350/1500 (+50%) nur
derselben2 Lampenzweige pro times.ngp; globale/sonstige Modelldistanzen gleich.
USER-RESULT.md bei N lesen. O erst nach Gameplay als Erfolg bewerten.
O116 unabhängige statische Checks PASS; O/N CheckOnly PASS. Desktop-Link
„Twisted Metal 4K Lampen Reichweite Test“ (+ RenderDoc) erstellt/rückgelesen.
O NICHT gestartet. N-Restprozess6388 hatte kein Spielfenster und wurde nach
Ankündigung beendet. N-Log gesichert als user-confirmed-run.log; echte Öffnung
des privaten Updatelevels/times.psarc im Log bestätigt. Keine EXE/Sicherheits-
Änderung. Neuer Test erst durch Nutzer; maximale fehlerfreie Reichweite unbekannt.

## 2026-10-09: N – echte Lampen-Geometrie gebunden, gezielter Distanztest

Farb-PixelHistory der alten Winteraufnahme4453 an VOLLEN4K-Koordinaten:
Lampen nah3018/400 Events8779/8787, fern1556/637 Events8735/8743.
Positionsstream aus Vertex-SSBO stimmt exakt mit SP-NGP0x553fb0 überein.
Primitive417e30 -> Typ6-Blatt4330 -> HierLOD43f8. Zweite Variante6588
teilt Nah-/Fernmaterial; MP4018/61a8 mit bytegleichen vollen Positionsstreams.
N = M plus ausschließlich2 Lampen-Nahgrenzen160->900 je times.ngp;
terminal1000 bleibt gleich, globaler Faktor1. Primitive/Streaming/Code unverändert.
4 abweichende Bytes pro NGP. F/L/M geschützt; eigene private Archive.
Desktop: Twisted Metal 4K Weihnachtsbaum Lampen Test (+ RenderDoc).
Details/Rohbelege: selective-xmas-lod/N-tree-light-identification und
N-targeted-tree-lights. Gameplay NOCH NICHT ABGENOMMEN;900 ist Testdistanz.
104 unabhängige statische Checks PASS; N/F/M CheckOnly PASS. N normal gestartet
04:43:17+02:00,PID6388. Fenstertitel mit FPS vorhanden; KEINE Winterlevel-Abnahme.
Aktives Log Sharing violation, auch mit ReadWrite; Spiel deshalb nicht beendet.
Nach Test Log sichern/Archiv-Override prüfen. RUNTIME-START.md enthält Details.

## 2026-10-09: M stabil, Baumdetail-Ziel weiterhin NICHT erreicht

Nutzer: kein schwarzer Boden; Weihnachtsbäume in Ferne matschig, nah detailliert.
Die33 Ergänzungen verbessern den getesteten Fernbereich nicht. Kein laufender
RPCS3-Prozess beim Nachsehen. Keine neue Fassung oder Spielstart in diesem Turn.
M-resident-small-lod/USER-RESULT.md beachten. Die Objektidentität und getestete
Distanz sind noch nicht gebunden: kein Beweis, dass alle93 Gruppen baumfremd
sind. Konkreten Baum-/verschachtelten Detailzweig identifizieren statt globalen
Null-Distanz-Schalter wieder aktivieren. F/L/M als unveränderte Kontrollen behalten.

## 2026-10-09: M vorbereitet – 33 vorher ausgelassene Detailgruppen

M ist private F-Kopie mit den60 L-Änderungen und33 zusätzlichen kleinen
HierLOD-Gruppen ohne MeshData-Checkliste. Insgesamt291 passende Kandidaten
statt vorher87;204 ohne Meshcheckliste. Auswahl nach vollständigem kleinen
Nah-Unterbaum über nachgewiesene Typ9/7/16-Kanten zu1–5 Typ6-Blättern,
Radius<=80; unbekannte Transform-/Effektzweige bleiben unverändert.
M vs L ausschließlich33 Nahgrenzen-Floats je NGP; kein globaler Nahzwang.
Typ6-Primitive-Pakete NICHT mit Streaming-MeshData verwechseln/überschreiben.
Welt-/Endgrenzen, Code,4K/Farben/Feuer unverändert; F/L CheckOnly weiterhinPASS.
800 statische Prüfungen PASS. Dies ist KEINE visuelle Abnahme oder tree_xmas-
Namensidentifikation. Desktop: Twisted Metal 4K Detailgruppen Test (+ RenderDoc).
Details/reproduzierbare Änderung: M-resident-small-lod/README.md im
checkpoints/2026-10-09-selective-xmas-lod. Erst Winterleveltest entscheidet.

## 2026-10-09: L stabil, aber Baumdetail-Ziel NICHT erreicht

Nutzer: Straße und Häuser normal, entfernte beleuchtete Bäume wieder matschig.
L-user-test.log bestätigt beide privaten Update-PSARCs tatsächlich geöffnet.
Also kein fehlender Archiv-Override; Auswahl der 60 Gruppen reicht nicht.
F/L bleiben unverändert. Weiterprüfung ausgelassener LOD-Knoten ohne
MeshData-Einträge; kein pauschales Wiedereinschalten großer Weltgruppen.
Nachweis: checkpoints/2026-10-09-selective-xmas-lod/L-USER-RESULT.md.

## 2026-10-09: L vorbereitet – Kleinobjekt-Assets statt globalem Nahdetail-Zwang

Private bodenfehlerfreie F-Kopie: bin-tree-small-lod-test; Code/Config bytegleich F.
Nur times.ngp in privaten levels/times.psarc und mplevels/times.psarc geändert:
je60 strukturell kleine LOD-Gruppen;27 andere geparste Gruppen unangetastet.
Nahende min(alt*4,terminal*.90); terminale Sichtgrenze unverändert. Passende
MeshData-Lade-/Entladegrenzen gemeinsam skaliert. Keine globalen H/I/J/K-Patches.
Je394 Floatfelder/949 veränderte Bytes; alle sonstigen Archivmitglieder bytegleich.
532 statische Checks PASS, Launcher/F CheckOnly PASS. Gameplay NOCH NICHT GETESTET.
Desktop: Twisted Metal 4K Kleinobjekte LOD Test (+ RenderDoc). L gestartet PID16984;
Frame311 echte Original-PPU-Opcodes und globaler Faktor1.0 bestätigt. Kein
Winterlevel-Erfolg behauptet. Alter headless Prozess19416 nach Ankündigung und
Log-Sicherung beendet. Einzelne Xmas-Namen weiter nicht sicher identifiziert.
Vollständige Änderungen/Reproduktion: selective-xmas-lod/L-selective-small-lod/README.md.

## 2026-10-09: K FEHLGESCHLAGEN – Boden weiterhin schwarz

Original-Ladeanforderung wiederhergestellt, neue Bereitschaftsprüfung und signed
Framealter beibehalten; globale Nahwahl/H-Gate weiter aktiv. Nutzer: Boden schwarz.
K-user-failed.log bestätigt tatsächliche Patch-Anwendung. 72 statische Checks
waren KEINE Erfolgsmessung. Details: K-USER-RESULT.md im selective-xmas-lod-Checkpoint.
J-Aufnahme3299 erfolgreich rekapituliert und exportiert: echte387MB-RDC, fehlende
Welt sichtbar, aber0 Drawcall-Actions/181 Events: nur finaler Compute/Presentpass.
Kein vollständiger Geometrie-/Tiefennachweis daraus. F unverändert bestätigt.
Nächster Ansatz selektive identifizierte Xmas-LOD statt globaler Null-Distanz.
Aktuell gefundener Prozess19416 ist bin-streaming-lod-test, NICHT K; unangetastet.

## 2026-10-09: J FEHLGESCHLAGEN – Häuser fehlen, Straße schwarz; Livewerte gesichert

PID18208: Nutzer bestätigt Teile der Welt schwarz, nicht gesamtes Fenster.
Frame4009:820 ready von1170; Frame9508:773 ready. Mehr geladene Modelle,
aber Boden-/Häuserfehler unverändert. J-USER-RESULT.md und zwei Live-Snapshots.
513 times.ngp-Meshsignaturen eindeutig live gebunden, Basis43d71400.
Zusatz-LOD-Cacheprobe unplausibel wegen Kontextpool; NICHT als Ursache verwenden.
Spiel nach Ankündigung selbst sauber geschlossen, J-user-failed.log gesichert.
F unverändert. Bisher keine bestätigte Endreparatur.
GPU-Nachprüfung abgeschlossen: identischer bekannter Material-FS in schlechtem
C-Frame15x vorhanden. Nicht einfach fehlender Shader; keine Modellidentität
bewiesen. GROUND-PROGRAM-FINDINGS.md. Noch keine aktuelle J-GPU-Aufnahme.

## 2026-10-09: J vorbereitet – LOD-Prüfung auch bei gleichem Nahindex

Konkrete I-Kontrollflusslücke: 3c00b4 führt nur bei kleinerem LOD-Index zur
Mesh-Bereitschaftsprüfung. Dauerhaft Index0 kann ungeprüft zurückgegeben werden;
dabei fehlt auch die Vorbereitung jüngster Berührung für I-Anforderung.
Private I-Kopie J: 3c00b4 41800254→48000254, Originalprüfung/Fallback nutzen.
101 statische Checks PASS, beide Launcher J/F CheckOnly PASS. NICHT abgenommen.
Details: checkpoints/2026-10-09-selective-xmas-lod/J-READY-SELECTION-TEST.md.
Desktop: Twisted Metal 4K Modellbereitschaft Test (+ RenderDoc). F unverändert.
J ohne RenderDoc gestartet, PID5672. Neue Instruktion im Gastcode exakt gelesen:
3c00b4=48000254. Frühe Startmessung Frame0, NICHT Match/Erfolg; siehe
J-RUNTIME-PATCH-PROOF.md. Spiel für Nutzer-Test offen gelassen.

## 2026-10-09: I FEHLGESCHLAGEN – Ladeabgleich behebt schwarzen Boden nicht

Nutzer meldet weiterhin dasselbe Problem. Die fünf neuen Lade-/Framealter-
Instruktionen waren im Gastcode nachgewiesen; trotzdem keine Reparatur.
Kein RPCS3-Prozess beim anschließenden Nachsehen offen. Keine Live-Messung
des I-Fehlerzustands vorhanden. F bleibt unverändert als guter Rückweg.
Details: `checkpoints/2026-10-09-selective-xmas-lod/I-USER-RESULT.md`.
Nachfolgende Vorbereitungseinträge sind historisch, keine Erfolgsmeldung.

## 2026-10-09: I vorbereitet – Nahmodell-Datenanforderung und Framealter

H live bei schwarzem Boden:1170 registrierte Records,439 jüngst berührte,
nichtgeladene Records außerhalb Ladegrenze, alle320 Queueeinträge leer.
Straßenidentität einzelner Records nicht nachgewiesen. Normal-Messung an
anderer Position; KEIN kontrollierter nur-Blickrichtung-Vergleich.
Neue private H-Kopie I: fünf PPU-Instruktionen in Anforderungs-/Freigabepfad.
Frische unbereite Modelle anfordern; signed Altervergleich schützt aktuellen
Frame vor unsigned -1. Poolgrenzen/Completion/ZCull/4K/Farben bleiben erhalten.
101 statische Checks PASS, Laufzeit/Erfolg noch NICHT bestätigt; F unverändert.
I danach ohne RenderDoc gestartet (PID10980), fünf neue und H-Vergleichsopcode
im laufenden Gastcode exakt rückgelesen; I-RUNTIME-PATCH-PROOF.md. Bild-/Boden-
Stabilität noch NICHT vom Nutzer bestätigt. H zuvor sauber geschlossen.
Desktop **Twisted Metal 4K Ladeabgleich Test** (+ RenderDoc).
Details: `checkpoints/2026-10-09-selective-xmas-lod/I-STREAMING-COHERENCE-TEST.md`.

## 2026-10-09: H FEHLGESCHLAGEN – Boden dreimal nur in derselben Fahrtrichtung schwarz

Nutzer: Bäume/Texturen weiterhin scharf; Straße in einer Richtung schwarz,
Rückfahrt normal; dreimal reproduziert. H-Log bestätigt beide PPU-Testpatches
wirklich angewendet. Zusätzlicher Mesh-Abstandsvergleich behebt es NICHT.
Details: `checkpoints/2026-10-09-selective-xmas-lod/H-USER-RESULT.md`.
F bleibt unverändert. Richtungsabhängigkeit ist Hinweis, kein Ursachenbeweis.
Neue statische Prüfung: Mesh+0x42 ist im untersuchten Pfad Ladebereitschaft,
nicht einfach Sichtbarkeit. Separate Ladeentfernung/5-Frame-Freigabe gefunden.
Read-only Messwerkzeug vorbereitet, noch keine Livewerte: STREAMING-STATE-FINDINGS.md
und probe-streaming-state.py im selben Checkpoint. Keine weitere Spieländerung.
Nachfolgendes „H noch NICHT gespielt“ beschreibt nur den historischen Vorbereitungsschritt.

## 2026-10-09: H vorbereitet – datenbasierter zusätzlicher Mesh-Abstandstest

Neue Auswertung von suburbs.ngp/times.ngp:112/87 HierLOD-Kandidaten mit Mesh-
Prüflisten; echte Grenzwerte632.5/687.5 neben LOD-Distanzenden500/550.
LOD-Auswahlabstand wird globalgenullt, nachfolgender Mesh-Vergleich nutzt aber
räumlichen Abstand. Konkrete Widerspruchshypothese, Ursache NICHT bewiesen.
Private C-Kopie `bin-tree-lod-consistency-test`: zusätzliche Instruktion
3c04e4 `10441dc6`→`10442dc6`, vergleicht Mesh-Grenze mit lebendem Nullvektorv5
statt tatsächlichem Abstandv3; Berechnung/Cachedistanz selbst bleibt erhalten.
Globale Nahdetails bleiben an, keine pauschale Bodenausnahme, Grafik unverändert.
79 statische Checks PASS; H noch NICHT gespielt. F intakt.
Desktop **Twisted Metal 4K LOD-Abgleich Test** (+RenderDoc).
Details/Grenzen: `checkpoints/2026-10-09-selective-xmas-lod/H-LOD-CONSISTENCY-TEST.md`.

## 2026-10-09: G FEHLGESCHLAGEN – Bäume klar, Boden und Häuser fehlen nach Fahrt

Nutzer bestätigt: zunächst gut, dann schwarze Straße und verschwundene ganze
Häuserblöcke. G-Synchronisationsschalter hat den Fehler NICHT behoben.
Log bestätigt Accurate RSX reservation access=true. Kein neuer Capture von G.
G nicht als Erfolg oder stabile Endfassung verwenden. F unverändert bewahren.
Details: `checkpoints/2026-10-09-selective-xmas-lod/G-USER-RESULT.md`.
Weitere read-only Prüfung an spielseitiger LOD-Auswahl/Arbeitslisten; keine
nachgewiesene konkrete Überlastung, keine neue Reparatur behaupten.

## 2026-10-09: G vorbereitet – globaler Nahdetailgewinn plus Reservierungs-Synchronisation

Nach Nutzerpräzisierung wird der Boden NICHT pauschal ausgenommen. Neue private
C-Kopie `bin-tree-neardetail-sync-test`: globaler Nahmodell-Patch bleibt an;
einzige Konfigurationsänderung `Core: Accurate RSX reservation access: true`.
RSX-FIFO Ordered & Atomic war bereits an und ist NICHT derselbe Schalter.
Quellcodebelegte Sperrpfade für Reports/Transfers/SPU-Atomics motivieren den Test,
beweisen aber keine Ursache. Kein Schärfetest, kein E-Draw-Guard, kein Cache gelöscht.
Details: `checkpoints/2026-10-09-selective-xmas-lod/G-SYNC-TEST.md`.
F bleibt die nutzerbestätigte flüssige Fassung ohne gemeldeten schwarzen Boden.
G ist noch NICHT visuell getestet; nicht als Reparatur behaupten.

## 2026-10-09: F vom Nutzer bestätigt – Bodenfehler weg, Weihnachtsbäume fern wieder niedrig detailliert

Verbindlicher Nutzerbefund: Boden-Isolation läuft richtig gut, Grafikfehler weg;
leuchtende Bäume im Schneelevel wechseln in der Ferne wieder auf schlechte Details.
F ist damit die neueste nutzerbestätigte stabile Vergleichsfassung. F NICHT
überschreiben! Ergebnis unterstützt Nebenwirkung des globalen Nah-LOD-Zwangs;
keine Behauptung, jeder mögliche Aussetzer sei mit einem kurzen Test ausgeschlossen.
Nutzerauftrag: Weihnachtsbäume gezielt nah detailliert halten, ohne den Bodenfehler
zurückzubringen. Globale Faktoren0/x4/x64 dürfen nicht als baumspezifische Lösung
wieder eingeschaltet werden. Objekt-/Asset-Zuordnung wird untersucht.

## 2026-10-08: Neuer Auftrag / Paketvergleich / LOD-Isolation F vorbereitet

Nutzer setzt Arbeit nach Übergabe fort; Verdacht fehlender Paket-Einstellungen.
EXE/DLL/Flags von Gemini und Baumfassung C identisch, Grafikconfigs und sechs
TM4K-Startparameter gleich. Kein nachgewiesen vergessenes Modul. Zentraler
Unterschied: GLOBALER Nahmodell-LOD-Patch, nicht baumspezifisch. Neuer Replay der
Gemini-Referenz4453 zeigt gültige Bodentiefen an den Vergleichskoordinaten, anders
als Schwarzboden3505; Kameras verschieden, kein vollständiger Ursachenbeweis.

Private Fassung F `bin-tree-lod-isolation-test`: exakte C-Kopie, einzig Nullabstand-
Patch deaktiviert. Originalfaktor1.0, x4 aus; originale Nebelwerte/Blur aus/4K/
Feuer/Farben/Cache erhalten. KEIN E-Draw-Guard. Desktop Boden-Isolation und
RenderDoc. 107 statische Checks PASS, KEIN Spielstart/visuell bestätigter Fix.
Ferne Baumdetails können dabei wieder umschalten: F ist gezielter Gegencheck,
nicht fertiger Baumfix. Alle alten Versionen und gesicherte Übergabe erhalten.
Details/Skripte/Beweisgrenzen in xmas-lights-stability/packaging-audit/README.md.
Neuester Nutzerbefund bleibt E fehlgeschlagen, bis F wirklich getestet wurde.

## 2026-10-08: E vom Nutzer als FEHLGESCHLAGEN bestätigt; Übergabe / Tagesende

Verbindliches Nutzerergebnis: schwarzer Boden bleibt zeitweise bestehen,
zusätzlich starke Ruckler. E (Boden-Zeichenfix / TM4K_DRAW_GUARD) ist KEIN
erfolgreicher Boden-Fix und KEINE beste Fassung. C bleibt die zuvor bestätigte
flüssige Baumfassung, aber auch C hat schwarzen Boden. Seit Beginn der
Baumarbeiten wurden schwarze/weiße Aussetzer gemeldet; zeitlicher Zusammenhang
ist dokumentiert, eine alleinige konkrete Ursache noch nicht bewiesen.

Keine weiteren Tests heute. Kein RPCS3-Prozess bei Übergabeprüfung vorhanden.
Übergabe im Desktopordner Übergabe Twisted Metal; zuerst 00_START_HIER.md lesen.
Unterordner2026-10-08_Baeume_Schwarzboden enthält aktuellen Bericht, Checkpoints,
Original-Capture3505, GPU-Exporte/rohe Tiefendaten, Source-Snapshot/Git-Diff,
Launcher, EXEs/Konfigurationen/Level-Overrides und letzten E-Lauf inklusive
Prüfmanifest. Alte Sichtweite-Dokumentation dort ist historischer Stand7.Oktober,
kein aktueller Erfolgshinweis. Nichts gelöscht, alte Startlinks unverändert.

## 2026-10-08: Aktuelle Boden-Geometrie fehlt; Zeichenbereitschafts-Guard E

Read-only Replay Frame3505: Tiefenpixel an vier schwarzen Straßenstellen bleiben
auf1.0 (gelöscht), vor/nach Farbpass. Viewports in beiden Durchläufen3840x2160
mit Origin0,0. Straße in Materialkopie ist Altinhalt, kein aktueller Drawbeweis.
Sicheres Skript trace-ground-passes.py / pass-trace.json / rohe D32S8-Werte.
Depth-PixelHistory ließ das Replay-Werkzeug abstürzen, kein laufendes Spiel;
stattdessen sichere Snapshots benutzt. Keine Behauptung, sämtliche Tiefen-/
Shader-/LOD-/Sichtbarkeitsursachen seien dadurch endgültig ausgeschlossen.

Separater E-Sourcebuild bin-tree-draw-guard-test: Opt-in TM4K_DRAW_GUARD=1,
Pipeline-Misses im Zeichnen synchron kompilieren, für bestehende Async-
Nullplatzhalter maximal2s/1ms-Pausen warten und Ergebnis/Timeout protokollieren.
Konkreter bisheriger Skip-Draw-Pfad abgesichert. Warme Cachehits unverändert.
KEINE neue LOD-/Farb-/Fog-/Depth-/Fusion-/Mipmap-Einstellung, C-Konfiguration
inkl. bestätigtem Nahbaumfaktor0.0 exakt übernommen. Alle alten EXEs bleiben.
Ziel: Twisted Metal 4K Boden-Zeichenfix, eigene RenderDoc-Verknüpfung.
Dies ist ein gezielter Kandidat, noch KEIN visuell bestätigter Boden-Endfix.
Build/Abnahmefortschritt und Quellstand:
checkpoints/2026-10-08-xmas-lights-stability/draw-guard-test/.
E fertig gebaut/geladen (EXE--version0),32 Dateischutz-/Konfig-/Buildprüfungen
PASS, beide Launcher-CheckOnly PASS und Desktoplinks erstellt. Normalstart
ohne RenderDoc 02:33:12, PID16368; Quelle/EXE-Hash im E-README. Match-Ergebnis
ausstehend, keine schwarze-Boden-Freiheit aus Prozess/FPS/Route abgeleitet.

## 2026-10-08: Boden-Test D negativ; LOD-Wert nicht alleinige Ursache

Nutzer: Spiel flüssig, Straße wird weiterhin zeitweise schwarz. Änderung von
Null-Abstand auf d/64 hat den Fehler nicht behoben. Kein erfolgreicher Fix.
Geschützte Revision C unverändert. Weitere Untersuchung am vorhandenen echten
Schwarzboden-GPU-Frame3505: Render-Draws, Tiefe und Herkunft kopierter MRT-Flächen;
vorhandene Straße in einem Materialpuffer kann auch aus einem älteren Frame
stammen und beweist alleine keine erfolgreiche aktuelle Bodendarstellung.

## 2026-10-08: Boden in Material-MRT vorhanden; separater Boden-Test D

Frame3505: MRT12064/17028 enthalten die Straße; spätere Szene/Farbe/Display
schwarz. Kein reiner letzter 4K-Fusion-/Farbfilterfehler. Alias-/MRT-Eigenheiten
verhindern bisher eindeutige Draw-Ursache. ZCull nicht blind deaktiviert.
Separater D-Test `bin-tree-ground-guard-test`: nur globalen Nahmodell-Faktor
0 ->1/64 geändert, um Nullwert-Sonderfall zu prüfen. Normaler Async/4K/Feuer/
Originalnebel identisch. x4 und0 deaktiviert, x64 aktiv. Keine finale Fixbehauptung.
Nahdetails-Test C und gute Referenzen unverändert. Neue Verknüpfung
**Twisted Metal 4K Boden-Test** / **- RenderDoc**.16 statische Prüfungen PASS.
Details: `checkpoints/2026-10-08-xmas-lights-stability/ground-guard-test/README.md`.

## 2026-10-08: Schwarzer Boden erfolgreich als GPU-Frame 3505 erfasst

`captures/tree-lights-stable-renderdoc/2026-10-08_01-51-03-139-6532f626/tm-tree-lights-stable_frame3505.rdc`
847335316 Bytes; RenderDoc OpenFile UND Vulkan Replay erfolgreich. 1482 Actions,
241 Texturen, letzter Event7050. Keine bloße Screenshot-Datei. Exportiertes
4K-Finalbild ResourceId63871 zeigt den gesuchten schwarzen Boden bei sichtbaren
Fahrzeugen/Gebäuden/HUD. Prüfskript und result.json samt unverfälschten PNGs:
`checkpoints/2026-10-08-xmas-lights-stability/ground-capture3505/`.
Damit ist der Fehler für die weitere Schicht-/Drawanalyse reproduzierbar
aufgenommen; Ursache dadurch noch nicht automatisch geklärt.

## 2026-10-08: Revision C vom Nutzer flüssig bestätigt; Boden gelegentlich schwarz

Nutzer: flüssig/deutlich besser, leuchtende Nahbäume weiterhin korrekt, Fehler
deutlich seltener. Restfehler: Straße/Boden vollständig schwarz, übrige Szene
sichtbar. Screenshot und EXE/Config/Patches/Log gesichert in
`checkpoints/2026-10-08-xmas-lights-stability/C-smooth-trees-black-ground/`.
69 neue Laufzeit-Shader, Async-Skip-Draw mögliche aber NICHT bewiesene Ursache.
Keine Änderungen am bestätigten Baum-Faktor oder 4K/Farben/Feuer. Nächster
Diagnoselauf dieselbe Revision C mit RenderDoc, F12 gezielt bei schwarzem Boden.
Nur GPU-Einzelframe; kein vollständiges Video voraussetzen.

## 2026-10-08: Leuchtende Schneebäume bestätigt; Stotter-/Schwarzbildtest C

Nutzer bestätigt Revision B: geschmückte leuchtende Bäume bereits in Entfernung
voll detailliert. Nahmodell-Faktor 0.0 ist hierfür jetzt visuell bestätigt.
Aber starke Stotterer und schwarze Aussetzer beim Blick auf Häuser verbleiben.
Revision B inkl. EXE/Config/Patches/Log gesichert in
`checkpoints/2026-10-08-xmas-lights-stability/B-xmas-near-confirmed/`.
200 Shader wurden im Lauf synchron kompiliert; KEIN erneuter Semaphore-Timeout.
Nur ein später Route-0-Wechsel; Ursache mehrerer Schwarzbilder noch ungeklärt.
Revision C ändert NUR Shader Mode zu normalem `Async Recompiler (multi-threaded)`
mit erhaltenem gefüllten Cache. Nahmodell-0/4K/Feuer/Nebel bleiben unverändert.
Desktop weiterhin **Twisted Metal 4K Nahdetails-Test**. Performance und
Schwarzbildfreiheit müssen überprüft werden; keine stabile Endfassung behaupten.
Revision C ohne RenderDoc gestartet 01:45:54, PID 10608; Patches/Modus geladen,
24 statische Prüfungen PASS. Sichtbare Match-Ergebnisse noch ausstehend.

## 2026-10-08: Nahdetails-Test A schwarz; Revision B ohne Shader-Interpreter

Nutzer bestätigt KEINEN erfolgreichen Start: langes Laden, Titelbild, schwarz,
dann selbst Task beendet. Fehlversuch vollständig in
`checkpoints/2026-10-08-xmas-lights-stability/failed-interpreter-start/` gesichert.
RSX Semaphore-Timeout 0:08:08 vor Beenden; Ursache noch nicht eindeutig isoliert.
Interpreter meldet nicht unterstützte Vertex-Texturen. Nur privaten Shader Mode
auf `Legacy Recompiler (single-threaded)` geändert: Compiler wartet auf fehlende
Programme statt Interpreter/Skip-Draw. Nahmodell-Distanz 0 bleibt zum Test erhalten.
Kein Cache gelöscht, kein globaler Treiber-/Sicherheits-/Grafikeingriff.
Gute Sichtweite- und Bäume-x4-Fassungen unverändert. Desktop weiterhin
**Twisted Metal 4K Nahdetails-Test**. Revision B: Start/Match noch nicht abgenommen.
24 statische Prüfungen PASS; ohne RenderDoc gestartet 01:40:02, PID 7164.
Nach 3 s Fenster/erwarteter Shader-Modus/Patches vorhanden, visuelles Ergebnis offen.

## 2026-10-08: Separater Nahdetail-Nullabstand + Shader-Stabilitätstest umgesetzt

Neue Desktop-Verknüpfung **Twisted Metal 4K Nahdetails-Test**, zusätzlich
**- RenderDoc**. Private Fassung `bin-tree-lights-stable-test`, ursprüngliche
Sichtweite-Fassung und bestätigter normale-Bäume-x4-Test bleiben unverändert.
Float bei 0x0010e83c im privaten Patch auf 0.0: effektiver Modell-LOD-Abstand
ist null. x4-Enable dort aus, alle anderen Patches/4K/Feuer/Farben erhalten.
Globaler Nahmodell-Test, noch NICHT als Weihnachtslampenfix verifiziert.
Mehr Draw-/Modellkosten möglich. Kein Schärfe- oder Texture-Mip-Bias-Eingriff.
Shader-Interpreter-Ersatz während Kompilierung aktiviert, Snow DistanceBlur aus,
Originalnebel erhalten. 23 statische Prüfungen und beide Launcherchecks PASS;
EXE-Loader Exitcode 0. Tatsächliche Patch-Anwendung im `startup-proof.json`.
Neuer Test gestartet; Match-Stabilität und Weihnachtslicht-Qualität noch offen.
Read-only GPU-Exporte weisen Kronen in separatem Pass nach, nicht im Baum-Guide;
die allgemeine Feuer-Restaurierung enthält diese Kronen nicht. Kein Material-
Patchoffset erfunden. Vollständiger Zwischenstand:
`checkpoints/2026-10-08-xmas-lights-stability/README.md`.

## 2026-10-08: Normale Bäume bestätigt; Weihnachtsbaum-Nahdetails noch offen

Nutzer bestätigt sichtbare Verbesserung entfernter normaler Bäume durch den
Modell-LOD-x4-Test. Geschmückte Bäume/Lichter im Schneelevel wechseln dagegen
weiter beim Annähern; zusätzlich kurze weiße/schwarze Bildaussetzer gemeldet.
Verbindlicher Auftrag: Nahdetails der geschmückten Bäume früher auswählen,
NICHT nachschärfen oder lediglich Mipmap-Bias ändern.

Bestätigte Fassung und Sichtweite-Referenz unverändert gesichert. Separater,
noch NICHT gestarteter Stabilitätskandidat mit Shader-Interpreter-Brücke und
nur Snow-Level DistanceBlur aus vorbereitet. Mipmap-Bias bleibt 0.0.
Kein fertiger Weihnachtsbaum-Fix, keine neue Verknüpfung behauptet.
Originales times.psarc entpackt, tree_xmas-Namensregistrierung nachgewiesen;
konkreter Lampen-/Baum-LOD-Pfad noch nicht zugeordnet. Vorhandenen Schneeframe
read-only geprüft; Copy/Clear-Pixelhistory liefert noch keinen Materialbeweis.
Details, Schutzmanifeste und offener nächster Schritt:
`checkpoints/2026-10-08-xmas-lights-stability/README.md`.

## 2026-10-08: Baumdetails / Modell-LOD x4 und Originalnebel, separater Test

Geminis Übergabe auf dem Desktop vollständig geprüft. Gute Referenz
**Twisted Metal 4K Sichtweite** / `bin-streaming-lod-test` bleibt unverändert.
Private Testfassung `bin-trees-original-fog-test`, dieselbe EXE
`697ABC3A7F457A31093850918A52FB7EE386E17A924A666F711AA489DE3B0F72`.
Neuer globaler HierLOD-Reset-Faktor: Float bei `0x0010e83c` von 1.0 auf 0.25.
Nachgewiesener Resetaufruf `0x0010e8a4 -> 0x003bd26c`, Quadrat bei `0x00f51bf8`,
LOD-Verbrauch u.a. `0x003c06cc/0704/0764/0768`. Rechnerisch Detailradius x4;
Baum-spezifische Wirkung und FPS noch im Match zu verifizieren. Kein
unendliches Vorladen, keine neue Texturauflösung, keine Kamera-Verschiebung.
Erfolglosen Gemini-Vektorpatch NICHT wiederverwendet.
Nur privates Suburbs-LTN: Fog Density 1.000237 / Extinction Rate 2.049999
exakt aus Originaldisc, Distance Blur bleibt 0. Andere Levels unverändert.
Bestehende 10000/16000-Konstanten, 4K/Mode7/Farben/Feuer/Performance erhalten.
34 statische Prüfungen PASS, Schutz-/Testmanifest vorhanden.
Desktop **Twisted Metal 4K Bäume + Nebel Test**, zusätzlich **- RenderDoc**;
Launcher `Start-TreesOriginalFog-Test.ps1`.
Vollständige Belege/Reproduktion/Abnahmekriterien:
`checkpoints/2026-10-08-trees-original-fog/README.md`.
Live-Startnachweis separat; sichtbare Qualität/FPS nicht vorweggenommen.
Startnachweis ergänzt: neue Patches im Loader angewendet, aktive 4K Route3
und Zuschnitt bestätigt. Spiel vom Assistenten beendet; zweiter Shutdown
meldet SpuPostFX-Speicherfehler erst NACH Beenden-Anforderung. Derselbe Typ
bereits im Log der unveränderten Sichtweite-Referenz vorhanden. Kein sauberer
zweiter Shutdown behauptet; Protokolle im Checkpoint gesichert.

## 2026-10-05: Fünf Farbmaster als getrennte Live-Fassung fertig

Nutzer gibt ausgewählten Dreipass-Farbmaster zum Einbau frei.
Mode7 ergänzt genau einen begrenzten Konsistenzpass (Gain0.5..2)
nach bestehenden Gain32/Gain8-Pässen. Feste Basisführung, keine UV-/Timing-/
UI-/Feueränderung. Vorhandene Puffer wiederverwendet, keine neuen 4K-Framebuffer.

Checkpoint `checkpoints/2026-10-05-final-five-live/README.md` enthält Quellen,
Buildskripte, Shaderprüfung, GPU-Nachweise, Startproof und Lieferung.
16 Shader PASS. GPU-Vergleich aller fünf Szenen PASS, jeweils sämtliche
8.294.400 Szenenpixel höchstens 1/255 Kanalabweichung.

Neue private EXE `bin-final-five-master-test/rpcs3.exe`, SHA256
`AF3D985E85E3B4577181326689860171E62BC8E8B69F8FF2E8BB1E2FA9740976`.
Desktop **Twisted Metal - 4K Final Master**, zusätzlich **- RenderDoc**.
Launcher `Start-FinalFiveMaster.ps1`, env TM4K_MASTER_DENSITY=7.
Private Konfiguration/Cache von guter Mode6-Fassung übernommen; vorhandene
per-EXE-GPU/HDR-Präferenz kopiert, keine globale Änderung.

Live-Grafikthread meldet Mode7, reagiert bei kurzem Starttest, keine erkannten
fatalen Shader-/Device-Lost-Fehler. Danach vom Assistenten normal geschlossen.
Langzeit-Match-/Bewegungs-/FPS-Abnahme noch offen. Starttitel ca.52–60 FPS
ist kein Beleg für stabile60 FPS in allen Levels.
Gute **4K Farben Master**-EXE/Links und drei geschützte Vorgänger unverändert.


## 2026-10-05: ChatGPT/Gemini-Mastervergleich

Fünf Gemini-Outputs vorhanden. Pixelgetreue mittlere Ausschnitte links ChatGPT,
rechts Gemini unter Desktop/Gemini Farben Master Vorarbeit/vergleich-chatgpt-gemini.
Script `compare_five.py`, Messwerte/aktuelle Hashes `pixelvergleich.json`.
Vollständige RGB-Bilder: abweichende Pixel 42/79/160/188/0 für
Frames 3075/3665/3785/4481/5129. Helles Level vollständig pixelidentisch;
übrige mindestens 99.997733 Prozent identisch. Gemini nutzt für die finalen
Outputs dieselbe Dreipass-Farbformel. Kein visuell begründbarer Qualitätsgewinner.
Originalmaster, Spielcode und Launcher unverändert. Kein neuer Live-Test.


## 2026-10-05: Fünf Offline-Farbmaster und Gemini-Übergabe

Nutzer meldet die reparierte 4K Farben Master-Fassung als bisher beste Fassung,
flüssig und ohne Hänger. Live-EXE in dieser Vorarbeit nicht geändert; SHA256
weiterhin `9E3B191749DDFDC523B33D351E0FF6F995F59BA743984BE3A3E713BDCFECB4BA`.

Vier ursprüngliche Captures 3075/3665/3785/4481 exportiert und reproduziert.
Wichtige Zuordnungskorrektur: 3785 ist ebenfalls dunkle Arena, NICHT helles Dorf.
Nachgelieferter Capture 5129 vom 05.10., 12:28 Uhr ergänzt das helle Level.
Alle fünf Export-Inventare PASS, jeweils reale Ressourcen/Event-IDs dokumentiert.

Übergabeordner: `C:\Users\box3\Desktop\Gemini Farben Master Vorarbeit`.
`exports/`: Schichten; `masters/`: Baseline, Konsistenzvariante, Vergleich und
Metriken; `final-masters/`: ausgewähltes Fünfer-Set 3840x2160, zusätzlicher
begrenzter Konsistenzpass. Farbmittel-Fehler sinkt bei allen fünf Frames;
kein Nachweis von 100 Prozent korrekter Originalbeleuchtung oder neuen Texeldetails.
Statische Offline-Bilder, KEIN neuer Live-Patch oder FPS-/Timing-Nachweis.

`reproduce.py`, `tm_core.py`, `recipe.json`, `export_four.py`, `status.json`
und `STATUS.md` sind vorhanden. `README_GEMINI.md` plus
`AUFGABE_2_FUENF_MASTER.md`: unabhängige fünf Gegenstücke in `output-gemini/`,
Quellen/Parameter/Code speichern, Live-Fassung unberührt lassen.
Replay funktioniert versteckt mit `--python`, absoluten Pfaden und einem
Capture pro frischem Prozess; Exporter benötigt kein `__file__` oder renderdoc.pyd.


## 2026-10-05: Farben-Master Startfehler eindeutig gefunden

Erster Nutzertest haengt beim Laden. Log: RSX `version not supported`,
`#version 45` und `#define NATIVE_SRGB 10`; danach fataler Compute-Compilefehler.
Die statische Bildpruefung war richtig, deckte jedoch das fehlerhafte
Zusammensetzen des Shadertextes im C++-Livepfad nicht ab.

Zwei Define-Einfuegungen hatten den fuehrenden Zeilenumbruch des Rawstrings
ignoriert. Reparatur in VKPresent.cpp: Einfuegung nach dem gefundenen
`#version 450`-Token, nicht fester Offset12.
Validierung reproduziert nun die Laufzeit-Einfuegung beider Defines.
15 Shader und beide 8.294.400-Pixel-GPU-Vergleiche erneut bestanden.
Keine neue Bildformel, keine Cache-/PS3-/Treiber-Aenderung.

Beleg/Quellrueckweg/Baureparatur:
`checkpoints/2026-10-05-native-colour-start-fix/README.md`.
Finalen Reparaturstatus nur aus dessen `build-status.json` und anschliessendem
Live-Startbeleg lesen. Die drei alten funktionierenden Fassungen bleiben
unveraendert. Links des Farben-Masters werden nach Reparatur weiterverwendet.

Reparatur fertig gebaut, neue EXE SHA256
`9E3B191749DDFDC523B33D351E0FF6F995F59BA743984BE3A3E713BDCFECB4BA`.
Tatsaechlicher Start durch Launcher erfolgt: Mode6-Meldung bei 17.905710s,
keine `version not supported` / Compute-Compilefehler im neuen Startlog.
Prozess antwortet, Fenstertitel zeigte FPS50.0; Spiel offen gelassen.
`live-start-proof.json` und `live-start.log` im Reparaturcheckpoint.
Das bestaetigt den Start, nicht die Qualitaet/Stabilitaet in einem Match.

## 2026-10-05: Freigegebene 4K-Masterbilder als Zwei-Pass-Farben-Test

Zwei aktuelle Captures (frame4841 Fahrzeugauswahl, frame5419 Gameplay)
als Offline-Master berechnet und vom Nutzer zur Umsetzung freigegeben.
Quelle: `checkpoints/2026-10-05-colour-layer-audit/reconstruct.py`;
Bilder und Parameter: dort `masters/`. Nicht generativ, kein CAS.

Neuer isolierter Mode6: bestehende Mode5-Szenenbasis plus zweifacher
kantenbewusster log-RGB-Abgleich gegen native 3x3-Farbmittelwerte.
Epsilon 0.0005, Range-Sigma 0.05, Spatial-Sigma 0.85,
Gain-Grenzen 1/32..32 und 1/8..8.
UI, Logos und Partikel separat; kein neuer temporaler Bildspeicher,
kein UV-Versatz. Output 3840x2160 = 8.294.400 Pixel.
Dies bezeichnet die Ausgabeflaeche, nicht neue Quelltexel pro Fahrzeugteil.

15 Shader kompiliert/SPIR-V-geprueft. Beide echten Szenen mit den neuen
Shadern auf RX7900XT ausgefuehrt: alle 8.294.400 Szenenpixel jeweils
hoechstens 1/255 Abweichung vom freigegebenen Offline-Master.
Keine FPS-, HDR-, Start-/Menue- oder Langzeitabnahme daraus ableiten.

Checkpoint `2026-10-05-native-colour-live/README.md`, dort
`gpu-validation.json`, `build-status.json`, `source-before`, `source-used`,
nach erfolgreichem Finish `delivery.json` und `shortcuts.json`.
Neue private EXE `bin-native-colour-master-test/rpcs3.exe`;
Launcher `Start-NativeColourMaster.ps1`; Schalter `TM4K_MASTER_DENSITY=6`.
Desktop-Namen **Twisted Metal - 4K Farben Master** / **- RenderDoc**.
Fertig gebaut und beide neuen Links angelegt; Compile/Link/Loader und
Launcher-CheckOnly (normal und RenderDoc) bestanden.
Neue EXE SHA256:
`10418EDBE512B328C0B888A100A18FAE297EF2B8EA25081FD49AAD836BCE219D`.
`delivery.json` vorhanden, Status ready-for-user-test. Spiel nicht gestartet.
Fertigstellung ausschliesslich nach `delivery.json` beurteilen,
Live-Spieltest bleibt danach offen. Drei funktionierende Alt-EXEs und
deren Verknuepfungen bleiben SHA256-geschuetzt und unveraendert.

## 2026-10-04: Nutzer bestätigt Hybrid Master als beste Fassung; Aufräumen

Nutzerbericht: „bis jetzt die beste Version“. Keine neue gemessene FPS-/HDR-
oder Langzeitabnahme daraus ableiten. Auf Nutzerwunsch aktuelle Hybrid-Fassung
und genau zwei direkte Vorgänger behalten: affine-master und colour-detail-repair,
jeweils normales Desktop-Icon plus RenderDoc-Icon.
Alte RenderDoc-RDCs (auch Kopien bei Gemini/Aufgabenzettel), ältere Test-EXEs/DLLs,
deren private Caches, historische EXE-Kopien und alte Desktop-Screenshots werden
endgültig entfernt. Maßgeblicher tatsächlicher Löschbericht:
`checkpoints/2026-10-04-cleanup/result.json` und `removed.json`; Fehler in `failed.json`.
Quellcode, Rezepte, Logs, Messwerte, erzeugte Referenz-Masterbilder/Exporte und
Spiel/Spielstände bleiben. RDC-basierte historische Reproduktion benötigt künftig
neue Aufnahmen; alte Manifestpfade sind nach der bewussten Löschung historisch.
Die drei aktiven Launcher prüfen ab jetzt `2026-10-04-cleanup/retained-exes.json`
statt alle acht längst überholten EXEs vorauszusetzen. EXE/Shader unverändert.
Shared Basis-bin und Junction-Ziele nicht gelöscht oder rekursiv verfolgt.

## 2026-10-04: Freigegebene Palette + Gemini als separater Hybrid-Modus5

Ausgeliefert: Desktoplinks **Twisted Metal - 4K Hybrid Master** und
**Twisted Metal - 4K Hybrid Master - RenderDoc**; beide Launcher CheckOnly PASS.
Finale EXE SHA256 `875EF604532EE51159A7C99B96BA66923D26B526CA539D6D44BCF20DE6688CB2`.
Compile/Link/Loader, Shader, sechs GPU-Bilder und Palette-Referenz PASS.
`delivery.json` / `shortcuts.json` sichern den Release, Status ready-for-user-test.
Kein Spiel gestartet, alte acht geschützte EXEs unverändert; Liveabnahme offen.

Nutzer gibt neueOffline-Palette frei undGeminis06StadtSepia, verlangtstartbarenLink.
Checkpoint `2026-10-04-hybrid-master-live/README.md`, neue TM4KHybrid.hpp mit8Shadern,
separateEXE bin-hybrid-master-test, LauncherStart-HybridMaster.ps1.
Auslieferungsstatus AUSSCHLIESSLICH build-status.json / delivery.json; keinSpiel
gestartet, keine60FPS-/Perfektions-/Bewegungsabnahme. Alte8EXEs SHA-geschützt.
Palette-Methode auf720Arbeitsraster fürandereSzenen; Sepia-Gameplayheuristik
Gemini sRGB-Radius1 GainPrior/kleinerFit+lineareKonsistenz, volle4KGuidepixel.
NativeDisplay-ECHTESAlpha rekonstituiert UI/HUD/Pause stattgrober3D-Menürechtecke;
Crop118,73.5/3606,2017 kubisch; nativeLogoheuristik, Feuer/Timing/CPU-Patch unverändert.
NativeAtmosphäre/Bias bleibt, keineCAS/generativenDetails/pauschaleNebelentfernung.
Neu gefunden: FloatFloor des3:1Raster bei x%3==1verschiebt4x4Tapfenster,
553Pixel>32/255u.a.NeonFilmrolle im erstenPort. Integerdekomposition inPredict+
ResidualMerge ip=(p+2)/3-1,pos=ip+((p+2)%3)/3 behebt das.
GPU4746 gegenGeminisfreigegebene06: alle8.294.400Pixel maximal1/255, P99=0,
MAEmaxRGB0.0001226128/255. ZweitesSepia8262 ebenfallsmax1/255.
Palette3474/3676/6592/9095 gegen unabhängigeCPU: max1/2/2/1, jeweilsP99=0.
Alle6echtenCaptures GPUdurchgerechnet, echteShaderstringsglslc+SPIRVvalid.
WichtigerAudit: Geminis„exact3x3NearestPROVEN“-WholeSurfaceThese NICHT bestätigt:
NativeColor nur79.7–88.8%identisch;720 hierAlgorithmusarbeitsraster, nichtexklusive
NativePufferquelle behaupten. input-proof.json dokumentiertQuellevsBildparität.
VorRasterfixBinary gesichert, finalerneugebaut; source-before/source-used/GeminiCode
undRezept eingefroren. Alte4K-Verknüpfung bleibt unverändert. KeinSaves-/DisplayReset.

## 2026-10-04: Gemini-Bildauswertung bereit, neue Kopfsaum-Vergleiche

Desktopordner `Gemini Twisted Metal Bildauswertung`: sechs echte RDC-Kopien
SHA256-geprüft, alle sechs Exporte inklusive neuem4746, Code/Rohdaten/Parameter,
vollständiger unabhängiger Auftrag; Nutzer beauftragt Gemini selbst.
Keine Nachricht an anderen Agenten gesendet. Gemini nur `output-gemini`, ChatGPT
in `checkpoints/2026-10-04-gemini-bildauswertung`. Handoffmanifest READY.
Neue Aufnahme4746 = helles Dorf/Diner/Filmrollen/Roadkill, finalMode4 event6628.
Scene17004 RGB8 3840x2160, Color17398/Display16396 BGRA8, Atlas17001 FP32.
Vier neue Download-Crops geprüft/kopiert: Scene-Crop bitgenau Rechteck
1800,1000/2600,1500; feiner Guidekopf/Waffe vs goldener Saum im tatsächlichen Merge.
Eigene neue Offline-Farbübertragung mit Guide+NativeColor-Nachbarschaftsgewichten:
SigmaColor0.035/0.075/0.15 direkt: Kopf besser, Straßenfarbmuster zu grob.
PaletteRadius1 (3x3), sigma0.075, eps5e-5: fünf vollständige4K-Kandidaten
4746/3676/6592/8262/3474 plus Gesamtansicht, Felder, UI-Masken, Hashes/100%-ROIs.
Kopf/Waffe4746 deutlich sauberer; Saum8262 und grobe native Straßenfarbe bleiben.
PriorGegenprobe eps5e-4/PaletteRadius3: mehr örtlicher Kontrast, aber neue dunkle/
rote Randstrukturen; nicht als universell bessere Lösung gewählt. Alles dokumentiert.
Pause9095 bleibt akzeptierte alte Referenz. Noch KEIN perfekter Bild-/Live-Fix.
RechenPASS nur Datei-/Pixelgültigkeit, kein Qualitäts-/Bewegungs-/FPS-Beweis.
Kein Spiel gestartet; EXE E61B7CC051D309FDCD7FC0A1318D2745044F931CA4D3FADE8C86D5CD3EB8DA72
und Shader/Settings/Spielstände/Launcher/Links unverändert. Alter4K-Link bleibt.

## 2026-10-04: Neue Menü-/Atmosphäre-Master, fünf tatsächliche neue Captures

Nutzer bestätigt guten Live-Modus4, meldet Sitz durch Story-Balken, Auswahl-
Rechtecke/fehlende Schattierung, Nah-Säume und Pause. Auftrag ausdrücklich zuerst
Masterbilder, KEIN neuer Live-Build. Neue RDCs3676/6592/8262/9095/3474 alle PASS:
Story-Menü, Autoauswahl, hellesLevel, Pause, dunkleArena; keine alte Referenz benutzt.
Checkpoint `checkpoints/2026-10-04-affine-menu-atmosphere-previews/README.md`:
RGB/FP32/FP16-Rohdaten, Inventare, Skripte, Parameter, fünf vollständige3840x2160
Master-PNGs, Vergleichsbilder, Galerie, Capture-/Output-Hashes und EXE-Schutz.
Live-Story-Rechteck endet x1280 mitten im Balken; anderes großes Menü-Rechteck
vergröbert3D-Hintergrund. Frame6592 trotzselectionscore0.664 alsGame0 erkannt;
schwarzerMindestanteil zu streng. Pause hat keinen eigenenUI-Zustand im Live-Code.
Offline engerer realerUI-Schutz plus RGB-affinerFit mit Guide-Farb-/Abstandsgewichten,
edge-awareGlättung/Upsampling16Nachbarn, Bias für nativeAtmosphäre. OriginalesFeuer
separat erhalten auchüber schwarzerSkyfläche. Pauseframe gemessen557,193/3262,1627,
schwarzeTransmission~0.1432992sRGB plus nativeForeground/UI; Alpha-Schätzung!
Berechnung der restlichenBilder lief trotz Verbindungsunterbrechung fertig.
Fünf Master visuell geprüft; keinePerfektions-/Fotorealismus-/LiveFPS-Zusage.
Aufhellung bereitsim nativenFarbbild, nichtalles falscherNebel; keineDepthFog-Steuerung
bewiesen. Zusätzlicher hellerPrior-Test epsilon0.0005 nichtzumStandard erklärt.
8.294.400 sindCanvas-/Ausgabepixel, nicht8.29MproAsset/kein8.1MStellregler.
Mode4EXE E61B7CC051D309FDCD7FC0A1318D2745044F931CA4D3FADE8C86D5CD3EB8DA72
unverändert; keineShader/Settings/Spielstände/Launcher/Links geändert, keinSpiel gestartet.
VorLiveeinbau echteUI-/Pause-Erkennung, Alpha-Zuordnung, GPU-Parität/Frametimes prüfen.

## 2026-10-04: Freigegebene Vorschau-Berechnung als separater Live-Modus4

Auslieferung abgeschlossen: ready-for-user-test, EXE SHA256
E61B7CC051D309FDCD7FC0A1318D2745044F931CA4D3FADE8C86D5CD3EB8DA72.
Compile/link/LoaderExit0,5GPUFramechecks,beideLauncherCheckOnly,2DesktoplinksPASS.
KeineLiveAbnahme/keinSpielstart. Portable.NET-SHAprüfung im neuenLauncher,
weilWindowsPowerShell-AutoloadGetFileHash imFinalisierer fehlgeschlagen war.

Checkpoint `2026-10-04-affine-master-live/README.md` plus neue TM4KAffine.hpp.
Ganz neuer FP32-Linearlicht-Farbzusammenbau: BOX1280x704,7x7 affineRGB-Regresssion
eps5e-5 slope0.05:1.5, Bias,5x5Feldglättung, pixelweise3840x2160Auswertung.
ExakteOffline-UI-Rechtecke plus frame-lokaleLogo/Menü/Auswahl/Game-Heuristik.
NativeDeckel/Schrift, Fire/Partikel/Timing/CPU-Patch bleiben eigenständig erhalten.
KeineKI-erfundenenTexel, keine8.29MjeAsset-Behauptung und kein weiteresSchärfen.
Fünf tatsächlicheVulkan-GPU-Tests jederAusgangspixel PASS:99.54–99.78% identische
RGB8Pixel,99.Perzentil0/255, wenige dokumentierte HalbbyteSchwarzSchwellen-Rundungen.
VierneueRDCs+eineausdrücklichälterehelle3543Referenz, keineLive-Abnahme.
NeuerBin `bin-affine-master-test`, Launcher `Start-AffineMaster.ps1`, neueDesktoplinks
`Twisted Metal - 4K Master Rekonstruktion` normal+RenderDoc NACH erfolgreichemBuild.
SiebenbestehendeEXEs geschützt. BuildStatus/DeliveryJSON sind maßgeblich!
ZweiMSVC-LinkerabstürzeLNK1000 undein glslc-Absturz protokolliert; erneuterBuild
mitOPT:NOICF/NodeReusefalse. KeinTreiberupdate/keinSpielstart durch diesePrüfung.
LiveFPS/Farben/Feuer/Menüwechsel noch durchNutzertest abnehmen; nicht"perfekt"behaupten.

## 2026-10-04: Fünf komplette 4K-Vorschauen, noch kein neuer Live-Fix

Nutzer meldet bessere Fahrer/Lichter, flüssig, aber falsche Farben und Deckel-Leck;
fordert zuerst vollständig neu berechnete Gesamtbilder, auch Clown und Umwelt.
Checkpoint `2026-10-04-five-master-previews/README.md`, reproduzierbares Skript,
recipe.json, affine-fields und fünf 3840x2160-PNGs unter masters.
Vier neue RDCs PASS:2518 Deckel,3385 Menü,4712 Autoauswahl,6931 dunkleArena.
Neue helle Gameplay-RDC NICHT gefunden; fünftes Bild ausdrücklich ältere3543-Referenz.
Ganzer Frame in linearemLicht, lokale affineRGB-Korrektur a*decodedGuide+b,
Radius3 eps5e-5 KoeffizientenRadius2; Bias ermöglicht nativen Nebel-/Farbanteil
statt instabilem reinen Gain nahe schwarzerGuidepixel. AllePixel ausgewertet,
keine generativen Details/Schärfung und keine8.29MproTexturbehauptung.
Deckel nativkomponiert, UI für diese Offlineframes durch dokumentierte Rechtecke geschützt.
NativeColor enthält bereits einige UI: reineDisplay-vs-Color-Differenz reicht nicht.
Farbübergänge/Quellen-/Konturprobleme bleiben, KEINE Perfektions-/Fotorealismuszusage,
KEIN Beweis für LiveFPS/Timing. EXE98C60B... unverändert, kein Spiel gestartet/Build geändert.

## 2026-10-04: Separater 4K-Szenendetails-Fix implementiert

Ausgeliefert 02:17:38 Europe/Berlin, EXE SHA256
98C60B25288C9D0611A2BAC0C0300B1E1B194DCBBDBB9A8B0B541EF492535AE2.
Compile/link/WindowsLoaderExit0, beideLauncherCheckOnlyPASS, beideDesktoplinks erstellt.
Status ready-for-user-test; KEIN Spiel gestartet, Live-Abnahme offen.

Checkpoint `checkpoints/2026-10-04-colour-detail-repair/README.md`.
Nutzer fordert Umsetzung nach Diagnose. Nur Mode3-UI-Maske geändert:
neun NativeColor-Nachbarn +/-3 texels, Minimum, smoothstep(.04,.12).
Feine Guide-Geometrie/Farbfit/Feuer/Startup-Sentinel/Displayfallback unverändert.
GPU-Replay neuer Shader auf allen drei unveränderten RDCs PASS, inklusive
letztem 4K-Compute-Pass. Numerisches GPU-Modell ROI-MAE<=.00845 Byte/Kanal.
Grobe UI-Beimischung >1% auf identischen ROIs: Boden48,88->0,27%,
Fahrer31,80->3,35%, Rücklicht19,96->0,50%, Auspuff15,40->0,07%.
Keine per-Textur8.29Mbehauptung; gültige Weltteile global gleiche direkte Szenenschicht.
UI weiterhin Heuristik; Opponents-Schrift im Replay noch schwach/hohl,
Menü-/Titelkanten nicht perfekt. Nicht als vollständige Grafik-/Stabilitätslösung verkaufen.
Neues bin-colour-detail-repair-test, alte sechs EXEs geschützt.
Desktoplinkziel Twisted Metal - 4K Szenendetails Fix (+RenderDoc),
Launcher Start-ColourDetailRepair.ps1. Aktueller Build/Deliverystatus in JSON,
keinen bloßen Kompilier- oder Replay-Erfolg als Live-Abnahme deklarieren.
Altes VKPresent-Loglabel no_UI_neighbour_erosion=1 veraltet; Quelle/SPIRV maßgeblich.

## 2026-10-04: Qualitätsregression im Farbbuild nachgewiesen (nur Diagnose)

Drei neue Mode3-RDCs frame3891/5246/3543 erfolgreich read-only ausgewertet.
Checkpoint `checkpoints/2026-10-04-colour-quality-audit/README.md` mit
Replay-Inventaren, Rohdaten, vierteiligen Bildvergleichen und reproduzierbaren Skripten.
Feine Szenenquelle bleibt 3840x2160; Detailverlust entsteht nach Farbkalibrierung:
UI-Maske `smoothstep(.025,.06)` ohne Nachbartoleranz stuft auch Weltkanten als
Overlay ein und mischt gröberes NativeDisplay zurück. Kein Beleg für fehlendes Komma/Flag.
Auf IDENTISCHEN neuen Frame3543-Daten: native Beimischung >1% im Bodenausschnitt
alter Mode2-Detektor 0,27%, neuer Mode3 48,88%; Fahrer-ROI 3,35% vs31,80%.
ROI-Rechtecke, keine Materialsegmentierung und kein allgemeiner Qualitätsprozentsatz.
Shader-Rekonstruktion vs tatsächlichem GPU-Output MAE0,00582 Byte/RGB-Kanal;
seltene Rundungs-/Schwellwertstellen max40, nicht bitidentisch.
Nutzer bestätigt Startschrift/SantaMonica/Deckel normal; Gameplayqualität schlechter.
Korrektur zum vorigen Eintrag: Farbfeld ersetzt selbst kein RGB, aber nachfolgender
UI-Merge tut es an fälschlich erkannten Weltkanten durchaus. Live-Qualitätsabnahme negativ.
Nächster Ansatz: UI-Deckung von Welt trennen, feine Farbkalibrierung/Feuer behalten;
keine pauschale UV-Verschiebung/Schärfung/Pixelerhöhung. Weitere Aufnahme für Befund unnötig.
Keine EXE/Quellshader/Launcher/Verknüpfung geändert, kein Spiel gestartet, kein Fix gebaut.
EXE-Hashes C8ED54...(Farbe), 0E6274...(Direktszene), BC9828...(Feuer) erneut unverändert.

## 2026-10-04: Drei Direktszenen-Captures / getrennte native Farbkalibrierung

Build fertig01:01:45 Europe/Berlin, SHA
C8ED54AD44746909DD2ED634F1417C10D1F462A6ACF4AE21253DB9FE6162F839.
WindowsLoaderExit0, beideLauncherCheckOnlyPASS, Desktoplinksangelegt, delivery.json.
5alteEXEsunverändert, alteMasterShadersbytegleich. KeinSpiellauf/LiveAbnahme.

Nutzerbestätigt0E6274…scharf/flüssig, aberfalschehelleFarben/fehlendeSchatten,
Menütransparent/Linkskasten undDeckel-Straßenleck. NeueRDC3335/4688/5915replayedPASS:
Hauptmenü/DeathWarrant-Auswahl/RoadkillMatch, KEINStartupLogoimneuenCapture.
Checkpoint2026-10-04-direct-colour/README.md, Quellen/RAWs/3Vergleichspanels.
Mode3: feineGuideScene*kleinesbilateralesnativeRGBGainfeld, keineLowresRGBErsetzung.
UIauswirklicherDisplay-Fläche, keinNachbarMinimum/keinefestenValiditäts-HUDBlöcke.
4Shadercompile/SPIRVvalidationPASS, numerischeFarbMAEdeutlichgesenkt, keinLiveBeweis.
Neuerbin-direct-colour-test; Buildstatus/SHA maßgeblichbuild-status.json.
Feuer-/Draw-/Timingquellenbytegleich, fünf vorherigeEXEs geschützt, alteLinksbleiben.
NeueLinks Farbe undSzenendetailsTest/RenderDoc, Spielnichtautomatischstarten.
Startup/Bewegung/PartikelzusammenspielundSpuPostFX181sbleibenAbnahmeoffen.
GeminiAufgabenzettelSTART_HIER_2026-10-04_FARBE_UND_SZENENDETAILS.md.

## 2026-10-04: Gesamte direkte Szenenschicht gegen Master geprüft

Nachtrag00:28: Build fertig0E627485709E9493B423FE172EAE39B3ABDEDBE2E1A8C78FA07ED9DC1DFF1F2F,
LoaderExit0, LauncherCheckOnlyPASS, getrennteDesktoplinks Direktszene undFeuerTest
sowieRenderDoc vorhanden. KeinSpiellaufgestartet; Live-Test explizitoffen.
Feuer/Draw/Timingquellenbytegleich (preserved-source.json). AppbezogeneHDR-Präferenz
der gutenFeuerfassung übernommen, keineglobalÄnderung, tatsächlichesAutoHDRoffen.

Neuer Stand `checkpoints/2026-10-04-motion-materials/README.md` plus
Desktop/Aufgabenzettel/START_HIER_2026-10-04_DIREKTE_SZENENSCHICHT.md.
Genau eine neue vollständigeRDC frame6711 read-only replayed, PASS. AlterMaster
frame6062 vorhanden, aber anderesAuto/Ansicht. Gleiche neueRohdaten daher mit
altemDirektMasteralgorithmus und separatemUI-Schutz verglichen.
DirekteSzenenschicht erreicht auchWände/Boden/Bäume, nicht nurFahrer/Lampen.
Neue getrennteFassung bin-direct-scene-fire-test, MASTER_DENSITY=2, direkte
HighresRGBohneLowresPalette, UI/fehlendeAtmosphäre/nativeFeuer getrennt.
Shadercompile+SPIRVvalidationPASS; verbindlicherBuildstatus/SHA in build-status.json.
Quellstandsource-used undLinkermap gesichert. Kein Live-Erfolg ausVorschaubild ableiten.
RGB-Grading anders; UI-Klassifikator experimentell; Bewegungsnachlauf offen.
Keine8.29MproTexturbehauptung: gesamtesRaster3840×2160.
GuterFeuerbuildBC9828… und drei weitereEXEs unverändert/hashgeschützt.
AlterMasterprozess10128 ohneFenster gezielt beendet, orphan-stop.json.
AktuelleCapture-Snapshots zeigenHDR/GuideSlot2 vsnativeColorSlot0; Tagsbelegen
Schreibordnung, nichtFrameIDs. LAG1 bleibt; keinewillkürlicheUVVerschiebung.

## 2026-10-03 23:52: Feuer tatsächlich sichtbar / separater Reparaturtest

Neuer Checkpoint `checkpoints/2026-10-03-particle-repair/README.md`.
EXE `bin-particle-repair-test/rpcs3.exe`, SHA256
`BC9828F599AEB390D204063EBDF3FF99653AF8023A4B8821FD0A462EC5169AE0`.
Dump-Unwind+Disassembly lokalisieren alten Crash in `vkCmdEndRenderPass`
aus `tm4k_particle_begin`; beide Feuerhooks schließen jetzt nur offene Pässe.
Neuer Start erreicht Menü und Roadkill-Match; >=2400 echte native
Partikelisolationen im gesicherten Log. Nutzer: **Feuer wird angezeigt,
eigentlich alles korrekt**, nur minimale nachziehende/grobwerdende Kontur
bei Bewegung. Damit Feuer visuell bestätigt, NICHT alles perfekt/stabil erklärt.
GPU-Vergleich vier Aufnahmen: native Palette epsilon0.008 opt-in, Fahrer und
Rücklichter erreicht; Startup pixelgleich. Dunkelmaskenversuch NICHT übernommen.
Privater300%-Build/scene2160 mit8294400 Szenenpixeln, keine4K-Bitmapbehauptung.
Neue Desktoplinks **Twisted Metal - 2160p Feuer und Details Test** und
**Twisted Metal - 2160p Feuer und Details - RenderDoc**; beide CheckOnly PASS.
Gute26BB... und LogoD3F8... EXEs unverändert und per Hash geprüft.
Test nach186s vom Assistenten mitAlt+F4 beendet, Log gesichert.
WICHTIG: bei181.119760s SpuPostFX unmapped read0x1c11064; anschließend
RSX-Flips bis185.5s und normaler Shutdown. Fehler nicht verschweigen oder
als driver/device-lost ausgeben. Bewegungsartefakt und SPU-Zustand offen.
Neue RenderDoc-Session derselben EXE für Kurven-/Bewegungsaufnahme gestartet;
Capture-Erfolg erst nach F12-Dateiprüfung bewerten. Finale Freigabe weiter offen.

## 2026-10-03 16:21: Feuer-Detailtest im Menü abgestürzt

Kandidat4A214796… erster Live-Test FEHLGESCHLAGEN. Windows Event1000:
amdvlk64.dll32.0.31041.1004, c0000005, Offset0x19d83f; Dump bestätigt Nulladress-Lesen.
Konkreter auslösender Vulkan-Aufruf noch NICHT bewiesen. Kein fertiger Feuerdurchlauf
im Log. Belege: `checkpoints/2026-10-03-logo-leak-and-fire/first-live-crash/README.md`.
Teststatus failed-live-test sperrt unkontrollierten Neustart. Gute26BB… und LogoD3F8…
Hashes unverändert. Kein Rollback am guten Build, kein Treiber-/Windows-Eingriff.

## 2026-10-03: Aktuelle fünf Captures / separater nativer Feuer-Detailtest

`checkpoints/2026-10-03-logo-leak-and-fire/README.md` ist der neue Stand.
Fünf Logo-Test-Captures tatsächlich GPU-replayed; alle Szenen3840×2160.
Outlaw-Detailfusion nun nachgewiesen. Nutzer: Nachladen behoben, sporadische
grobe Schicht/Startup-Lecks sowie Fahrer/Lampen/Draht bleiben zu prüfen.
Globaler UI-Schwellwert .04:.12 verworfen: 1,51MGameplaypixel grober gemischt.
Final: altes Detailblending plus90%-schwarzeUI-Seiten-Sentinel, GPU-Regressionsbelege.
Native orange Partikel an Half-Scissor1920×1080 isoliert; Blendinversion
6243Menü/2208Matchpixel mit validError0. Neuer opt-in GPU-Extraktions-/Layerpfad
im separaten `bin-particle-detail-test`. Feuer LIVE NOCH NICHT ABGENOMMEN.
Build-/Loader/Hashstatus ausschließlich `build-status.json` beachten.
Best26BB… und LogoD3F8… samt Links bleiben unverändert. Keine pauschale
8,29M-Einstellung pro Textur; SceneRaster bereits4K. Neues Testspiel nicht gestartet.
Nachtrag16:09: separater Feuer-Test erfolgreich gebaut, LoaderExit0;
EXE4A214796A78D024713AF90B561816827794A9ECA15070436626A2F54128AD94E.
Desktop **Twisted Metal - 2160p Feuer Detailtest**, zusätzlich **… - RenderDoc**.
Konservative GPU-Regression: alle5Fusionsoutputs exakt Baseline;
native Feuerhooks/Platzierung/Performance bleiben Live-Test, kein Erfolgsversprechen.

## 2026-10-03: Zehn neue Captures / Logo-Datenverlust belegt und GPU-Fix geprüft

Checkpoint `2026-10-03-cohort-ten-captures/README.md` mit allen zehn Szenen,
echten Replays, Rohpixelbelegen und Grenzen. Motorrad-Verfahren dokumentiert:
fehlenden Menü-Detailpfad über abgeschlossene Snapshots wieder zugelassen,
KEIN Materialtexturersatz. Alte beste EXE26BB4F2D... bleibt unverändert.
Startup1054/2059/2306 haben leeren Color-Puffer, aber echte Logos im Display;
neuer Empty-Scene-Sentinel liefert drei pixelidentische native RGBA-Fusionen.
Sechs Szenen-Fusionen im Shader-Regressionsvergleich exakt unverändert.
Outlaw4327 ohne Fusion: Color→Display1442; neuer separater1600-Menütest,
Prädikat PASS, Live-Abnahme offen. Nicht als fertige Universallösung ausgeben.
Gutes Axel-Menü und Axel-Match enthalten orange Partikel an falscher Position;
6.243 bzw.2.208 Pixelbeiträge/Shader/Post-VS gesichert. Feuer NOCH NICHT restauriert.
Neue Startlinks erst nach Build/Loader; maßgeblich `build-status.json`.
Nachtrag15:05: serieller Link-Retry und Loader erfolgreich. Neue EXE
`D3F8FE1B37F98B6A57203350437F3E29CAA7D4F653CDAD9D2EE75FD739EE7027`,
Desktop **Twisted Metal - 2160p Logo Detailtest**, zusätzlich **… - RenderDoc**.
Beide Launcher CheckOnly PASS; Best26BB... identisch. Privater warmer Cache
41,87MB und appbezogene Auto-HDR-Registrierung übernommen. Kein Spielstart,
Live-Ergebnis und Feuerreparatur weiterhin offen.

## 2026-10-03: Detailpfad vom Nutzer als beste Fassung bestätigt / eigener RenderDoc-Link

Nutzer bestätigt den neuen Detailpfad-Test als bisher beste Fassung.
Desktop-Link **Twisted Metal - 2160p Detailpfad - RenderDoc** verwendet genau
`bin-scene-cohort-test/rpcs3.exe`, SHA256
`26BB4F2D6F0DB3092A5A39C86469F315767D416FCB8E2CEF058F9B2DBDC352DF`.
Schalter bleiben scene2160=1, geometryLag1=1, cohortWindow=1200, masterDensity ungesetzt.
Captures: `captures/scene-cohort-renderdoc/`, eigener Ordner je Start.
Details: `checkpoints/2026-10-03-three-scenes-and-fire/RENDERDOC_SHORTCUT.md`.
Nur Verknüpfung vorbereitet; kein Spielstart und keine neue Capture-Auswertung.
Normale Verknüpfung und funktionierende Fassung unverändert.

## 2026-10-03: Drei neue Szenen ausgewertet / Motorrad-Menü fällt auf native Ausgabe zurück

Checkpoint **2026-10-03-three-scenes-and-fire**, README mit Repro und Grenzen.
Nachtrag14:04: neuer Teilfix fertiggebaut; SHA256
26BB4F2D6F0DB3092A5A39C86469F315767D416FCB8E2CEF058F9B2DBDC352DF.
Desktop **Twisted Metal - 2160p Detailpfad Test**, CheckOnly/Loader PASS,
appbezogenes Auto-HDR-Profil registriert, beide bisherigenBest-Hashes identisch.
Spiel NICHT gestartet. Fenster1200 ist eine Testhypothese und nicht bereits
als erfolgreiche Menü-/Feuerreparatur abgenommen.
Frames4040/5448/7615 tatsächlich GPU-replayed. Primäre Farbe/Guide/HDR
3840x2160=8.294.400; Umgebung/Figuren-Draws und Depth ebenfalls inventarisiert.
Beide Matches echte Fusion+MicroE. Motorrad-Menü hingegen KEINE Fusion, obwohl
4K-Guide und abgeschlossene Snapshots vorhanden sind; Kopfdetails darin fein.
Neuer getrennt gebauter Teilfix: COHORT_WINDOW1200 statt Default600, nur
abgeschlossene nichtzukünftige Snapshotquellen, Lag1 und MicroE unverändert.
Tatsächlicher C++-Prädikatstest PASS (Logbeispiel Abstand736). Buildstatus
im Checkpoint; neuer Link erst nach LoaderPASS. Bisherige4K-/2160p-Links geschützt.
Kein automatischer Spielstart und noch keine Live-Menü-Abnahme.

Feuerpass im Menü1392 real vorhanden, orangeFP16, aber falsch positioniert:
19.034 geändertePixel, bbox881,51..1211,149. NICHT bereits live repariert.
Keine zusätzlichen PostFX-PPU-Calls blind aktiviert. Originaltexturen sind
NICHT alle4K; Szenenpixelzahl ist kein Texel-/Qualitätsregler.

Gemini-Kopfcheckpoint gefunden (Darkside-Kabine, nicht belegbar Motorrad):
TM4KMicro-fixed.hpp offline in den NEUEN GPU-Matchframes geprüft, PASS;
MotorfahrerROI nur1,585%Pixel geändert, HUD-ROI unverändert. Nicht pauschal
als Lösung des fehlenden Menüdispatch übernehmen. Zusatz-PixelHistory ließ
den RenderDoc-Replay-Helfer in renderdoc.dll abstürzen, Application1000/1001;
kein laufendes Spiel betroffen, kein gültiger Materialnachweis daraus.

## 2026-10-03: Szene2160 startet / echte HDR-Größe bestätigt / RenderDoc-Link

Nutzer: Start funktioniert, Menü/Ingame besser. Erststart kurz negativartig/
überbelichtet, zweiterStart ohne diesen Effekt; Häuser/Bäume zunächst nachgeladen,
danach vollständig. Ursachen offen. NeuesterLog als scene-2160/user-second-run.log
gesichert: TM4K scene-2160 host3840x2160 pixels8294400 UND tatsächlicher
HDRfusion-Input hdr3840x2160/final3840x2160. MicroE/Bewegungsfix aktiv,
kaputter direkterMasterRGB nicht aktiv. Keine pauschale perfekteStabilität behaupten.
EigenerAufnahmelink **Twisted Metal - Szene 2160p - RenderDoc** vorbereitet,
launchers/Start-Scene2160-RenderDoc.ps1, gleicheEXE+Schalter, einzelneCaptureordner
captures/scene-2160-renderdoc/. CheckOnly ist keinGPUCapture-Erfolg.

## 2026-10-03: Getrennter echter 2160-Zeilen-Szenenversuch in Arbeit

Nachtrag12:28: fertig gebaut, Statusready-for-live-test. NeueEXE SHA256
73C1BB4A57AAD129A168DDB33104D68E65F16E9AEB0DDA0A54D91F4058291D34.
Loader und Launcher-CheckOnly PASS. Desktop **Twisted Metal - Szene 2160p Test**
angelegt, Argumente geprüft. AppbezogeneAuto-HDR-Markierung vonBest kopiert,
keinGlobalwechsel. AlleBest-Hashes identisch. Spiel NICHT gestartet; tatsächliche
GPU-Größen/Schichten, AutoHDR und Stabilität noch keine Live-Abnahme.
Extrahierte echteC++-Skalierungsfunktionen mitAssertions kompiliert und ausgeführt:
Optin0=8.110.080 / Optin1=8.294.400;705Koordinatengrenzen geprüft.

Nutzer verlangt Quellszene3840x2160=8.294.400 statt3840x2112=8.110.080.
Rationaler Y-Faktor45/44 bei unverändertem X3 und Gast1280x704. Gemeinsamer
Skalierungshelfer, WPOS/Shader-X-Y, Snapshotvertrag und nichtinteger Downsamples
angepasst. Gute Micro-E-Farb-/Masken-/Filterwerte und Lag1 erhalten; kaputter
Master-RGB-Opt-in ausdrücklich aus. Kein pauschales Gamma-/Bloom-Restaurieren.
Elf Shader SPIR-V-validiert, kleine Raster-/Abtastungsprüfungen PASS; noch
kein tatsächlicher GPU-/Live-Abnahmenachweis. Checkpoint2026-10-03-scene-2160
mit Baseline, Repro, README/konkreten Bestwerten und Buildstatus. EigeneRuntime,
Config/Cache/Log; normaler4K-Link bleibt unverändert. Neuer Desktop-Link erst
nach erfolgreichem Build und Loader. Kein automatischer Spielstart.

## 2026-10-03: Auflösungshebel exakt dokumentiert / neue GPU-Captures vorhanden

`2026-10-03-master-density/RESOLUTION_MAP.md` dokumentiert Quell3840×2112
=8.110.080Pixel und vorhandene Ausgabe3840×2160=8.294.400Pixel. Kein Pixelzahl-
Regler; reale Stellen Config ResolutionScale300, VKPresent Quell-/Ausgabevertrag,
TM4KMicro decode/crop und TM4KMaster merge. SharedConfig betrifft auch Best!
Nutzer hat Menüframe4421 und Matchframe6062 mit RenderDoc aufgenommen, ca1,95GB
und2,97GB unter captures/master-density-renderdoc/2026-10-03_03-31-35-463-9937cbad.
Aufnahmeerzeugung belegt, GPU-Replay und Rekonstruktion werden separat geprüft.

## 2026-10-03: Pixeldichte bestätigt, Bildschichten fehlen / Windows-Systemcrash

Nutzer bestätigt gewünschte feine Pixel im Master-Test, aber falsche Menü-
Anordnung und fehlende Szenendetails. Danach Schwarzbild/keinSignal/Neustart.
Systemereignis1001: Bugcheck0x1A/0x41792, beschädigter PTE laut Microsoft;
Dump100326-16406-01.dmp vorhanden, Ursache noch nicht analysiert. Nicht einfach
als normalen RPCS3-Absturz/GPU-TDR oder als stabilen Erfolg darstellen.
Checkpoint SYSTEM_CRASH.md mit Befund/Grenzen; RenderDoc-Launcher vorbereitet,
vor erneutem Start Nutzer wegen zusätzlichem Systemcrash-Risiko informieren.
Neue Aufnahme noch NICHT gestartet/erzeugt. Best-4K-Version unverändert.

## 2026-10-03: Master-Pixeldichte direkt im Renderer – separater Test fertig

Nutzer priorisiert feine Szenenpixel ausdrücklich auch ohne Feuer. Referenz
XXX-Feuer-Master-4K.png; echter Packed-Guide RGB*AlphaByte/Exposure0.5/Reinhard/sRGB.
Neue TM4KMaster.hpp und Prozessvariable TM4K_MASTER_DENSITY=1 wählen direkte
Scene-RGB statt begrenzter Luma-Übertragung. Lag1/Best-Patches unverändert.
Startup/HUD/fehlender Himmel fallen auf Best-Basis zurück; keine kostenlose
Wiederherstellung aller Licht-/Nebel-/Partikelschichten behaupten.
Checkpoint `2026-10-03-master-density/`: Baselines, Quelle, Build/Shaderprüfungen,
README mit Grenzen, eigener Launcher Start-MasterDensityTest.ps1. Alte4K-EXE
und deren Link unverändert. Buildstatus verbindlich; Live-Erfolg noch ungetestet.
Drei Link-ICEs dokumentiert, gezielter VKPresent /GL-/PCH-Workaround gesichert.
Numerische große CPU-Prüfungen mit wechselnden seltenen Abweichungen nicht als
stabilen PASS behaupten; Ursache nicht ermittelt, siehe Checkpoint-README.
Nach Nutzer-Neustart zwei Offline-Prüfungen jeweils maximalerBytefehler0;
Link-Codegenerierung erfolgreich. Historischer nachgelagerter windeployqt-Schritt
suchte alte entfernte bin/rpcs3.exe; separate DLL/Flag/Loaderprüfung erfolgreich.
Kandidat SHA256 1FE21BFF05C4EA2038E1D576227B0CDE9DD4DD8B8592B85554FCCFD1054D9B40.
Eigener Desktoplink **Twisted Metal - Master Pixeldichte Test**, eigener Logordner,
appbezogene HDR-Erkennung von Best kopiert; alle geschützten Best-Hashes identisch.
Test gestartet mit TM4K_MASTER_DENSITY=1 und GEOMETRY_LAG1=1, last-launch.json.
Noch KEIN visuell bestätigter Live-Erfolg oder Feuer-Fix. Normaler4K-Link unverändert.

## 2026-10-03: Clown-Master als verbindliche Qualitätsreferenz gesichert

Nutzer bewertet die zweite offline rekonstruierte Clown-Szene als Zielbild:
feine Gesicht-/Hautdetails und glatte Streben sollen beim Live-Einbau erhalten
bleiben. Desktop-Kopie `C:\Users\box3\Desktop\XXX-Feuer-Master-4K.png`,
3840x2160, SHA256 BB0FD39EEBE2AA4B54B82C342059E1D8EAFB58071FDC0A275908CA9745851D70.
Vorhandenes Nutzerbild `xxx.png` nicht überschrieben. Reproduzierbare Quelle:
`checkpoints/2026-10-03-fire-analysis/build_fire_master.py` mit `fire-master/recipe.json`.
Originale Feuerfarbe/Alpha rechnerisch isoliert; Platzierung2x und Gesamtgrading
weiterhin Offline-Vorschlag. Kein fertiger Live-Fix. Szene absichtlich ohne UI.
Best-EXE SHA256 924F8AE7336F774C6295156E091DDEFDBEB57FE7938E5F703E662AAF86E2B0F5
und Patchkonfiguration E379F95B23C029D5C065D09F041AF7801A5284C7CFC575F639F4C547E247F88B
nach Erstellung erneut geprüft, unverändert. Für die Übertragung müssen native
Feuerschicht, Koordinaten und zeitliche Zuordnung erhalten bleiben; nicht erneut
pauschal alle PostFX einschalten oder Details durch bloßes Schärfen ersetzen.

## 2026-10-03: Orange Feuer-Schicht rechnerisch isoliert / Offline-Master

Vorheriger Feuer-Eingabetest fehlgeschlagen: Nutzerbild Streifen/Schattierung
kaputt, kein Feuer; failed-live.log mit SpuPostFX-AV. Nicht in Beststand übernommen.
Auf neuen Nutzerauftrag nur Original-Capture2049 offline weiter ausgewertet.
Event1558/1576 FP16 davor/danach: 151724 geänderte Pixel, alle Zielalpha vorher0.
Native Alpha-Blend-Gleichung invertiert, originale premultiplizierte RGB+Alpha
isoliert; Float32-Rückprüfung maxFehler0.0. Orange/warmweiße 16x1-Palette erhalten.
Unter `2026-10-03-fire-analysis/fire-master/` lineares native-fire-layer.npz,
Quell-Layer, Shader, recipe.json, transparente Vorschau, MASTER-FEUER-4K.png und
VERGLEICH-FEUER.png. Vorschlag2x-Koordinatenkorrektur vom halbaufgelösten Feuer-
Feld auf volle Szenenfläche; Szene aus bestehendem Packed-Guide dekodiert.
Explizit OFFLINE, nicht fertiger Livefix oder bewiesene originale Gesamtgrading-
Rekonstruktion. Oberer Flammenrand bereits im Capture abgeschnitten, nicht erfunden.
Szenenreferenz absichtlich ohne Menüelemente; Endbild mit UI separat erhalten.
Best-EXE/-Patches/-Launcher unverändert; kein Spiel für diesen Auftrag gestartet.

## 2026-10-03: Feuer-Captures ausgewertet / gezielter Eingabetest gestartet

Zwei echte Original-PostFX-Captures: Menü2049 + Match3372, GPU-Replay jeweils
PASS. Unter `checkpoints/2026-10-03-fire-analysis/` Schichten, Rastertrace,
Shaderdisassemblierung, Hashes und README mit nachprüfbaren Grenzen.
Menü Event1576 erzeugt orange/gelbes Feuer im FP16-HDR; c119/Endbild cyan/grün,
HDR-Flamme räumlich falsch skaliert; Packed-Guide enthält sie nicht. Kein Beweis
für einzelnen DOF-Schalter oder einen kostenlos zurückgewonnenen Effekt.

Zusätzlicher automatischer Best-Capture traf noch Startwarnung, nicht Clown;
nicht als erfolgreichen Clown-A/B-Vergleich werten. Vergleichslauf selbst
sauber geschlossen. Anschließend privater **Feuer Eingabe-Test** umgesetzt:
SharpFast0.6 entfernt nur NOPs0066ab30/ab74/ab84/ac1c, sieben andere bleiben.
60FPS/SPU-MLAA-disable/Best-4K-Timing an. Keine pauschale Umfärbung/Shaderänderung.
Hypothese native Eingabe-/Vorbereitung, Erfolg und FPS noch Nutzerprüfung.
Vorherige Original-PostFX-Patchfiles in `2026-10-03-fire-input-test/` gesichert.
Vorhandenen privaten bin-original-postfx-test weiterverwendet, kein weiterer
großer Emulator-/Spiel-/Cache-Klon. Alte Original-Test-Launcher durch ihre
Hashprüfung absichtlich nicht mehr passend; neuer Launcher
`launchers/Start-FireInputTest.ps1` + Desktop **Twisted Metal - Feuer Eingabe-Test**.
Gestartet02:30:45 PID6964, ohne RenderDoc. Normaler 4K-Link/Bestfiles unverändert.

## 2026-10-03: Original-PostFX-Regression / Clown-Feuer untersuchen

Nutzer meldet kaputte Grafik und ca.20FPS im getrennten Original-PostFX-Test.
Zusätzlich erschien erstmals bewusst beobachtetes, aber falsch gefärbtes
Feuer auf dem Clown-Kopf. Kein Nachweis, dass es vorher überall fehlte oder
welcher der11restauriertenAufrufe es erzeugt. Nicht pauschal alsDOF einordnen.
Ersten Testlog separat `2026-10-03-original-postfx-test/first-user-test.log(.gz)`
gesichert. Log belegt 60FPS-Patch und DisableSPUMLAA angewendet, HDR-Fusion
3840x2112→3840x2160 und motion-generation-test applied=1. Unser Renderer-/Timing-
Fix war also NICHT herausgenommen. OriginalerPostFXBypass aus, dessen Mehrarbeit
ist eine plausible, noch nicht einzeln benchmarkte Ursache derRegression.
Beststand und normaler4K-Link weiterhin unverändert. Auf Nutzerauftrag dieselbe
kaputte Testfassung mit RenderDoc gestartet, eigener Launcher
`launchers/Start-OriginalPostFX-RenderDoc.ps1`, eigene Capturewurzel
`captures/original-postfx-fire/`. ErwarteteSzene: Fahrzeugauswahl,Clown mitFeuer.
Erst Capture sichern, danach nur gezielte Feuerstufe finden; nicht diesen
schlechten Gesamt-PostFX-Zustand alsBeststand aktivieren. Captureerfolg amStart
noch nicht behauptet; F12/Datei/GPU-Replay separat zu bestätigen.

## 2026-10-03: Separater Original-PostFX-Test für DOF-Vermutung vorbereitet

Nutzer möchte Tiefenschärfe vorsichtig zurückholen, beste Fassung schützen.
RPCS3-Wiki nennt SPU-MLAA-Abschaltung, nicht DOF. Beste Patchauswahl geprüft:
60FPS + DisableSPUMLAA + PostFXSharpFast0.5 an, Raw4K aus. Selektiver eigener
Bypass überspringt11Stellen, eine einzelne DOF-Stufe bisher nicht bewiesen.
Nicht mit „Bloom aus“ oder „DOF-Aus-Schalter“ gleichsetzen.
Eigenes `build-msvc2026/bin-original-postfx-test/rpcs3.exe`, SHA924F8A…, gleicher
Renderer/TimingOpt-in. Private Config/Patches/GuiConfig/Logs/Caches und kopierte
Spielstände; ISO/Firmware/installierteSpieldaten geteilt, kein komplettes Spielduplikat.
Einzige Teständerung: private PostFXSharpFast Enabled=false. 60FPS-Patch und
SPU-MLAA-Abschaltung bleiben an. Stellt gesamte11Stellen-Gruppe wieder her,
keine behauptete DOF-Isolation; kann andere Filter und Kosten reaktivieren.
Auto-HDR-Spielregistrierung wieBeststand eingetragen, tatsächliche Aktivierung
noch unbestätigt. Desktop „Twisted Metal - Original-PostFX Test“ neu erstellt.
Loader/LauncherPreflightPASS, kein Spiel gestartet, Wirkung/FPS ungetestet.
Best-EXE/Config/Patches/Launcher unverändert gegengeprüft. Beste Desktoplinks
weiter unverändert verwendbar. ErsterTest benötigt eigeneCacheKompilierung.
Checkpoint `2026-10-03-original-postfx-test/README.md`, baseline/testPatchauswahl,
Setup-/Finish-Skripte, Hashstatus undHDRregistrierung vorhanden. Für DOF-Nachweis
später originalColor gegenFusionsEndbild vergleichen, daFusionBlur verdeckenkann.

## 2026-10-03: Bewegungsfix nutzerbestätigter BESTSTAND; alte Laufzeiten aufgeräumt

Nutzer nach Test: „beste Version“, „läuft in-Game ... perfekt“. Das ist eine
Live-Bestätigung des Nutzers, kein unabhängiger Dauer-/FPS-/HDR-Messnachweis.
Aktuell alleinige aktive Modfassung: `build-msvc2026/bin-motion-hdr-test/rpcs3.exe`,
SHA256924F8AE7336F774C6295156E091DDEFDBEB57FE7938E5F703E662AAF86E2B0F5,
zwingend über Launcher mit TM4K_GEOMETRY_LAG1=1 starten.
Normaler Desktop-Link „Twisted Metal - 4K Test“ zeigt jetzt auf denselben
Start-MotionGenerationTest.ps1 wie „Timing Test - Bewegungsfix“.
„Timing Test mit RenderDoc“ ist auf exakt diese EXE/Hash und Opt-in aktualisiert.
Beide Launcher -CheckOnly PASS nach Bereinigung; kein Spiel automatisch gestartet.

Auf ausdrücklichen Löschauftrag 7 alte Laufzeitordner und 13 alte EXEs direkt
im gemeinsamen bin in den Papierkorb verschoben, dazu 4 obsolete Desktoplinks.
3.803GiB/ca.4.08GB ausgewählt. Speicher erst nach Papierkorb-Leeren frei!
79 alte Junctions zuvor nur selbst, nicht rekursiv, entfernt. Ihre Ziele bleiben
unberührt. `bin` bleibt als Datenspeicher für aktive Modfassung erhalten: Firmware,
Spielstände, Config, Shadercache, Plugins usw. Spiel-ISO, Quellcode, Checkpoints,
GPU-Aufnahmen, Vergleichsbilder und Veröffentlichungsbelege unverändert erhalten.
`Cleanup-RetiredVersions.ps1`, cleanup-plan.json und cleanup-result.json enthalten
exakte Ziele, EXE-Hashes, Junction-Zuordnung und Ergebnis. Alte Verzeichnislinks
müssen bei einer Wiederherstellung aus dem Papierkorb anhand dieses Manifests
neu angelegt werden. Historische Build-/Startskripte mit bin-timing-test oder
alten bin-EXEs sind ab jetzt NICHT als aktueller Launcher zu verwenden!
Best-EXE weiterhin als vollständiger Checkpoint rpcs3-motion-generation-test.exe
vorhanden; aktuelle YAML zusätzlich config-user-confirmed-best.yml gesichert.

## 2026-10-03 01:29: Auto-HDR zweiter Versuch – Test heißt wieder rpcs3.exe

Nutzer bestätigt: zusätzliche GameConfigStore-Registrierung allein aktivierte
Auto-HDR NICHT. Geminis Namensänderung daher ebenfalls nachvollzogen:
identischer Kandidat SHA924F8A… jetzt in `build-msvc2026/bin-motion-hdr-test/rpcs3.exe`.
DLLs/Flags kopiert, alle Datenverzeichnisse als Junctions zu genau denselben
Zielen wie gute Timing-Fassung. Eigene UserGpuPreferences/GameConfigStore-
Registrierung, Flags17/Revision2691; Live-HDR noch unbestätigt.
Beste Timing/4K-EXEs und Links hashgleich. Bewegungsfix-Link weiterverwenden:
`Start-MotionGenerationTest.ps1` startet jetzt diesen neuen Pfad mit
prozesslokalem TM4K_GEOMETRY_LAG1=1. Loader --version und Launcher -CheckOnly PASS.
Keine Spielsession gestartet. `Prepare-NamedHDRTest.ps1`, named-hdr-*.json/txt
im Checkpoint dokumentieren Deployment und ursprüngliche Registrywerte.
Windows änderte beim --version-Start automatisch MatchedExeFullPath im alten
Child9f4d04fe… auf den neuen rpcs3.exe-Pfad; guter Child e419b48a… bleibt vorhanden.
Keine zusätzlichen globalen HDR-/Sicherheitsänderungen, kein Shader-Neubuild.

## 2026-10-03 01:24: Auto-HDR-Spielerkennung für Bewegungsfix ergänzt

Nutzer meldet dunkles Bild/kein Auto-HDR im neuen Kandidaten. Geminis Notiz
`TIMING_TEST_BUILD_AKTUELL.md` und tatsächliche Registrierung geprüft: gute
Timing-EXE besitzt UserGpuPreferences UND GameConfigStore. Kandidat hatte nur
den ersten Eintrag. Fehlenden GameConfigStore-Child ergänzt, Type=1,
Revision=2691, Flags=17 und Parent aus bestätigter guter Timing-Registrierung.
Neuer Child `4429e26f-1be0-442e-beb4-7da1f996e1bb`, Parent-Children um genau
diesen Eintrag erweitert; andere Einträge erhalten. AutoHDREnable=2097 besteht.
`Register-CandidateAutoHDR.ps1`, vorherige Werte und Prüfergebnis im Checkpoint
`2026-10-03-motion-generation-fix/`. Registrierung geprüft; tatsächliches
Auto-HDR nach Spielneustart noch NICHT bestätigt. Kein Spiel gestartet, kein
Shader/EXE/Save geändert, keine globale HDR-/Sicherheitseinstellung verändert.
Weiterhin Link „Twisted Metal - Timing Test - Bewegungsfix“ verwenden.

## 2026-10-03: Kugel/Auto-Doppelkonturen in Frame5064 belegt; separater Kandidat

GPU-Replay PASS,4226Drawcalls/11Dispatches, letzteEvent21947. Gleichkoordinatige
Ausschnitte in `2026-10-03-timing-ghost-capture/analysis/` zeigen versetzte Kugeln,
Ketten und Gegnerauto zwischen nativem Color und Guide; Endbild hat beide Konturen.
Schon Basis vor Micro E betroffen. Source-History geprüft, passende ältere
Geometriekohorte nicht vollständig in dieser Aufnahme vorhanden: KEIN offline
bewiesener korrigierter Endframe. Gezielte Timing-Hypothese umgesetzt in Source:
`TM4K_GEOMETRY_LAG1=1` wählt vorherige distincte HDR/Guide-Kohorte bei gleichbleibender
Color-/UI-Auswahl. Keine UV-Verschiebung, kein Schärfe-/Gamma-/Auflösungs-Edit.
Best-Link Timing2D4093…/44206F44… und alte4K-C337/9C9AAAD8… geschützt und gesichert.
Checkpoint `2026-10-03-motion-generation-fix/`:BaselineEXE/Source/YAML/Link,
README mit Grenzen,7Fälle Python-PolicyPASS, Hintergrundbuild separateEXE.
Build nun FERTIG:03.10.2026 01:12:27 ready-for-live-test,0Fehler,3bekannteCRTwarns,
LoaderPASS, separaterEXEhash924F8AE7336F774C6295156E091DDEFDBEB57FE7938E5F703E662AAF86E2B0F5.
Neuer Desktop-Link `Twisted Metal - Timing Test - Bewegungsfix` erstellt,
Launcher-PreflightPASS undLinkargumente geprüft. Beide Best-EXEs/Links weiterhin
hashgleich. KeinSpiel gestartet, Livewirkung UNGETESTET. Vorhandener perApp-HDRwert
AppStatus=1;AutoHDREnable=2097; ausschließlich für neueEXE kopiert, Originalprofil
undglobaleWindowswerte unverändert; protokolliert in app-hdr-preference.json.
RenderDoc-Startup-Hänger umgangen durch unveränderte Diagnosekopie
qrenderdoccmd-replay.exe (überspringt eigenen Breakpad-Helfer); keineSecuritysettings.

## 2026-10-03: Timing Test ist nutzerbestätigter Beststand; Aufnahme bewegtes Auto

Nutzer bestätigt: bisher beste Fassung, nahezu perfekt; frühere Säume an
Gartenstühlen/Tischen bleiben beim Fahren stabil. Selten im dunklen Arena-Level
bei schnell driftendem Gegner ein versetztes Geister-Abbild. Ursache noch offen:
Schicht-/Zeitzuordnung oder nativer Bewegungs-/Schatteneffekt; kein bewiesener Fix.
Desktop `Twisted Metal - Timing Test.lnk` zeigt inzwischen auf
`rpcs3-live-4k/build-msvc2026/bin-timing-test/rpcs3.exe`, SHA256
2D4093FBE03573F6298E6613DA8C7C2E5C6E2462C7A12AC308AA8B37F360A029.
Neuer Aufnahmeordner `checkpoints/2026-10-03-timing-ghost-capture/`.
Startskript prüft Best-EXE und bereits aktivierte Renderdoc-Kompatibilität,
verwendet identische Link-Argumente und Arbeitsverzeichnis. Keine Änderungen
an EXE, Shadern, Konfiguration, Flags, Spielstand oder Desktop-Verknüpfungen.
Prozesslokale RenderDoc-Layer-Variablen, separate Capture-Dateinamen.
Start verifiziert: PID11576, RenderDoc-DLL geladen, Target38920. Aufnahme
`tm-timing-moving-car-ghost_frame5064.rdc`, 2.983.067.580Byte, SHA256
6E0F2B798C4E64D702A9E80687D2221E5406784A457E48CEB0176704DA07FE67.
RenderDoc-Thumbnail erfolgreich extrahiert, zeigt dunkle Arena mit Gegnern.
Vollständiger GPU-Replay und Ursachenanalyse noch offen; nicht aus Vorschau
als behoben ableiten. run.log gesichert; Spiel bei Prüfung bereits geschlossen.
Zusätzlicher Desktop-Link `Twisted Metal - Timing Test mit RenderDoc.lnk` auf
`launchers/Start-Timing-RenderDoc.ps1` erstellt, Hashguard/Preflight PASS.
Jeder Start eigener Ordner `captures/timing-test-renderdoc/`; keine bestehenden
Verknüpfungen oder Best-EXE verändert. Neuer Link noch nicht live gestartet.

## 2026-10-02: Timing-Kandidat fertig, eigener Link; alter Best-Link unverändert

Build/LoaderPASS, neue separate EXE SHA256
2D4093FBE03573F6298E6613DA8C7C2E5C6E2462C7A12AC308AA8B37F360A029.
Neuer Desktop-Link „Twisted Metal - Timing Test“. Alte normale4K-Verknüpfung
weiter auf unveränderteC337-EXE, OriginalLinkhash9C9AAAD8… gleich. KeinSpielstart.
Livewirkung aufNachziehen/Halos/FPS/UI nochUNGETESTET. Checkpoint
`2026-10-02-timing-generation-test/`: SnapshotEXE/Source, BaselineEXE/Link/Source/
YAML, PolicyPASS, Buildlogs, Loaderausgabe, build-status.json undtest-link-status.json.
EinthreadLink9:28min,0Buildfehler,3vorbekannteCRTLinkwarnungen. ErsterPowerShell-
Preflight wegenCmdletmodulen fehlgeschlagen, mitgebündeltempwsh neu gestartet;
kein C++Fehler. KeineGrafik-/Save-/YAML-/Flagänderung, keineAktivierungaufaltemLink.
Gemini-Übergabe enthält fertigenStand; Nutzer testet explizitNEUEN Timing-Link.

## 2026-10-02: SEPARATE Timing-Testfassung implementiert, Best-Link bleibt C337

Auf expliziten Nutzerauftrag neue Timing-Fassung, alte Desktop-4K-Verknüpfung
NIEMALS umbiegen. Aktive EXE C337… unverändert, Link SHA9C9AAAD8… unverändert;
beide voll gesichert in `2026-10-02-timing-generation-test/baseline/`.
VKPresent.cpp:3distincte HDR/Guide-Kohorten statt3Snapshot-Versionen speichern;
erste/späte Color-Version derselben Kohorte erhalten, Auswahl jeweils jüngste
zulässige <=Display-Tag. Belegte doppelte Kohorten3494/3489 und3644/3640 etc.
Deterministische Policyprüfung PASS (synthetische Displaybeispiele, kein Livefix-
Beweis). Micro-Shader/YAML/Flags/Save unverändert. ZusatzVRAM bis97.32MB.
Neuer eigenständiger Build läuft via build_candidate.ps1, StatusJSON undLogs im
Checkpoint. Ziel nur `bin/rpcs3-timing-generation-test.exe`, NICHT rpcs3.exe.
Kein automatischer Spielstart/Aktivierung. Auch bei Kontingentende kann derBuild
fertig laufen; normalen4K-Link weiter aufBeststand lassen. Gemini-Fortsetzung
liest README/build-status.json und testet nur separate EXE nach LoaderPASS.

## 2026-10-02: Nutzer beschreibt 1–2Pixel nachlaufende Konturen beim Driften

Timing-Hypothese präzisiert, keine neue Einstellung/Liveänderung. Codeprüfung:
gemeinsames Guide/HDR/Color-Snapshot mit Barrieren und Schreibmarken, Auswahl
jüngstes color_tag<=display_tag mit Abstand<600. Kein nachgewiesener gemeinsamer
Guest/SPU-Frame-Identifier mit getrenntem native Present. Marken keine ms/Frames.
VSync/Wake-Up-Delay nicht blind als Zuordnungsfix verändern. Statischer Frame
registriert dominant(0,0), kann Bewegungsdrift nicht beweisen. Gemini-Auftrag
`PRIORITAET_TIMING.md` ergänzt: Producer/Display-Generationen und Maskenwechsel
getrennt untersuchen. Beststand unverändert.

## 2026-10-02: Frischer C337-GPU-Frame4294 erfolgreich, Gemini-Kopie verifiziert

Neue Session `2026-10-02-best-E-capture-retry1`, RDC 2.206.237.895Byte,
SHA256 B3F4EDDBF8C49A8040B1EF20F539D044D6EF0EAC2C4C627ABCC488D7D18E1F0B.
OpenFile/GPU-Replay PASS, 2853Drawcalls,28Dispatches,lastEvent13206.
d22=9-Input-Basisfusion,d23=Crop,d24=Guide,d25=Logs,d26=Fit,d27=E-safe-Endbild.
Nutzer meldet helle Umrisse nur bei Bewegung, auch Satellitenschüssel; im Stand
nicht. EinzelnerFrame ist keine zeitliche Prüfung; keineStartlogo-Aufnahme darin.
Testfenster von mir auf Wunsch Alt+F4 geschlossen, normales Quit im Log, Prozess
10780 beendet. EXE/YAML/Flags/Save unverändert. Neue hashgleiche Kopie für Gemini
unter Aufgabenordner `GEMINI_2026-10-02_BEST_E_LOGO_BEWEGUNG/input/fresh-frame4294/`.
Status/Auftrag aktualisiert; Schichtvergleich läuft, noch keine Livekorrektur.

Nachtrag Schichtvergleich: CPU-Reproduktion des GPU-Endbilds Mittel0.00000812/255,
max1; 2.939.592geschütztePixel exaktBasis. SechsKantenROIs inklusiveSchüssel haben
besten gemessenen Shift(0,0), Suchbereich+/-16Pixel. Kein Beleg für globalenOffset,
kein Temporalbeweis. Analysecode/JSON/Layer-Vergleiche und FINDINGS anGemini kopiert.
Beststand C337… unverändert, kein neuer Spielstart oder Livepatch.

## 2026-10-02: F12-Versuch ohne fertige RDC; Freeze dokumentiert, Beststand geschützt

C337… unverändert. Testlauf PID19440 per Nutzerwunsch beendet; Dead FIFO bereits
bei Emulationszeit1:50.56, as_rtt-null-Verifikation erst beim Stoppen4:07.93.
Kein neuer RDC-Fund, verbundene TargetControl-Abfrage ohne NewCapture. Nicht als
erfolgreiche GPU-Aufnahme darstellen. Log und Befunde im neuen Session-Checkpoint.
Zusatzanalyse: absolute Highlight-Schulter verändert Null-Detail-Gain; CPU-Test
neutraler Gain, alterFrame4060, mittlereBildänderung nur0.00056275/255. Kein Beleg
für flächigen Schleier, kein Live-Fix. RGB-UI-Schwellen und harte Logo-Sentinel/
Guide-Masken als konkrete offene Hypothesen dokumentiert. Gemini erhält aktuelle
Quellen/Log/Befunde und explizit alte1.266GBframe4060-Kopie plus E-safe-GPU-Replay.
Siehe `2026-10-02-best-E-motion-logo-capture/FINDINGS.md`; nächste frische Aufnahme
gezielt per TargetControl nach Nutzerbereitschaft, keine aktive Grafikänderung.

## 2026-10-02: Nutzer bestätigt E-sky-safe als neuen Beststand; Logo/Bewegung offen

Aktive C3377586… vom Nutzer als bisher beste Fassung bestätigt. Offen laut Nutzer:
Straße durch gelbes Startlogo, schwarzes Santa-Monica-Flackern, Menüschattierungen,
minimaler Schleier und weiße Konturen bei schneller Bewegung, nicht im Stillstand.
Keine Ursachenbehauptung; kein weiterer Grafikumbau vor frischer GPU-Aufnahme.
Beststand vollständig in `checkpoints/2026-10-02-E-sky-fix/` erhalten.
Neue Session: `checkpoints/2026-10-02-best-E-motion-logo-capture/`.
Gemini bekommt unabhängig Kopien im Desktop-Aufgabenordner
`Aufgabenzettel/GEMINI_2026-10-02_BEST_E_LOGO_BEWEGUNG/`; Auftrag vorbereitet,
neue Captures noch ausstehend. Einzelne F12-Frames sind keine Videoaufnahme.

## 2026-10-02: E-sky-safe jetzt AKTIV, Nutzer-Livetest offen

EXE SHA256 `C337758693C32E02364822912B22E05028CCCB7496C1F34238E06E3D2D652928`
aktiviert nachGPU-Replay/Build/LoaderPASS. Desktop **Twisted Metal - 4K Test**
bleibtdieselbeVerknüpfung. YAML315F…/sämtlicheFlags/Save unverändert, Spiel
nichtgestartet. BestätigtealteE1D3A… eigenständiggesichertimneuenBaseline.
Checkpoint `2026-10-02-E-sky-fix/activation.json`, E-sky-safe-EXE, Quellstände,
GeneratedVCXproj-Workaround, GPU-Bilder/Raw/Repro, Prüfwerte undBuildlogs komplett.
GezieltercellSysmodule-/GL-/PCH-Workaround erlaubte3.Build0Fehler; EinthreadLink
9:23Minuten. NochKEINFPS-/Bewegungs-/neuerSpieltestbeweis. NächsterSchritt:
gleicheszweitesLevel, Nebel/Himmel/Felskanten, Auto/RaketeinBewegung, danach
ersteshellesLevelundFahrerHeckgitter. AlteEinträgebeschreibenfrühereZustände.

## 2026-10-02: E-Himmel/Fernkonturen gezielt korrigiert, EXE-Build läuft

Der echte GPU-Capture4060 ist vollständig exportiert: d06=Basis vorE-Detail ist
sauber, d07Guide ohneHimmel/mitdunklenFelsen, d10Endbild hat starkeKontursäume.
Korrektur nur in3bestehendenMicroShadern: Vertrauen smoothstep(.12,.65,yg/yd),
Guide-Luma<=.015 ungültig, MinimumUnterstützung über gesamtenr4Filterkontext,
UI-/Fehlquellenpixel exaktBasis. Auflösung/Stärke1.35/300%/Farben/Basispfad/
Pufferpaarung unverändert. KeineGlobalweichzeichnung/Grafikkartentreiberänderung.

`checkpoints/2026-10-02-E-sky-fix/`: echteGPU-Replay-Korrektur nachErsatz genau
dieser3LiveShader an8040/8044/8048, siebenShaderPASS, CPU/GPUmax1ByteAbweichung,
3.774.448geschütztePixel exaktBasis; >.5StopFehleranteilFernwand11.35%->0.14%,
Fernzentrum17.09%->0.71%. Fahrzeugmitteländerung0.664/255; alterFrame3869-
FahrerCounterfactualCPU0.397/255,92.86%volleDetailgewichtung. SieheREADME,
verification.json, gpu-replay.json, Vorher/NachherPNG. KeinefrischeFPS-/Temporal-
oderuniverselleHimmel/RaketenGarantie. BewegteProjektile/Framepaarung bleiben
alsseparateHypothese offen. BesteE1D3A… separat baseline/rpcs3-E.exe erhalten.

ZweiBuilds C1001/LNK1000 inunverändertemcellSysmodule.cpp; /CGTHREADS1alleine
halfNICHT. AktuellerWorkaround nurdieseDateiRelease ohneGL/PCH, Codeunverändert,
GeneratedVCXproj-Baseline+Rezept dokumentiert. SerielleLTCG-EXE jetzt imBuild.
NochNICHTaktiviert; bisactivation.json bleibt1D3A… dieaktiveE.

## 2026-10-02: Nutzer bestätigt E als bisher beste/flüssige Fassung; Himmel offen

E 1D3A5664… laut Nutzer bisher beste Bildqualität, sehr flüssig, Auto gut.
Im zweiten Level Himmel/Umgebung noch falsch dargestellt. Keine neue Änderung
an Code, EXE, Flags, YAML oder Save. Nutzer wünscht gezielten RenderDoc-Capture.
Neue Session unter `checkpoints/2026-10-02-E-sky-capture/`, Starter prüft E-Hash
und Capture-YAML (ausschließlich RenderDoc-Kompatibilität zusätzlich).
Capture erst nach Nutzer-F12; nicht vom erfolgreichen Start auf korrektes
Bild oder vorliegende Aufnahme schließen. Beste E-EXE/Quellen bleiben im
Checkpoint `2026-10-01-micro-E-live/` gesichert. Himmel-Ursache noch unbewiesen.

## 2026-10-01: Freigegebene bereinigte E-Variante LIVE eingebaut, Test offen

Nutzer bestätigte speziell Fahrerhaut hinter dem Heckgitter (nicht den äußeren
Beifahrer) in `glow-study/heckgitter-fahrer-vergleich.png`, unten E.
E-Live-EXE SHA256 `1D3A56645DC5AA6F5C623A501776A23787E5F04869659D8A6A4369DA42EFB66A`
ist jetzt normal über **Twisted Metal - 4K Test** erreichbar. Kein Spiel gestartet.
Neue Flag `tm4k-micro-e-enable.flag`; vorhandene Flags/YAML/Saves unverändert.
Beste bestätigte C-EXE E775… zusätzlich sicher im neuen Checkpoint `baseline/`.

Checkpoint `checkpoints/2026-10-01-micro-E-live/` enthält E-EXE, VKPresent-E.cpp,
TM4KMicro.hpp, Build-/Loaderlogs, 7 validierte GLSL/SPIR-V-Shader, Prüfrezept,
Aktivierung/Hashes und ausführliches README. Neuer 5-Pass-GPU-Pfad: A-artige
Grundfusion (range0.02) einmal auf Vollbild zuschneiden; Packed-Guide perTexel
dekodieren/RGB8/Lanczos; logLuma; GuidedFilter r2 eps0.015; nur Mikrodetail1.35,
ohne Meso oder encodedRGB-Detail, farbtonerhaltend. Normale C-Basis range0.012
bleibt ohne E aktiv, 400%/AA haben keinen E-Pfad. Niemals doppelt zuschneiden.
Startlogo-Sentinel/HUD bleiben geschützt; SharedMemory/FP32 lokaleSummen statt
globalerFP32Integralsummen. Ca.348MiB zusätzliche GPU-Bilder, Performance offen.

7 Shaderprüfungen PASS, Build0Fehler, WindowsLoaderPASS. Gegen freigegebenes
E-PNG im Fahrer-ROI >99.998% Bytewerte exakt, max1/255 in CPU-Lokalfiltertest.
Das ist KEIN Beweis für pixelgleichen Live-Frame, KEINE garantierten60FPS und
KEIN Beweis, dass Bewegungs-Pixelwechsel behoben sind. Nächster Schritt:
Nutzer-Test Fahrer/Heckgitter/Auspuff Stand+Bewegung, Farben, Himmel, HUD/Logos
und FPS. Neue RenderDoc-Aufnahme erst bei Bedarf. Alte3869/4267 sind vor C/E.

Ältere Einträge unten beschreiben damalige Zustände, nicht den aktiven Build.

## Nutzerziel verbindlich: Geminis feine Arm-/Hautkontur halten, helleSäume entfernen

Noch keineLiveänderung. GeminiMasterbild mit eigenerRepro exakt0ByteAbweichung
nachgebildet; darauf D/E Offline3840-Varianten in gemini-C-comparison/glow-study/
erzeugt. E behält feinsteDekodiertdetails/Mikrostärke1.35, entferntMeso-/packedRGB-
Detailanteile, reduziert sichtbareKarosserie-Aufhellung. Arm2xVergleich gespeichert.
Filmrollen weiterhinSäume/Körnung, NICHTperfektbehaupten. Auflösung nichtreduziert.
Zusätzlich präzisereIntegralsummen/Farbclipping/HUDschutz; nichtalsisolierterTest
einzelnerRegler missdeuten. AktiveC E775… bleibtbestätigteSpielversion unverändert.
Protokoll+Rezept+Code unterglow-study/STATUS.md und remove_glow.py; Bewegung offen.

## Nutzer meldet C als beste Fassung; nur Gemini-Bildvergleich erstellt

C E775… Live lautNutzer bisherbestesBild, Auspuff imStand rund/klar. BeiBewegung/
Schießen etwaspixeliger, Ursache offen. Kein Liveeingriff in diesemTurn.
Checkpoint2026-10-01-gemini-C-comparison enthält fairen3869-Vergleich3840x2160,
Gesamtansicht+1:1Auto/Auspuff/Filmrollen/Fassade, gefrorenesGeminiPNG+Rezept/Code,
OriginalC-Spielprotokoll und Befunde. NeueC-Offline4K-Ausgabe ist KEIN neuerLiveCapture.
GeminiFassaden kontrastreicher, Filmrollen mit Säumen/körnigenFlächen. 95%-
Kontrastverlust-These unbelegt; tatsächlich4.26%GuideTexel capaktiv, nichtInfoverlust.
Route3 über letzteAbschnitte aktiv, wechselndePixelmasken/Treffereffekte/Framephase
weiteroffen. ErstStill-/Bewegungscapture gegenüberstellen, nicht blindTexturenverschieben.

## Rechte Offline-Variante C auf ausdrücklichen Nutzerwunsch live übertragen

Checkpoint2026-10-01-edge-palette-live: nur Packed-Alpha-Palettegewicht
range0.02->0.012, spatial1.28/Detail1/Cap0.06/UV/Himmel/HUD unverändert.
Vorige105FE…-EXE, Quelle, YAML/Flags gesichert und EXE-Hash geprüft.
Serialer Build0Fehler, GLSL/SPIR-V validiert, Loader--version Exit0 geprüft.
Aktiver EXE-SHA E775D778CE053A0645EC1F062C7BF5334539ED3F5C6DA166A77AFE80242F848D.
NormaleDesktopverknüpfung Twisted Metal - 4K Test bleibt und startet diesenStand.
YAML315F…/Save/Flags unverändert; noch KEIN Ingame-Test oder Grafikverbesserungsbeweis.
C hatte Neon-Ausreißer offline; Nutzer will bewusst Live-Vergleich. Kein stilles
Zurückstellen, Rückweg105FE… jederzeit vorhanden. Captures3869/4267 bleiben0.02!

## Ruhiger gemeinsamer Vergleich: Captures3869/4267 gesichert und exportiert

Erste Offline-Varianten fertig: A aktuelleFormel, B nurDetailstärke1.25,
C nurKantenfarbgewicht0.012. Vorschauen1920 und1:1-ROIs, keine neuen4K-Assets/
Live-Fixes behauptet. CPU/GPU mittlereFehler0.026..0.258/255, p99<=0.519,
ABER seltene hoheAusreißer (Auto98, Filmrollen177). Deshalb nicht pixel-exakt;
Gemini soll diese unabhängig prüfen. C nichtLive übernehmen, B kleiner noch
nicht belegterQualitätsgewinn. FINDINGS.md/export-audit.json dokumentieren alles.
19 primäreExportdateien einschließlichFP16 ebenfalls SHA-geprüft für Gemini
kopiert; damit unabhängige CPU-Arbeit ohne konkurrierendenReplay möglich.

Beide echte .rdc-Dateien vollständig replayt/exportiert, keine neuen Screenshots
als GPU-Capture ausgegeben. Hauptframe3869 (1744Draws, Merge8384), Kontrollframe
4267 (2038Draws, anderer Blickwinkel). Beide SHA-geprüft identisch für Gemini
in Desktop/Aufgabenzettel/GEMINI_RUHIGER_CAPTURE_2026-10-01/input/ kopiert.
Auftrag AUFGABE.md aktualisiert, Manifest enthält Hashes. Bestfassung105FE…
und YAML315F… unverändert. Kein Live-Fix oder neue Stabilitätsbehauptung.

Spiel von ChatGPT perAlt+F4 beendet; Log zeigt Stop4:08.487 VOR fatalen
Verifikationsfehlern4:08.747/4:13.280 beimAbbau. Fehler gesichert, getrennt
von Bildqualität. Offline-Repro/Varianten in still-frame-comparison/reproduce.py;
Input5 native Endbild, merge-output echter GPU-Merge. Historisch falsches
Dateilabel native-before-present.png nicht als natives Bild verwechseln.
Siehe neuen CheckpointREADME und frame3869-offline/metrics.json nach Abschluss.

## Nutzerauftrag: ruhiger Vergleichs-Capture, unabhängige Offline-Arbeit

Gute105FE… unverändert mit RenderDoc gestartet; Nutzer positioniert und drückt
F12 ohne Bewegung, meldet dann Aufnahme. Erst danach sauber schließen,
Capture+Log sichern, exakt identische .rdc für Gemini separat kopieren und Hash
prüfen. Neuer Checkpoint still-frame-comparison, Aufgabe für BEIDE unter
Desktop/Aufgabenzettel/GEMINI_RUHIGER_CAPTURE_2026-10-01/AUFGABE.md vorbereitet.
Input noch nicht vorhanden, kein Gemini-Replay parallel zu Spiel/ChatGPT. Danach
Offline-Komposition/Varianten aus gleichen GPU-Daten, keine aktivenCodeänderungen.
Beste bisherige Variante/Bewegungscapture3592 bleiben geschützt und Vergleich.

## GPU-Kurzcheck nach Display-Reset ausgewertet

Frame3592 echte1,84GB-GPU-Datei, vollständiger Replay/Export, 1498Draws,
9Input-Merge7209 aktiv. Spiel perAlt+F4 regulär beendet, kein parallelerReplay.
NativeBildbereich und finaler Crop117/72..3723/2088 passen zu festemMod-Rechteck.
User bewegte Auto beiF12; kein neutralerStillstand. Keine universelle Links-
verschiebung belegt. EdgeNCC Native/Color dx0,dy1..2 Kandidat, Guide/Color
schwach/uneinheitlich; keine Offsetänderung aus bloßemKorrelationspeak. Ruhiger
Vergleich am selben Ort sinnvoll, Bewegungscapture nicht verwerfen. Logos/
Titel-Flackern separat offen. Beste105FE…/YAML unverändert, kein Neustart.
[Vollständige Belege](checkpoints/2026-10-01-default-display-ingame/FINDINGS.md).

## Nach Display-Reset: Nutzer meldet jetzt richtige Ingame-Zuordnung

Nutzer spielte105FE… nach Save-Reset: Grafik richtig, keine sonstigen Fehler
gesehen. Rest getrennt: kurze Auspuff-Konturveränderung nach Raketentreffer,
schwarzes Flackern SantaMonica/Deckellogo, schwarze/normal wechselnde 3D-/
Schattenbestandteile des TwistedMetal-Titels. Startschrift weiterhin korrekt.
Kein Vollperfekt-Claim. Zuerst unveränderter Ingame-GPU-Check auf Nutzerauftrag:
fensterlosen beendenden Rest7484 nach Logsicherung gestoppt; neues Spiel2068,
RenderDoc-DLL geladen, SDKConnected=true. Wartet auf F12. Keine neuen Shader/
YAML/Displayänderungen. [Checkpoint](checkpoints/2026-10-01-default-display-ingame/README.md).

## Neue Eingrenzung: Display-Regler und festes Inset prüfen

Neuer Stand22:11: Nutzer-F12 erzeugte tm-best-inset_frame3545.rdc (2,015,599,061B),
noch nicht vollständig replayt. Spiel von ChatGPT mitAlt+F4 beendet; gestoppte
Threads im Log. Nach Nutzer-Escape keine weiteren UI-Eingaben; Prozess weg.
Spielstand auf Nutzerauftrag recoverabel nach display-inset-diagnosis/
savedata-before-reset/BCUS98106-FIXED- verschoben, alle3Hashes geprüft.
Frische Standard-Displaywerte erst bei nächstem Start bestätigen, keine50%-
Schätzung. Spiel nicht neu gestartet, EXE/YAML/Flags/Best-Sicherung unverändert.

Nachtrag: Auf Nutzerauftrag beste105FE… mit RenderDoc gestartet, PID15068,
Target38920. renderdoc.dll geladen, SDKConnected=true (ready.json) bestätigt.
Nur temporäre RenderdocCompatibility-Konfig, kein aktiver Grafikparameter
geändert. Wartet auf Nutzer-F12 am geeigneten Ort; noch kein neuer Frame.

Nutzer verschiebt Konturen über In-Game Screen Border Adjustment: passt
Auspuff, ist Welt versetzt und umgekehrt. Screenshot27/44/26/41%,Gamma50%.
Code nutzt weiterhin festen Inset39,24/1202,672 plus ursprünglichen Crop.
Kein belegtes Prozent->Pixel-Mapping, kein bestätigter bloßer Linksversatz.
Empfehlung neuer GPU-Frame der guten105FE… mit notierten Reglern, stillem
Auto/Kamera und nahen+fernen Kanten. Aktive Version/Best-Sicherung unverändert,
kein Spielstart/Capture/Fix in diesem Diagnoseblock.
[Befund und nächster Messschritt](checkpoints/2026-10-01-display-inset-diagnosis/README.md).

## Maßgeblich: beste Detail-only-Fassung vom Nutzer bestätigt und gesichert

01.10.2026: Nutzer meldet „fast perfekt“, gute Qualität, „beste Fassung, die
wir bis jetzt hatten“. Zwei Originalscreenshots3840x2160 gesichert. Rest:
entfernte Objekte beim Fahren etwas weich/gefühlt linksversetzt, kein belegter
fester Offset. Ein Fenstertitel zeigt45.32FPS: keine konstante60FPS-Garantie.
Aktive EXE105FE54796EA96263FFDCED9F04D41392FAF5F11642D68B57EBA1172411FA6A5
und YAML315FDECC6E0DDDFF485F06B1CA3BD1BCA11371B4AE136DA22DA95A81378CE06F
erneut geprüft. EXE/Config/Flags/Shader unverändert, kein neuer Test gestartet.

Best-Sicherung: checkpoints/2026-10-01-best-detail-only-confirmed mit Runtime,
Patches/Konfig/Flags, gesamten lokalen Source-Diff und geänderten Dateien,
Build-/Shadernachweisen, Bildern, Live-Logpunktkopien und SHA256-Manifest.
Nicht neue komplette Game-/Firmware-/Spielstandkopie. PID20204 beim Sichern
noch offen, nicht von ChatGPT beendet. Übergabe für Gemini vollständig unter
Desktop/Aufgabenzettel/START_HIER_AKTUELL_2026-10-01.md; älteres Startblatt
verweist darauf. Aufgabe4-Ergebnisse gelesen, nicht ungeprüft übernommen:
beziehen sich auf Vorgänger933019…, starke Frame-/RGBM-/UI-Claims nicht belegt.
Nächster Ansatz zunächst Stillstand vs Bewegung und exakte Pixelzentren prüfen,
beste Fassung schützen, keine neuen CAS/UI/Palette/Offsetänderungen blind.
[Sicherung und Rückweg](checkpoints/2026-10-01-best-detail-only-confirmed/README.md).

## Historisch: Detail-only-Kandidat aktiviert, Matchtest damals offen

01.10.2026: Nutzer testete Completed-Scene933019…: Startschrift nicht mehr
transparent, Menü normal, Spielgrafik weiter falsch. ChatGPT schloss regulär
mit Alt+F4; Stopping/closeEvent bestätigt. Frame4372 komplett ausgewertet:
neun Inputs, echte FP16-HDR-Kopie, kein Normalen-Input als HDR. HDR partiell
schwarz, scharfer Guide komplett. Falsche/coarse Strukturen teils bereits im
nativen Eingang vorhanden. 'Normalen als HDR' ist hierfür keine belegte Ursache.

Jetzt neue enge Shader-Änderung: statt absoluter Guide-Luma-Zielhelligkeit nur
Guide-High-minus-Guide-Low als Detail übertragen. Native breite Beleuchtung,
Farbton, UI-Schutz und gespeicherte Pakete bleiben. Keine CAS-Erhöhung. CPU-
Referenz trifft erfasste GPU in geprüften Ausschnitten im Mittel0.21–0.24/255;
Validität des Nachbaus bewiesen, nicht perfekte Beleuchtung des Originalspiels.
Neue EXE105FE54796EA96263FFDCED9F04D41392FAF5F11642D68B57EBA1172411FA6A5
aktiv im bestehenden 4K-Test-Link. Build, Versionsprobe, drei Shaderprüfungen
und Null-Detail-Invariante bestanden. Kein Livetest dieser Variante; geschlossen
gelassen. Beide vorigen EXEs und Capture/Log bleiben gesichert.

Wichtige historische Korrektur: 0x670bc0/c510fc startet auch Image-Jobs, ist
nicht bloß eine reine SPU-Tonemap-Synchronisationsbarriere. Performance-NOPs
nicht auf dieser alten Behauptung rückgängig gemacht; alle Patches unverändert.
[Details, Reproduktion und Grenzen](checkpoints/2026-10-01-detail-only/README.md).

## Aktuell 01.10.2026: echter Completed-Scene-Kandidat aktiv, Livetest offen

Source-Contract-EXE 8E007197… getestet: Nutzer meldet weiterhin kaputte Grafik.
Frame4027 vollständig exportiert; 2.170 Draws, kein Mod-Merge, Präsentation
21081→3363. HDR ist im Live-Katalog resident (RGBA16F), wird aber im Capture
nicht gelesen; 'keine FP16-Ressource in RenderDoc' bedeutet NICHT 'kein HDR'.
Color/Guide aktuell neuer als das Display; Material-/Beleuchtungsbilder sind
keine sicher aktuellen Endfarben. Alte Schlussfolgerungen entsprechend begrenzt.

Jetzt tatsächliche Source-Änderung: drei GPU-Pakete mit kopiertem HDR, Color,
Guide am vorhandenen Submit-Punkt nach Renderpass-/Query-Ende. Auswahl gegen
Schreibtag des Display-RT, nicht globales RSX-'jetzt'. Keine aktuell unabhängig
gewählten Next-Pass-Bilder; kein HDR-only-Wechsel bei fehlendem Guide. Noch
Heuristik, keine bewiesene SPU-Frame-ID. Build und Versionsprobe Exit0, acht
Offline-Prüfungen PASS. Aktive EXE 9330196418805F557F31EDC1EE8CA59B39CA84DE07E62DC63C2A6072A4ACE87C.
Erstes Live-Protokoll bei Flip3480: packed3256, none224 (Startphase), changes2;
nach Aktivierung durchgehend Packed-Route protokolliert. Kopien aktiv und
passendes gespeichertes Paket ausgewählt. Noch kein Nachweis richtiger Pixel
oder Match-Stabilität; Spiel für Nutzer-Test offen gelassen.
Mit RenderDoc gestartet, TargetControl38920. Noch kein bestandener Match-
Livetest, kein Erfolgsclaim. Original-Konfiguration weiterhin Hash315FDECC….
Vorversion vollständig bewahrt, Shader/ISO/Performance-Patches/YAML unverändert.
Gemini-Auftrag4 auf Desktop-Aufgabenzettel: unabhängige Prüfung und echte
Produzenten-/Scanout-Bindung. [Checkpoint](checkpoints/2026-10-01-coherent-scene/README.md).

## Aktuelle GPU-Auswertung: HDR-Quelle/Präsentation prüfen

01.10.2026: Frame3940 vollständig replayt/exportiert, 2.180 Draws. Kein
Mod-Merge in diesem Frame; finaler Blit20960→3488. Aktuelle Texturliste hat
keine hochauflösende FP16-HDR-Fläche, anders als der gute ältere Frame3752.
Code nimmt feste Gastadressen als Pufferrollen an und prüft raw/HDR-Format
nicht. Zuordnung muss vor nächstem Farb-/UI-Tuning belegt werden. Route0
entfernt nicht automatisch HUD; Alpha-Thread-Ursache unbewiesen. Zweiter
Gemini-Auftrag im Desktop-Aufgabenzettel ergänzt. Aktive EXE/Config unverändert.
[Belege, Reproduktion, nächster Ansatz](checkpoints/2026-10-01-str2-regression/capture-current/FINDINGS.md).

## Maßgeblicher Nachtrag: Neu 5 noch nicht flackerfrei

**Aktueller Livetest fehlgeschlagen / STR2:** Nutzer meldet weiterhin farblose
ausgesparte Startschrift und Straße im Logo, außerdem falsche Schattierungen
im anderen Level. Aktive D3D7C1EA…-EXE und Desktop-Link geprüft. Logo-Cover ist
kein bestätigter Fix; alte korrekte Replay-Logos bilden Livefehler nicht ab.
STR2 und Lauf gesichert, ältere UI-/Alpha-/Decoder-/Sync-Erkenntnisse erneut
geprüft. Keine neue Source-/EXE-/Configänderung oder Rücknahme. Nächster Beleg:
aktuelle fehlerhafte Input5/Marker/Merge/Präsentation und getrennte Level-
Schattierung. [Diagnose und genaue Eingrenzung](checkpoints/2026-10-01-str2-regression/README.md).

**Logo-Schutz-Kandidat jetzt testbereit und aktiv:** neue EXE SHA256
`D3D7C1EA0E7BF871E68298D6A30236863D09AC44F00159097A39717310C31478`,
über bestehenden Desktop-Link „Twisted Metal - 4K Test“. Build Exitcode 0,
saubere Versionsprobe Exitcode 0; Konfiguration und Flags unverändert.
Neu 5 FD998337… bleibt separat als Rückweg. Alte fensterlose RPCS3-Reste
beendet; Live-Log vorher/nach Freigabe gesichert. Noch kein Logo-/Gameplay-
Livetest und kein bewiesener genereller Anti-Flacker-Fix. Maßgeblich ist nun
[Logo-Cover-Checkpoint](checkpoints/2026-10-01-logo-cover/README.md).

**STR belegt Live-Straße im Deckellogo; gezielter Schutzkandidat im Bau.**
Replay-Logo korrekt, HDR-Fallback mit denselben Capture-Daten ebenfalls korrekt;
Routenwechsel erklärt Logo somit nicht allein. Weglassen des Alpha-Schutzes
simuliert eine Straßenüberlagerung, ist aber kein Beweis für Live-Alpha-Ausfall.
Im bestehenden Fit-Pass nun ein grobes Raster für große deckende Startbilder;
Mehrheit „Screen schwarz, Szene darunter farbig“ setzt Marker, Merge übernimmt
dann unveränderte Präsentation. Acht gespeicherte Frameprüfungen und Shader-
Validierung erfolgreich; noch kein Livetest, kein bewiesener Anti-Flacker-Fix.
Aktive EXE bis Aktivierung weiter Neu 5. [Kandidat, genaue Formel, Grenzen](checkpoints/2026-10-01-logo-cover/README.md).

**Deckellogo-Frame 2512 ausgewertet:** Replay des Eat-Sleep-Play-Logos korrekt,
Nutzer bestätigt aber Menüstraße im Live-Logo. Vor-/Nach-Merge-PNGs wieder
identisch, neun Inputs, Event 1249. Präsentationsquelle besitzt im Frame keinen
aufgezeichneten Schreiber, sondern nur Barrieren/Compute-Lesen; Logo bereits
Frame-Eingang. Einzelner Replay-Frame reproduziert sichtbaren Livefehler nicht;
Momentwahl/Capture-Sync/Live-Ressourcenproblem noch zu trennen. Nächster Test:
normaler Bildschirm-Screenshot plus GPU-Framefolge. Neu 5 unverändert. Zweiter
Lauf meldet erneut Dead-FIFO/Frozen. [Belege und Producer-Trace](checkpoints/2026-10-01-neu5-ui/README.md).

**Neueste Aufnahme ausgewertet:** Nutzer hat beim Start F12 gedrückt und das
Spiel selbst geschlossen. Frame 1516 erfolgreich replayt: Warnhinweis, Merge
845 mit neun Eingängen (Packed). Vor-/Nach-Merge-PNGs byteidentisch; dieser
einzelne Frame zeigt keine durch unseren Pass verursachte Schrifttransparenz.
Kein Match-/Zeitpaar vorhanden, Fehler bei anderen Logos/Namen weiter offen.
Im Lauf zusätzlich Dead-FIFO/Frozen bei 1:27 vor manuellem Schließen, Ursache
noch nicht bestimmt. Aktive EXE/Quelle unverändert, Spiel geschlossen.

01.10.2026: Nutzer meldet weiterhin Wechsel zwischen guter/schlechterer
Spielweltqualität, teiltransparente Gegnernamen/Startlogos und Bergsaum.
Die frühere Aussage unten „6257 Frames kontinuierlich / null Flackern“ ist
zu korrigieren: Flip 9360 zählt kumulativ `changes=93 none=297 hdr=2806
lit=0 packed=6257`, auch spät im Lauf steigen HDR-Fallback und Wechselzähler.
Neu-5-EXE `FD998337…6529A` unverändert gesichert und mit RenderDoc gestartet.
Globale UI-Schwelle 0,04/0,12 verändert im Offline-Vergleich auch Fahrzeug-
und Straßendetails; gewichteter Sky-Fit löst die Horizontkante nicht sauber.
Beide Vorschläge NICHT aktiviert. Neues direkt aufeinanderfolgendes Framepaar
vorbereitet; wartet auf Nutzer „bereit“ im Match. [Aktueller Stand, Messwerte
und Übergabe](checkpoints/2026-10-01-neu5-ui/README.md). Keine neue Live-Fix-
Behauptung und kein Wiederherstellen des entfernten Draw-Snapshot-Eingriffs.

Letzter Stand: 01.10.2026, 05:35 Uhr. Dieses Dokument ist der Einstiegspunkt, damit 60 FPS, PostFX, RenderDoc, 4K-Fusion und weitere Versuche nicht wieder vermischt werden.

**Aktueller Meilenstein: Nutzertest Neu 5 bestanden, Stabilität gesichert.** `neu5.png` gesichert. Nutzer: *„sieht schon ziemlich gut aus ... aber an sich schon richtig geile Qualität.“* Kein Ladeabsturz mehr; der vorherige Treiber-Absturz in `amdvlk64.dll` wurde durch Entfernen des vorzeitigen `vk::end_renderpass` in `VKDraw.cpp` restlos behoben. Live-Log `neu5-run.log` belegt 6.257 Frames kontinuierlich auf Route 3 (Packed Alpha Native-Hue), null Flackern, `future_live=0`. Aktive EXE: SHA256 `FD9983378CD6FC9B3C1AA13B450F63DF57C70A1253CA230D36AFF5F3B226529A` gesichert unter `checkpoints/2026-10-01-neu5-latch/`. Zwei verbliebene Feinheiten eingegrenzt: 1. Schrift/UI-Teiltransparenz durch zu groben `smoothstep(0.18, 0.36)` in `merge-packed.comp` (Zeile 114); 2. Bergschleier durch fehlenden Himmel im RSX-Guide. [Details & Belege](checkpoints/2026-10-01-neu5-latch/README.md).

**Neuer konkreter Integrationsbefund nach Frame 3565:** gültiger aktueller Neun-Input-Capture numerisch geprüft; im Auto mittlere Detailmaske 0,9696. Geminis „fünf Methoden“ sind nicht fünf reale Engine-/Depth-/Mesh-Lösungen; Endbildformel nahezu bitgenau reproduziert, der Hintergrundsaum bleibt ohne zusätzlichen Glanz. Keine davon blind integriert. Vulkan-Layout-Helfer berücksichtigt an den Übergaben unseres Compute-Zusatzpasses keine Compute-Zugriffe; eigene explizite Abhängigkeiten für Quelllesen, Wiederverwendung, Quellrückgabe und Präsentationslesen eingebaut. **Alle Bild-/Shaderformeln unverändert.** [Compute-Sync-Kandidat, Nachweise und Grenzen](checkpoints/2026-10-01-compute-sync/README.md). Statischer Fehler belegt, Kausalität zum gesamten Qualitätsschwanken/Livetest noch offen. Vorherige Native-Hue-EXE/Quelle gesichert.

**Native-Hue-Livetest: fast gut, Qualität schwankt weiter.** `neu3.png` gefunden/gesichert. Neuer Log zählt im Match ausschließlich Packed-Route 3, null HDR-/LIT-Ausweichframes; der verbliebene sichtbare Wechsel ist daher nicht mehr der frühere Freshness-Pfadwechsel. Masken, Quellenzuordnung und Zeit-/GPU-Synchronisation nun gezielt prüfen, kein blindes weiteres Erhöhen der Grenze. Zwei unmittelbar folgende GPU-Frames der neuen Fassung vorbereiten. [Nachtest, Logbelege und eingegrenzter Diagnoseplan](checkpoints/2026-10-01-temporal-quality/STATUS.md). Wieder SpuPostFX-Gastfehler nahe Laufende, zeitlicher Zusammenhang mit Nutzer-Schließen noch ungeklärt. Screenshot zeigt momentan 45,21 FPS, keine stabile 60-FPS-Messung.

**Neuer Farb-/Umschalt-Teststand fertig:** Geminis `CHATGPT_PAKET` geprüft; neues Log-Fusionsbild und gelieferter GLSL-Block sind verschiedene Algorithmen, daher nicht blind kopiert. Packed-Alpha-Decodierung bleibt erhalten; native Tonemap-RGB-Farben werden jetzt von decodierter 4K-Helligkeitsstruktur getrennt. Relative Guide/HDR-Freshness statt absoluter Guide-Grenze korrigiert das im Live-Log nachgewiesene Route-1/3-Schwellwertproblem. Build Exitcodes 0, Versionsfenster geladen/selbst geschlossen (kein Spielstart); EXE `58FC974EC611786DACDD9E78F938E8FD88BF9AD071C2017FFC1516718EB41B08`. Drei neue Shader auf echtem gespeicherten GPU-Frame ersetzt und ausgeführt, numerisch geprüft; 410.500 geprüfte HUD-Pixel identisch. Vorherige EXE/Quelle gesichert, bestehender Desktop-Link unverändert. [Implementierung, Gemini-Prüfung, echte GPU-Vergleiche und Grenzen](checkpoints/2026-10-01-native-hue/README.md). **Livetest noch ausstehend; vorherige Gast-/Dead-FIFO-Fehler separat offen.**

**Erster Alpha-Live-Test: Anomalie laut Nutzer weg, aber Farben/Wechsel nicht gelöst.** `neu2.png` gesichert und mit gewünschten Referenzen 7/13 betrachtet. Letzter Log bestätigt Packed-Alpha aktiv, enthält aber bei 1:47,243 auch SpuPostFX-Gastfehler/Frozen-Meldung; getrennt offen, keine bewiesene Kausalität zum GPU-Pass. Für neue echte Neun-Input-Aufnahme lediglich C++-Routen-Diagnose ergänzt, Shader/GPU-Settings unverändert. [Befunde, Diagnose und Capture-Plan](postfx-120min/renderdoc-alpha-color-2026-10-01/STATUS.md). Neuer Farbfix noch nicht umgesetzt; alten guten Farbstand nicht als endgültig gelöst darstellen.

**Diagnosefassung gestartet:** EXE `AAB5CAD40D2AA1B49C5C9AD1DA22541D4B7FEFAD3AEBE47A94F929FDD771C37E`, Build-/Loader Exitcode 0. RenderDoc-Start Target-Control-ID 38920, Capture-Präfix `tm-alpha-colors` im Diagnoseordner. Shaderformeln unverändert zur Alpha-Testfassung; ergänzt sind tatsächliche Routenwechsel-/Count-Logs. Match/Overlay und neue F12-Aufnahmen noch vom Nutzer zu bestätigen; keine neue Farbkorrektur behauptet.

**Alpha-Live-Kandidat auf Nutzerauftrag umgesetzt:** Dekodierung vor Interpolation/Downsample, Float-Guide gegen Clipping, niedrigaufgelöste 7×7-Farbkalibrierung mit Quellkontrast-Erhalt und separat geschützte UI. Erster zu weicher Regression-only-Versuch in Vollbild-QA erkannt und verbessert. Aktueller Kandidat auf Frames 4380/4429 nachgerechnet; HUD-ROI unverändert, Shaderprüfung erfolgreich und alte Merge-Fallbacks SPIR-V-identisch. Gesicherte vorherige C-EXE/Quelle vorhanden. [Algorithmus, Varianten, Tests und Rückweg](checkpoints/2026-10-01-packed-alpha/README.md). Finaler Build/Loader und Aktivierung im zugehörigen BUILD_VERIFICATION dokumentieren; noch kein Live-Test, keine neue 60-FPS-/Stabilitätsgarantie.

**Finaler Alpha-Build testbereit und aktiviert:** EXE SHA256 `3CB8C7EFB58140687418F5FFE0109FFE2CBB4A062618E984DA05220171B5AF21`, Quelle `FB6F528A71243194F1F0CA4C45F1D724DBD37DABCD7467BFD5997FFE8F6F7037`. Beide finalen Builds und Loader Exitcode 0, neue Funktionen in EXE nachgewiesen und gesichert. `tm4k-packed-alpha-enable.flag` zusätzlich aktiv. Bestehender Desktop-Link **Twisted Metal - 4K Test** geprüft und unverändert auf neue EXE; kein Gameplay gestartet. [Endgültiger Build-/Startnachweis](checkpoints/2026-10-01-packed-alpha/BUILD_VERIFICATION.md). Alter C-Stand vollständig rückholbar, Livetest von Bild/FPS/Menü noch ausstehend.

**Neue Gemini-Übergabe geprüft:** Auch der dort zitierte Shader 5870 / 5029 speichert RGB geteilt durch Alpha-Faktor. Hardware-Dither-Ursache nicht belegt; vorgeschlagene GLSL-Korrelationsmomente sind mathematisch nicht Pearson und liefern auf konstantem farbigem Testsignal fälschlich ~1. Eigene Alpha-Vorschau geprüft: 3840×2160, 1.842.240 geschützte HUD-/Minimap-Pixel bitidentisch. [Prüfung, Gegenbeispiel und Integrationsentscheidung](postfx-120min/gemini-masterpiece-audit-2026-10-01/GEMINI_REVIEW.md). Kein Live-Code/Build geändert; vollständige Decoder-/Live-Verifikation noch offen.

**Gemini-Masterpiece und Säume – neuer konkreter Befund:** Ursprüngliches bevorzugtes Frame-4429-Bild separat mit Hash gesichert. Die Schaumkonturen folgen der isolierten RGB-Ansicht der LIT-Fläche 21414. Alpha enthält kleine diskrete Werte statt deckendem 255. Ein tatsächlicher Geometrie-Shader (Event 3796 / 3403) teilt RGB durch einen gemeinsamen Faktor und schreibt diesen in Alpha; reine RGB-Auswertung lässt die Beleuchtungskodierung unvollständig. RGB×Alpha vor Interpolation/Downsample entfernt die auffälligen Konturen in einer neuen vollständigen Offline-Vorschau deutlich. Gesamtdecoder, alle Material-/MRT-Pässe und Live-Stabilität noch nicht vollständig bewiesen. [Belege, Decoderexperiment, Bilder, aktuelle Recherche und Grenzen](postfx-120min/gemini-masterpiece-audit-2026-10-01/FINDINGS.md). Offizieller Twisted-Metal-SAT/ABS-Fix vom August bereits im lokalen Quellstand enthalten. Keine GPU-/Treiber-/Spiel-Konfiguration geändert; C-EXE unverändert, Gemini-Arbeit nicht verändert.

**Aktueller Nachtrag: C-Captures 4380/4429 erfolgreich ausgewertet.** Acht gebundene Eingänge, Float-HDR, C-Ausgabe und finales Bild vorhanden. CPU-Nachrechnung reproduziert C bis auf normale Float-/8-Bit-Rundung. Zusätzliche Verkleinerung auf 3606×2016 und linearer Vollbild-Blit bestätigt; direkte finale Pixelberechnung und sieben lokale Fit-Varianten nur offline geprüft. Kandidaten unter `postfx-120min/renderdoc-lit-c-live-2026-10-01/frame4380-detail-work/`; aktive C-EXE unverändert. [Capture-Belege, Grenzen, Stabilitätspunkt und Gemini-Übergabe](postfx-120min/renderdoc-lit-c-live-2026-10-01/STATUS.md). Gemini bekam eigenen Desktop-Analyseordner mit Kopie Frame 4429 und geprüftem Referenzcode; ChatGPT arbeitet auf Frame 4380. Keine Original-Spieltexturen ausgetauscht, keine generativ erfundenen Bilddetails, keine neue Live-Stabilität behauptet.

**Nutzerurteil C: fast perfekt, geringe Restunschärfe.** Zwei neue Aufnahmen inzwischen vollständig replaybar. Der spätere gesicherte Log enthält einen separaten fatalen RSX-Dead-FIFO-Fehler; dessen Ursache ist ungeklärt. Story-Video und Langzeitstabilität bleiben offen, obwohl die untersuchten Bilder gültig sind.

**Nutzerwahl C, danach Umsetzung:** Nutzer wählte die rechte C-Spalte (`LIT luma 1 @ 0.1`), nicht Vorschlag A. Lokale reale LIT-Feinstruktur wird begrenzt als Helligkeitsdetail übertragen; native Farben, HDR-Fallback und HUD-Schutz bleiben erhalten. C enthält auch die genauere frühe UI-Abtastung. Vollbild-CPU-Nachbildung stimmt an Spiegel/Fahrer/Heck bis maximal 1/255 mit der ausgewählten C-Spalte überein; 1.614.102 geschützte HUD-Pixel unverändert. Shaderprüfung erfolgreich. [C-Teststand, Buildstatus, Vorschau und Rückweg](checkpoints/2026-10-01-lit-c/README.md). Noch kein bestätigter Live-Test; Story-Video/SpuPostFX-Gastfehler bleiben getrennte offene Punkte.

**C-Build fertig:** EXE SHA256 `372D9D4108B0999A47271EB34A16925AD2FE6F6C041429335AFC223673AEBB0A`, Quelle `7244176927421478F85B02695E0B92F6AB58FBDB034826F1C8B4A1B406CA2EBD`. Library und EXE erfolgreich gebaut, C-Code in der Binärdatei geprüft, Loader-Test Exitcode 0. `tm4k-lit-c-enable.flag` zusätzlich zu den vorhandenen Haupt-/LIT-Flags aktiviert. Bestehende 4K-Test-Verknüpfung bereit; Spiel nicht gestartet. Vor-C-EXE/Quelle und fertige C-Fassung separat gesichert. Live-Ergebnis noch ausstehend.

**LIT-0.1-Nachtest und 36 Detailvarianten:** Nutzer meldet verbleibende Unschärfe und hat das Spiel geschlossen. Auf demselben hellen Capture weitere Farb-/Ortsgewichte, echte HDR-Detailresiduen, lokale LIT-Farb-/Helligkeitsübertragung und korrigierte frühe UI-Abtastung verglichen. Letztere verhindert falschen UI-Schutz an untersuchten Weltkanten. Vier Kandidaten mit unvergrößerten Ausschnitten gegenübergestellt; vorläufig A (LIT 0.1 plus begrenzte HDR-Feindetails). **Nur offline, aktive EXE unverändert.** [Varianten, Belege, Bilder und Grenzen](checkpoints/2026-10-01-detail-variants/README.md). Letzter Nutzertest-Log enthält wieder einen ungeklärten SpuPostFX-Gastfehler beim Ende des Laufs; separat gesichert, keine Kausalität zum neuen Vulkan-Pass behauptet.

**Neuester Versuch – LIT 0.1:** Der Nutzer wählte aus gleichen-Frame-Vergleichen abschließend LIT 0.1. Die hochaufgelöste beleuchtete Zwischenfläche führt nun eine 3×3-Rekonstruktion der nativen korrekten Tonemap-Farben; ihre falschen RGB-Farben werden nicht direkt ausgegeben. Frühe/späte UI sowie HUD und Minimap bleiben geschützt. Normaler HDR-Fallback-Shader ist SPIR-V-identisch zur vorherigen Sharp-HDR-Menü-Fassung. CPU-Prüfung: 1.614.102 geschützte HUD-Pixel ohne Änderung, gültiger Wertebereich. [Implementierung, Vorschau, Live-Grenzen, Buildstatus und Rückweg](checkpoints/2026-10-01-lit01/README.md). **Noch kein bestätigter Live-Test.** Story-Video bleibt separat offen. Vorherige EXE und Quelle erhalten; Aktivierung nur über `tm4k-lit01-enable.flag`, bestehende 4K-Test-Verknüpfung.

**Nachtrag: Menü-GPU-Diagnose abgeschlossen.** Drei echte RenderDoc-Menüframes zeigen das vollständige Menü vor unserem `tm4k_merge`-Pass und fehlende/dunkle UI danach. Ursache ist der unzureichende RGB-Schutz später halbtransparenter UI, nicht eine fehlende Asset-Datei. Zusätzlicher Alpha-Schutz stellt die Menüelemente offline wieder her, ohne die untersuchte Fahrzeugregion des älteren Match-Frames zu verändern. Noch kein Live-Patch; EXE unverändert. Ein globaler Wechsel der Farbbasis wurde wegen gröberer Fahrzeugkanten verworfen. [Befunde, Bilder und eingegrenzter Patch-Kandidat](postfx-120min/renderdoc-menu-error-2026-09-30/FINDINGS.md). Das schwarze Story-Video bleibt ein gesonderter offener Punkt.

**Darauffolgender Umsetzungsauftrag:** Auch Kopf, Haare, Brust und Schulter in den drei neuen Menüframes geprüft. Neuerer HDR-Match-Frame `tm-rail-retry_frame4261` mit tatsächlichem Merge-Event 18038 als Gegenprobe ergänzt. Zusätzlich zum Alpha-Schutz wird nun die Grundfarbbasis korrigiert, ohne das bestehende 4K-Residual zu ersetzen (`fused+screen-low_tone`). Quelle geändert, Shader-Prüfung erfolgreich, Build in Arbeit. Bisherige EXE/Quelle separat gesichert. [Teststand und Rückweg](checkpoints/2026-09-30-menu-color-test/README.md). Dieser neue kombinierte Versuch ist noch nicht live bestätigt und nicht pixelgleich zur alten Gameplay-Fassung.

**Build abgeschlossen:** EXE `828A7632B1AECB4AECE0796C6B2EEECEC98E9EE28B801158BC113A8A9DC9A0ED`, beide neuen Shaderbestandteile in der EXE nachgewiesen, GLSL-/C++-Build erfolgreich. Unveränderte Desktop-Verknüpfung zeigt darauf. Bisherige EXE separat erhalten. Bereit zum Live-Test; Spiel nicht automatisch gestartet. Bildqualität, Menü-Rückkehr und Performance müssen nun im Spiel bestätigt werden.

**Nutzer-Live-Nachtest:** `neu1.png` erscheint laut Nutzer im Match schlechter als Bild 18, besonders am Rückspiegel. Visuell mehr Körnung/Überlagerung am Heck; nicht identische Szene und Bild 18 stammt aus der älteren 4K-Szenenfusion. Kombinierte Korrektur daher nicht als neue gute Fassung einstufen. Auf Nutzerwunsch [GPU-Regressionstest im hellen Level](postfx-120min/renderdoc-bright-regression-2026-09-30/STATUS.md) gestartet, aktive RenderDoc-Anzeige geprüft, F12-Aufnahme noch ausstehend. Keine weitere Shaderänderung oder automatische Rücknahme.

**Heller Capture ausgewertet:** Echter Frame 5256 erfolgreich wiedergegeben, Fusions-Event 7330. Nah passende CPU-Nachbildung des tatsächlichen neuen Shaders (Fehler 0,282 von 255) bestätigt: globaler Zusatz `screen-low_tone` trägt grobe Präsentationskanten ein. Alpha-only bleibt an Spiegel, Fensterstreben, Heck, Fahrer und Straße exakt gleich zum rekonstruierten guten HDR-Pass. Nur den globalen Farbzusatz entfernt, HDR-Detailpfad und Alpha-Menüschutz behalten; neue EXE im Build. [Gleiche-Frame-Bilder und Grenzen](postfx-120min/renderdoc-bright-regression-2026-09-30/FINDINGS.md), [Sharp-HDR-Menü-Teststand](checkpoints/2026-09-30-sharp-hdr-menu/README.md). Menü-Clown-Farben und Story-Video bleiben offene getrennte Punkte.

**Sharp-HDR-Menü-Build fertig:** EXE `E01E0E3FD54D72D2AFF45B22A26F21876BF6928D005A1909A7A5CE9716A6F86A`, über bestehende 4K-Test-Verknüpfung startbar. Beide Codezustände in EXE geprüft: Alpha-Schutz vorhanden, globaler Farbzusatz entfernt. Noch kein Live-Test. Zusätzlich alle acht großen 3840×2112-BGRA-Flächen am neuen Capture-Event 7330 exportiert (`scene-candidates/`, Nutzungsereignisse dokumentiert). Scharfe frühere LDR-Szene besitzt auch wieder die unerwünschten Schattensäume; nicht blind als neue Ausgabe einsetzen.

**Nachtest vom 30.09., Story und Menü:** Der Nutzer meldet ein schwarzes Story-Video, das beim zweiten Aufruf funktioniert, und zeitweise unterbrochene Menüschrift (F1/F2). Die randlose EXE ist unverändert. Der [gesicherte Diagnosebericht](checkpoints/2026-09-30-menu-video-investigation/README.md) trennt Video-/Audio-Ringpuffer-Timeouts von der wahrscheinlichen UI-Masken-/Quellenzuordnung im Fusionspass. Bei F1s ungefährer Speicherzeit sind die 4K-Quellen frisch; ein alter Puffer ist daher nicht als Ursache bewiesen. **Der bisherige Stand ist ein guter Gameplay-Checkpoint, aber keine vollständig fehlerfreie Endfassung.**

## Welche Fassung ist welche?

| Fassung | Start / Ablage | Status |
|---|---|---|
| Vollständige 60-FPS-Sicherung | `C:\Users\box3\Desktop\ps3\Twisted Metal - SAFE 60FPS - 2026-09-27` | Transportierbare ältere Sicherung; nicht für Experimente verändern. |
| Aktueller 300-%-HDR-Fusionsstand, randlos | Desktop: `Twisted Metal - 4K Test.lnk`; [finaler Checkpoint](checkpoints/2026-09-30-final-4k/README.md), EXE SHA-256 `060FDAF4ED73DA5568880035B9B5CA5CBFCE734242B42D6E1887B1480B7B2910` | Nutzer bestätigte „Rand weg, Bild vollständig“; vorheriger HDR-Stand mit `Unbenannt20.png` im Match geprüft. Keine bestätigte Langzeitstabilität/60-FPS-Messreihe. |
| 400-%-Vergleich | Isolierte Installation `rpcs3-live-4k/build-msvc2026/bin-400-test/`; überholte Desktop-Verknüpfung archiviert | Nutzer meldet keinen erkennbaren Qualitätsgewinn. Log bestätigt 5120×2816 Rohszene und aktiven Fusionspass. Das ist Supersampling, keine 4K-Texturersetzung. |

**Nachtrag nach Nutzertest:** Die 400-%-Fassung wurde gespielt; der Nutzer erkennt keinen nennenswerten Qualitätsgewinn. Der getrennte Log belegt, dass 400 % und der Fusionspass tatsächlich aktiv waren (`raw=5120x2816`, `final=5120x2880`, Aktivierung bei 0:40). Die Anzeige-/Texturgrenze lässt sich also nicht durch noch mehr Auflösungsskalierung allein lösen. Der Log enthält auch einen `SpuPostFX`-Gastfehler bei 3:12 und später beim Beenden; ob das erste Ereignis vor oder während des Schließens ausgelöst wurde, ist nicht geklärt. Der 300-%-Checkpoint bleibt die bevorzugte Qualitäts-/Leistungsbasis.

**Analyse derselben Fusionsstelle:** `postfx-120min/renderdoc-test/audit_live_fusion_mask.py` simuliert die Schutzmasken des Live-Shaders auf dem gespeicherten Match-Frame. Beim Auto liegt die mediane Detailübernahme in der untersuchten Region bei ca. **0,95**, auf der linken Straße bei **0,14**, auf der mittleren Straße **0,02**, an der weißen Wand **0,20**. Das erklärt, weshalb Auto/Umrisse stark gewonnen haben, während manche Weltflächen noch grob bleiben. Es beweist **nicht**, dass ein pauschales Hochdrehen der Maske gut aussieht: Die früheren Offline-Versuche importierten dadurch falsche Konturen/Schatten. Die sichtbaren Haare können aus einer kleinen Originaltextur, Alpha-Geometrie oder dem verbliebenen PostFX-Detailverlust stammen; das muss vor einer gezielten Änderung getrennt werden.

**Asset-Vorbereitung ohne Spieländerung:** Im USA-ISO liegt `PS3_GAME/USRDIR/cars/sweet.psarc`. Eine Kopie wurde nur in `texture-upscale-experiment/PS3_GAME/USRDIR/cars/` extrahiert (PSARC SHA-256 `9FF490895EA034C73A871D371E36598751C424C834FC5F4F105B245DDEB0B920`); die fünf enthaltenen Dateien, darunter `sweet.vram` (4.816.384 Bytes), liegen unter `texture-upscale-experiment/sweet/`. Dies ist ein **Kandidaten-Asset für Sweet Tooth**, aber die konkrete Haartextur und ihr Format sind daraus noch nicht identifiziert. Weder ISO noch installierte Spieldateien wurden verändert.

Das Original `C:\Users\box3\Desktop\ps3\RPCS3-Twisted-19843` wurde von den Vulkan-Experimenten nicht überschrieben. Das ausführliche technische Protokoll und die vollständige Chronik stehen in `checkpoints/2026-09-30-aligned-4k/README.md` und `TM4K_LIVE_STATUS_2026-09-29.md`.

**Getrennter Schattenkanten-Kandidat (30.09.):** Der Nutzer präzisierte, dass ihn nicht der bereits im alten Screenshot vorhandene Fleck am Kofferraum stört, sondern dessen rahmenartiger Saum sowie ähnliche Säume an Arm und Rücken des Fahrers. Ein nur auf starke Residual-/704p-Kanten wirkender Anti-Halo-Shader wurde unter der Desktop-Verknüpfung `Twisted Metal - Halo Test.lnk` getestet. Der bekannte gute `Twisted Metal - 4K Test.lnk` bleibt unverändert. Details und Hash stehen in `checkpoints/2026-09-30-aligned-4k/HALO_TEST.md`.

**Testergebnis:** `Unbenannt19.png` aus dem Halo-Test erscheint am Auspuff und weiteren Fahrzeugkanten weicher als `Unbenannt18.png`; der Log belegt dabei weiterhin 3840×2112 Rohauflösung und aktive Fusion. Der globale Kanten-Schutz nimmt also gerade sichtbar gewünschtes Detail weg. **Halo-Test nicht zur neuen Standardfassung machen**; `Twisted Metal - 4K Test.lnk` bleibt der bevorzugte 300-%-Checkpoint. Wegen unterschiedlicher Szenen ist dies ein qualitatives, kein pixelgenaues A/B-Urteil. Weitere Saumkorrektur braucht eine gleiche-Frame-Quellentrennung, keine weitere globale Maskensenkung.

**RenderDoc-Aufnahme erneut geprüft:** Die Datei `twisted-match-compat_frame12865.rdc` enthält einen Match-GPU-Frame, keine Videoaufzeichnung des Menüs und des Spielverlaufs. In ihren 266 GPU-Bildern wurde aber eine bisher ungenutzte 3840×2112-HDR-Fläche (`12967`, `0xc1be0000`) gefunden, deren grobe Struktur dem korrekten Farbbild deutlich besser folgt als die bisherige Rohquelle. Eine unmodifizierte Substitution würde dennoch Fahrzeugdetails verlieren; sie ist **nicht live aktiviert**. Befunde und der Plan für getrennte Menü-/Match-Frames: `postfx-120min/REVISIT_CAPTURE_2026-09-30.md`.

**Neue Offline-HDR-Korrektur:** Die erste HDR-Vorschau war zu dunkel und ihre Farbabbildung ließ vorhandene 4K-Details weich erscheinen. Eine neu belichtete und gefittete HDR-Diagnose (`postfx-120min/renderdoc-test/compare_hdr_refit_mirror.png`) erhält die Spiegel-/Fahrzeugkante deutlich besser und zeigt weniger gesprenkelte Säume. Das ist ein einzelner aufgezeichneter Frame, **noch kein Live-Patch**; der bevorzugte 300-%-Build bleibt unverändert. Reproduktionsskripte, Messwerte und Einschränkungen stehen im obigen Analyseprotokoll.

**Live-Nachtrag (30.09., später):** Die HDR-Korrektur wurde danach in `Twisted Metal - 4K Test.lnk` eingebaut. Neuer EXE-Hash `FBED8A49335ABB3D117CE53BC55CAD7C560E7F5887D9A4D9FFB5462C2B970946`; vorherige EXE mit Hash `052145190924958CD211D1777AB4704445BC653C548CFA3A53376A64AE249F3F` unter `checkpoints/2026-09-30-hdr-refit/` gesichert. Log: HDR-Fusion aktiv mit 3840×2112 → 3840×2160. Nutzer spielte einen Match, meldete deutlich besseres Bild, normales Spiel bis zum eigenen Schließen und speicherte `Unbenannt20.png`. Der Ausschnittsvergleich mit Bild 18 zeigt am Auto ähnlich feine Pixelstrukturen und geringere auffällige Schatten-Säume; wegen anderer Position/Beleuchtung kein pixelgenauer Qualitätsscore. Ein `SpuPostFX`-Gastfehler erscheint im Log kurz vor dem normalen Fensterschließen; keine bestätigte sichtbare Störung im Match. Einzelheiten und Rückweg: `checkpoints/2026-09-30-hdr-refit/README.md`.

**Finaler lokaler Nachtrag (30.09.):** Ein separater AA-Test brachte nach Nutzerurteil gegenüber der guten 4K-Fassung kaum sichtbaren Nutzen und wurde nicht aktiviert. Das spielseitige schwarze Sicherheitsrechteck wurde anhand der GPU-Messung `(117,72)` bis `(3723,2088)` beim finalen Vulkan-Blit zugeschnitten. Nach einem zunächst falsch gebauten Zwischenstand wurde die tatsächlich geänderte Emulator-Bibliothek neu kompiliert und die EXE neu gelinkt. Das Log bestätigt aktiven Zuschnitt; der Nutzer bestätigte „Rand weg, Bild vollständig“. EXE, Quelle und Konfiguration liegen vollständig im [finalen Checkpoint](checkpoints/2026-09-30-final-4k/README.md); eine [beschriftete Bildergalerie](publication-evidence/README.md) zeigt die Entwicklungsstufen. Der Vor-Rand-Fix-Build bleibt separat als Rückweg erhalten.

## Gesicherte technische Kette

1. Offizieller `60 FPS`-Patch, `Disable SPU MLAA` und selektiver `PostFX Sharp Fast` v0.5 machen den älteren Stand deutlich flüssiger. Der PostFX-Patch deaktiviert mehrere teure Stufen; Bloom-Vorstufen und finales Tonemapping bleiben aktiv. Die frühere Kurzform „Bloom aus = 60 FPS“ ist zu ungenau.
2. Mit 300 % RPCS3-Skalierung erzeugt das Spiel bereits eine 3840×2112 große 3D-Rohszene. Der PS3-PostFX-Pfad verarbeitet wesentliche Farb-/Bildelemente weiterhin in 1280×704. Die RenderDoc-Matchaufnahme beweist den anschließenden verlustbehafteten 3×-Blit.
3. Der isolierte RPCS3-Vulkan-Pass kombiniert Details aus der hochauflösenden Rohszene mit den korrekten Farben des PostFX-Bilds. Die erste Live-Version hatte fehlendes Menü/HUD, die nächste falsch ausgerichtete Bildquellen. Die gemessene Abbildung der Quellen beseitigte die großen Versatzfehler in den Nutzerbildern 16–18.
4. Die aktuelle Ausgabe ist 3840×2160, aber das eigentliche Spielbild ist im Capture auf 3606×2016 eingerückt/skaliert. Der finale Vulkan-Blit entfernt den schwarzen Außenrand; damit wird das vorhandene Bild geringfügig vergrößert, nicht mit neuen Texeln ergänzt. Eine 4K-Bilddatei garantiert nicht, dass jede Haarsträhne, Textur oder Alpha-Kante eine 4K-Quelle besitzt.

## Regeln für den nächsten A/B-Test

- Zuerst einen identischen Standpunkt im selben Level mit dem **300-%-Checkpoint** und dem **400-%-Test** vergleichen; jeweils Screenshot und gemessene FPS/Framezeit. Nicht unterschiedliche Level oder Kamerawinkel als Qualitätsbeweis vergleichen.
- Menülesbarkeit, Fahrzeugkopf/Haar, Fahrerhaar, Straße, HUD, Farben und Artefakte getrennt beurteilen. Falls der 400-%-Log `TM4K fusion active` nicht meldet oder im Probe-Log nicht die erwarteten GPU-Abmessungen erscheinen, war es **kein gültiger Vergleich** der Fusionsfassung.
- Wenn 400 % die Haarstruktur nicht verbessert oder zu viel FPS kostet, auf 300 % bleiben. Danach gezielt die Original-Textur/Alpha-Karte und ihre Mip-Stufen im GPU-Capture oder in den Spielassets identifizieren. Ein echter Textur-Mod erfordert eine sichere Zuordnung und einen eigenen Rückweg; nicht blind alle `.vram`-Dateien ändern.
- Vor jeder neuen Code-/Config-Änderung die getestete EXE und Konfiguration mit SHA-256 sowie einen sichtbaren Screenshot sichern; nur eine Variable pro Versuch ändern. Erkenntnisse und gescheiterte Ansätze hier ergänzen.

## 01.10.2026 – Compute-Übergabe und separater Himmelvorschlag

Der neue Native-Hue-Match-Log bleibt durchgängig im Packed-Pfad; früherer
Freshness-Fallback erklärt das restliche Qualitätsschwanken nicht mehr.
Echter neuer F12-Frame 3565 mit neun Merge-Eingängen erfolgreich exportiert und
numerisch geprüft. Statisch fehlen unserem Zusatzpass vollständige Compute-
Abhängigkeiten beim Einlesen, Wiederverwenden, Zurückgeben und Präsentieren
der Bilder. Vier lokale Barrier-Gruppen ergänzt; keine globale Änderung am
Windows-/AMD-Treiber, keine neue Bildformel oder Konfiguration.

Release gebaut und aktiviert, EXE-SHA256
`475AD81489ADA8521713B477796714624E3E8069A744534851B7BD2D73F8B147`.
Loaderprüfung `--headless --version` mit tatsächlichem Exitcode 0. Normaler
Desktop-Link **Twisted Metal - 4K Test** zeigt auf diesen Build. Kein Spiel
automatisch gestartet; Livewirkung, Frametimes und Gastfehler noch zu prüfen.
[Checkpoint und Buildbeleg](checkpoints/2026-10-01-compute-sync/README.md).

Geminis neue Himmelbehandlung separat eingefroren und nachgerechnet:
fehlende schwarze Guide-Bereiche vor dem Log-Detailtransfer mit nativer
Helligkeit füllen. Breiter Saum deutlich reduziert, einzelne Konturen bleiben.
Helligkeitsschwelle ist keine perfekte Depth-Maske; Roh-Alpha wird dort weiter
ignoriert, synthetischer Glanz addiert. Nachrechnung nahe übereinstimmend,
aber seltene große Ausreißer: kein bitgenauer PASS und kein 100-%-Beweis.
Kein separater Bloom-Eingriff im gelieferten Endbildcode belegt. Nicht in den
Sync-Test gemischt; gültigkeitsgewichtete Live-Fit-Nachbarschaft als gezielter
späterer Ansatz festgehalten.
[Prüfung, Original-Snapshots und Vergleich](checkpoints/2026-10-01-temporal-quality/gemini-sky-review/README.md).

## 01.10.2026 – Neu 4: Synchronisationskorrektur reicht nicht; Guide-History

Nutzer meldet weiterhin schnelles Schwanken, schlechte/unscharfe Bereiche und
Menüstraße hinter den Startlogos. Nicht als erfolgreiche Reparatur einstufen.
Letzten Log und `neu4.png` gesichert. Route 3 im Match durchgängig; unter 41
protokollierten Packed-Stichproben ist der aktuelle Guide 14-mal neuer als HDR,
während HDR nie neuer als Color ist. Der symmetrische Altersabstand akzeptierte
also unpassend neuere Inhalte. Tags sind Versionen, keine eindeutigen Frame-IDs.

Neuer isolierter Versuch: am HDR-Verbraucher die fertig geschriebene Guide-
Version in drei GPU-Slots erhalten, beim Flip nur Versionen <= HDR und mit
kleinem Abstand verwenden. Zu neuen Live-Guide nicht mehr wählen. Dazu exakt
schwarze tatsächliche Endbildpixel im Packed-Shader erhalten, damit dunkle
Menü-RGB-Werte nicht durch opake Startup-/Übergangsbilder scheinen. Keine neue
Schärfung/Farbkurve, kein Gemini-Endbildshader, keine Config-/FPS-/LOD-Änderung.
Zusätzlicher VRAM-/Kopieraufwand dokumentiert, Performance nicht garantiert.

Shader-/SPIR-V- und statische Prüfungen bestanden, C++-Library gebaut. Normale
4K-Test-Verknüpfung bleibt Ziel. Finalen EXE-/Loader-Status und künftigen
Nutzertest im [Frame-Pairing-Checkpoint](checkpoints/2026-10-01-frame-pairing/README.md)
prüfen; bisher keine bestätigte Liveheilung. Neue Diagnose protokolliert gewählten
Guide-Tag, HDR-Tag, History-Verwendung und erfolgreiche Snapshot-Aufrufe.


## 16.10.2026 - Operation 2D: 4K-Rekonstruktion des Schmutzrahmens und 2D-Overlays

Nutzer meldete nach Bildauswertung der Texturen auf dem Desktop:
- Schmutzrahmen / Blutspritzer / Vignetten an den Bildschirmrändern (Texturen 26160 / 20848) zeigten grobe 3x3-Treppenstufen (720p/704p-Blockpixel).
- Auto-Texturen (26495 mit Scheinwerfern und Reflektoren) sind perfekt und bleiben unberührt.
- Beide Feuerebenen auf Sweet Tooths Kopf (65607 hinten und 65675 vorne) sind aktiv und werden summiert in 4K gerendert.
- Auftrag: Schritt unter dem Namen **Operation 2D** durchführen, Checkpoints anlegen und Schmutzrahmen auf 4K rekonstruieren.

Umsetzung in TM4KNativeColour.hpp und TM4KHybrid.hpp:
display(vec2 pos) tastet nicht mehr mit 1-Pixel-Schritten innerhalb der 3x3-Pixel-Plateaus ab, sondern verwendet eine kontinuierliche 720p-Raster-Bikubik (Catmull-Rom mit Stride 3.0), die direkt über die Zentren des nativen 720p-Rasters interpoliert:
- 3x3-Klötzchenkanten auf Schmutzspritzern, Rändern und UI werden fließend geglättet.
- Kein Detailverlust bei HUD oder Schrift, saubere 4K-Rekonstruktion.
- Ramming-Fix (intra_gap 2500, cohort_window 4000, 2-Frame-4K-Latch) bleibt voll aktiv.
- HierLOD 0.50 (+100% Distanz / 2x Reichweite), CAS 8, LOD-Bias -0.20, Bäume 5000.

Build erfolgreich kompiliert (Release x64 MSBuild in 144.8s).
EXE-SHA256: 1B6877AF0298855DC96C58F8B1DB2DEE881C5B8CD1D28799BA4A281CAFF54FE1.
Launcher-Prüfung Start-TwistedMetal-4K-Echte-Dichte-Sichtweite-Test.ps1 -CheckOnly: PASS.
[Checkpoint-Dokumentation](checkpoints/2026-10-16-operation-2d/README.md).
