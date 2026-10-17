# Architektur und Komponenten

Aktuelle Details: [Technischer Überblick](docs/TECHNICAL_NOTES.md), [Build](BUILD.md).

| Baustein | Eingabe | Aufgabe |
| --- | --- | --- |
| RPCS3-Quellmod | tracked-changes.patch + acht TM4K-Header | Vulkan-Renderziele, Farbrekonstruktion, Partikel, Timing und Zuschnitt |
| Game-Patches | configs/patches + patch_config.yml | Selektiver PostFX-Bypass, Community-60-FPS, SPU-MLAA aus |
| Host-Konfiguration | config_BCUS98106.yml | 300% Referenzskalierung, Vulkan und abgestimmte Optionen |
| Level-Overrides | Delta-Patcher und eigene Disc-Inputs | Ausgewählte Times-NGP-Knoten und zwei Distance-Blur-LTN-Werte |
| Launcher + Eingabe | sieben Prozessvariablen; eigenes Pads-Profil | Mod-Modi setzen und tatsächliches Gerät des Nutzers zuweisen |

## Wichtige Abgrenzungen

- Fünf Flags sind nicht fünf Shader. Der Quellumfang enthält acht Zusatzheader.
- 8.294.400 sind Ausgabepixel, nicht die Texelzahl jedes Materials.
- Windows-Grafiktreiber wird nicht verändert; Auto HDR ist getrennt.
- Globale Near-LOD-Experimente bleiben aus; sie verursachten schwarze Weltteile.
- CPU/GPU-/Controller-Marken allein sind kein Kompatibilitätsnachweis.
- Ein Dateicheck stellt keine universelle Diagnose, Bildfehlerfreiheit oder FPS fest.
- 10000/16000 und 5000 sind Spiel-/LOD-Werte, keine vermessenen Kilometer.

Die frühere Diagnosematrix enthielt unbewiesene pauschale Ursachen und
Hardwareversprechen. Sie ist in der lokalen Originalsammlung und der Git-Historie
nachvollziehbar, wird hier aber nicht als geprüfte Garantie fortgeschrieben.
