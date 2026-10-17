# Veröffentlichungsprüfung – 18. Oktober 2026

## Kurzfassung

Geminis Paket enthält die wesentlichen Patch-/Shader-/Konfigurationsdateien.
Es war aber noch keine fachlich durchgehend belastbare Veröffentlichung:
Darstellung, Sicherheitsratschläge, Hardwareversprechen und private Inventare
mussten geprüft und getrennt werden. Originaldossier bleibt unverändert;
die Überarbeitung entsteht in einer eigenen Veröffentlichungskopie.

## Korrigierte Aussagen

| Bisheriger Eindruck | Belastbare Formulierung |
| --- | --- |
| Weltweit erster, perfekter nativer 4K-Remaster | Fan-Mod mit 4K-Ausgabe/Rekonstruktion; Weltrekord und Fehlerfreiheit nicht nachgewiesen |
| Feste 60 FPS / null Eingabeverzögerung / 100% Stabilität | Bis zu 60 FPS als Ziel; keine kontrollierten frametime-/latenz-/kampagnenweiten Messungen |
| Zwei Agenten auf frischen PCs, sofort fehlerfrei | Getrennte Source-Builds auf vorhandenem Rechner; Toolchains/Abhängigkeiten teils vorhanden, dokumentierte CRT-/Shader-Modus-Korrekturen |
| Jede Textur hat 8,29 Millionen Pixel | 3840×2160 hat diese Ausgabepixelzahl; kein Nachweis über jedes Asset oder jeden Zwischenpuffer |
| Fünf Shader | Acht Zusatzheader mit mehreren Compute-Varianten; fünf Aktivierungsflags sind nicht die Shaderzahl |
| Bloom/PostFX komplett abgeschaltet | Selektiver elf-NOP-Bypass; weitergehender Raw-4K-Bypass bleibt aus |
| Garantiert 5 km Sichtweite überall | Geänderte Schwellwerte in Spielkoordinaten, ausgewählte Times-Knoten; keine vermessene Universalwirkung |
| Vulkan-Grafikkartentreiber modifiziert | User-Space-Renderer in RPCS3 modifiziert; Windows-Treiber unverändert |
| YouTube-Trailer schon online | Nur Kanal-URL verifiziert; kein spezifischer veröffentlichter Videolink |
| Feste Controllerprofile verhindern Windows-Abkopplung | Eigenes Gerät in Pads zuweisen; behauptete Windows-Kernelursache nicht belegt |
| Defender-Ausnahme/ExecutionPolicy löst Smart App Control | Kein allgemeiner Ersatz für Signierung und keine pauschale Sicherheitsabschaltung empfehlen |

## Inhalt und Zugriff

Öffentlich nötig: Basiscommit, tracked patch, acht Header/Overlay, Flags,
passende Game-/Patch-Konfiguration, Asset-Deltas, Patcher und Bauanleitung.
GitHub erhält nicht automatisch Zugriff auf das restliche Dossier oder deinen PC.
Gitignore schützt nur nicht-getrackte Dateien; bereits getrackte interne Daten
müssen ausdrücklich aus dem aktuellen Veröffentlichungsbaum genommen werden.

Nicht weiter im öffentlichen Baum: rohe Capture-/Checkpointinventare,
Registry-/Desktop-Link-Exporte, lokale CMake-/VS-Projektcaches, private
Controllerprofile, interner Übergabezettel und Rohdaten-Verzeichnisse.
Sie bleiben im ursprünglichen Desktop-Dossier erhalten.

Die frühere Git-Historie bleibt bestehen: Ausblenden im neuesten Commit entfernt
keine schon veröffentlichten alten Blobs. Die Prüfung sucht deshalb zusätzlich
nach Token-/Schlüsselmustern. Kein pauschales Versprechen, historische Daten
seien aus dem Internet gelöscht; kein Force-Push ohne eigene Entscheidung.

GitHub-Browser, Connector und Git-Zugang können unterschiedliche Konten nutzen.
Für Veröffentlichungen das beabsichtigte Konto und die Repositoryrechte prüfen.
Keine Zugangsdaten in Dateien oder Logs speichern. Ein öffentlicher Quellbaum
gibt Lesern keinen Zugriff auf den lokalen PC.

## Grenzen und weiteres Vorgehen

Keine Änderung an der funktionierenden EXE, installierten Patches oder Saves.
Quellcode-/Dokumentationsveröffentlichung, kein neuer Spieltest.
Für einen Binärrelease müssen zusätzlich Lizenz-/Abhängigkeitsdateien,
CPU-Ziel, Signaturstrategie, Paketinhalt und praktische Abnahme geprüft werden.
Andere Regionen, GPUs, Handhelds und Mehrspieler sind nicht zertifiziert.
