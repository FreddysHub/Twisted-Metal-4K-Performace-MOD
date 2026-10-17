<p align="center"><img src="docs/assets/twisted-metal-4k-remastered-fire.png" alt="Twisted Metal 4K — Remastered, Fan-Mod von FreddysHub" width="100%"></p>

<p align="center"><b>Ein Fanprojekt für schärfere Spielgrafik, mehr Ferndetails und bessere Performance auf RPCS3.</b></p>

<p align="center">
  <a href="BUILD.md"><b>Nachbauen</b></a> · <a href="docs/TECHNICAL_NOTES.md">Technik</a> ·
  <a href="docs/REPRODUCTION.md">Testnachweise</a> · <a href="README.md">English</a>
</p>

> [!IMPORTANT]
> **Quellcode und Bauanleitungen, kein fertiges Spiel.** Eigene rechtmäßig bezogene Spieldaten, passendes Update und PS3-Firmware erforderlich. Kein offizieller RPCS3-/Sony-Release. 3840 × 2160 und bis zu 60 FPS sind das Ziel, nicht auf jedem PC und in jeder Szene garantiert.

**Das PS3-Spiel ist nicht enthalten.** Jeder muss Twisted Metal selbst besitzen
und den passenden eigenen Spieldump bereitstellen. Keine Spieldateien, Updates,
Entschlüsselungsschlüssel oder PS3-Firmwaredateien werden hier hochgeladen
oder als Spieldownload verlinkt.

[Datenschutzprüfung und Abgrenzung der Spieldateien](docs/PRIVACY.md).

## Was wir verändert haben

| Bereich | Umsetzung |
| --- | --- |
| **Spielgrafik** | Vulkan-Compute-Pässe verbinden hochauflösende Szenendetails mit den niedriger aufgelösten Farb-/Ausgabepuffern. |
| **Performance** | Selektiver PostFX-Bypass mit elf NOPs, Community-60-FPS-Patch und deaktivierte SPU-MLAA. Nicht einfach „Bloom komplett aus“. |
| **Ferndetails** | LOD-Deltas in `times.ngp` der Times-Archive. Globale Near-LOD-Experimente bleiben aus: Sie verursachten in unseren Tests schwarze Straßen und fehlende Weltteile. |
| **Vollbild** | Einheitlicher Zuschnitt entfernt den Innenrand; Timing-, Farb- und Partikelpfade behandeln beobachtete Darstellungsprobleme. |
| **Nachbau** | Basiscommit, Quellpatch, acht Header, fünf Flags, Konfiguration und Asset-Patcher. |

**4K verständlich:** 3840 × 2160 sind **8.294.400 Ausgabepixel**. Das setzt nicht jede Textur auf acht Millionen Pixel. Texturen, Schatten, Videos und manche Zwischenpuffer behalten ursprüngliche Grenzen. Kein KI-Texturpaket enthalten.

## Einblick in die Entwicklung

| Helles Level · Frame 3698 | Stadt · Frame 6060 |
| --- | --- |
| ![Helles Level](docs/assets/development-village.png) | ![Stadt](docs/assets/development-city.png) |

Vorhandene RenderDoc-Replay-Exporte in 1920 × 1080. Keine neuen Screenshots des letzten Nachbaus und kein FPS-Nachweis. [Bildherkunft](docs/assets/README.md).

🎬 **[YouTube-Kanal von FreddysHub](https://www.youtube.com/channel/UCuaFl468uckb1ayU8lqgaQQ)** — konkreter veröffentlichter Trailer-Link noch nicht verifiziert.

## So baust du es nach

1. US-Version **BCUS98106**, Update **01.06**, PPU-Hash prüfen.
2. Separaten Emulator bauen: [aktuelle englische Anleitung](BUILD.md) oder [deutsche Anleitung](03_MANUELLE_SCHRITT_FUER_SCHRITT_ANLEITUNG.md). Funktionierende Installation und Spielstände erhalten.
3. Logos, Menü, Fahrzeuge, Feuer, helle/dunkle Level, schnelle Fahrt und Fernobjekte testen.

Es gibt **keinen angehängten vorkompilierten Release**. „Download ZIP“ lädt Quellen, nicht Emulator, Spiel oder Firmware.

## Was ist belegt?

Ein getrennter Quell-Neubau auf dem Entwicklungs-PC bestand **38 Shader-Varianten**,
**46 Rezept-/Runtimeprüfungen** und den Loader-Test. Beide Times-Archive haben die
erwarteten Prüfsummen. Installierte Werkzeuge/Abhängigkeiten wurden verwendet:
kein vollständiger Neu-PC-Nachweis.

Spätere Test-4-Notizen dokumentieren Menüstart und Nutzer-Spieltests, auch
anfängliche Grafikfehler. Getrennte Läufe, keine Kampagnenabnahme oder Prüfung
aller GPU-Hersteller. [Teststatus](docs/REPRODUCTION.md).

Der Windows-Grafikkartentreiber wird **nicht verändert**. Modifiziert wird
RPCS3s Vulkan-Renderer. Windows Auto HDR ist getrennt davon. Keine globalen
Sicherheitsfunktionen für den Nachbau abschalten.

## Dokumentation & Geschichte

- [Technik: PostFX, Farben, LOD und Pixelzahlen](docs/TECHNICAL_NOTES.md)
- [Veröffentlichungsprüfung und Korrekturen](docs/PUBLICATION_AUDIT.de.md)
- [KI-Neubau-Prompt](02_PROMPT_FUER_KI_NEUBAU.md)
- [Projektchronik](01_PROJEKTCHRONIK_UND_HISTORIE.md)
- [Test-4-Erkenntnisse](NACHTRAG_TEST4_FUER_GEMINI.md)
- [Fehler melden und mitarbeiten](CONTRIBUTING.md)

Ältere Notizen können überholte Thesen enthalten; aktuelle Bauanleitung und
Veröffentlichungsprüfung haben Vorrang.

## Credits

**FreddysHub:** Initiative, Beobachtungen, Testfahrten und Qualitätskontrolle.
KI-gestützte Arbeit mit **Google Gemini / Antigravity** und **OpenAI ChatGPT / Codex**.
Grundlage: **RPCS3-Team und Mitwirkende**. Community-Patches/Forschung:
**illusion**, **Whatcookie** und weitere Autoren in den Definitionen.
GPU-Analyse: **RenderDoc**. Keine offizielle Empfehlung dieser Organisationen.

### Unterstütze die nächste Testrunde

**[Freiwillig via PayPal unterstützen](https://www.paypal.me/FedorTempel)** ·
**[YouTube](https://www.youtube.com/channel/UCuaFl468uckb1ayU8lqgaQQ)**

Spenden sind keine Voraussetzung. PC-Port, Online-Wiederbelebung und
Handheld-Profile sind Ideen, keine zugesagten Leistungen.

---

[Lizenz](LICENSE) · [Herkunft und Rechtehinweise](NOTICE.md).
Keine Spieldateien, Firmware oder Emulator-Binärdateien im öffentlichen Paket.
