# Übergabe an Gemini – Erkenntnisse aus Test 4

Bitte diese Punkte in die Neubau-Anleitung und das spätere GitHub-Paket übernehmen. Der Master-Prompt `02_PROMPT_FUER_KI_NEUBAU.md` und `03_QUELLCODE/Verify-TM4K-Setup.ps1` im Desktop-Dossier wurden bereits entsprechend korrigiert.

## 1. Mein Fehler bei der Wahl des Shader-Modus

Ich wählte die optionale Interpreter-Variante aus dem Prompt, statt die mitgelieferte Standardkonfiguration zu verwenden. Die fertigen Tests 1/2 und 3 nutzen:

```yaml
Video:
  Shader Mode: Async Recompiler (multi-threaded)
```

Dieser Wert ist jetzt auch in Test 4 aktiv. Die gesamte Spielkonfiguration stimmt per SHA256 mit diesen Referenzen überein. Es wurden dafür keine EXE-, DLL- oder Cachedateien aus den Referenzversionen übernommen.

## 2. Fehlerhafter Optionsname und fehlende Warnung im Prompt

`Async with Shader Interpreter` ist im festgelegten RPCS3-Commit kein gültiger YAML-Wert. Ich übernahm ihn ungeprüft. Das Laufzeitlog meldete `invalid enum or integer`; anschließend war der Standard `Async Recompiler with Shader Interpreter` aktiv.

Der Interpreter-Modus löst die zusätzliche Vorbereitung allgemeiner Vulkan-Interpreter-Pipelines aus. Genau dazu gehörte die beanstandete Anzeige mit 6.630 Basisvarianten. Die PPU-Vorkompilierung, der eigentliche Spiel-Shader-Cache und diese Interpreter-Vorbereitung müssen in der Anleitung getrennt erklärt werden.

Der korrekt geschriebene optionale Wert wäre `Async Recompiler with Shader Interpreter`. Auch damit bleibt die zusätzliche Vorbereitung aktiv. Für den regulären Neubau deshalb beim normalen Async-Recompiler bleiben. Der Interpreter darf nicht mit einem garantierten schnellen oder vollständig bildfehlerfreien Erststart beworben werden.

## 3. Der ursprüngliche 8/8-Check war keine Funktionsabnahme

Er erkannte den falschen Shader-Modus nicht. Das habe ich vor dem ersten Spielstart nicht ausreichend gegengeprüft. Der Prüfer kontrolliert jetzt zusätzlich den normalen Shader-Modus. Ein Gegenversuch mit der alten Einstellung wird gezielt abgewiesen.

Außerdem wurde seine pauschale Ausgabe „zu 100% startklar“ ersetzt. Dateivorhandensein und YAML-Prüfungen beweisen weder die effektiv geladene Konfiguration noch funktionierende Controller-Eingaben oder Bildfehlerfreiheit. Auch Laufzeitlog und Spieltest sind erforderlich.

## 4. Controller: Windows-Verbindung genügt nicht

Ich ließ die frische Installation zunächst bei der Standard-Tastatursteuerung und übergab sie ohne nachgewiesene Gamepad-Zuweisung. Der Xbox-Wireless-Controller war bereits in Windows verbunden. In RPCS3 fehlte die Zuordnung für Spieler 1.

Behoben über `Pads → Player 1 → XInput → verbundenes Gerät → Save`, hier lokal `XInput Pad #1`. Das Laufzeitlog bestätigt das Laden des Profils und `XInput device 0 connected`. Die konkrete Gerätenummer muss auf jedem PC geprüft werden. Bestehende Profile erhalten; keine fremden festen Geräteprofile verteilen. Eine erneute Windows-Kopplung war hier nicht nötig.

Die ältere Behauptung im Prompt, fremde Profil-IDs würden automatisch Windows-Controller entkoppeln, wurde in diesem Test nicht belegt und entsprechend korrigiert.

## 5. Ergebnis und verbleibende Grenze

Test 4 startet über seine Desktop-Verknüpfung bis ins Hauptmenü. Der erwartete PPU-Hash wird erkannt, die 4K-Fusion ist laut Log mit `3840x2160` aktiv, und beide gepatchten Asset-Prüfsummen stimmen.

Der Nutzer hat anschließend selbst gespielt: Anfangs gab es kleine Grafikfehler, danach lief es nach seiner Aussage normal. Erster Cache-Aufbau ist eine mögliche Erklärung, aber keine bestätigte Ursache. Dieselbe Szene beim zweiten Start prüfen; wiederkehrende Fehler gesondert untersuchen. Eine vollständige Abnahme beider vorgesehenen Levels oder Langzeitstabilität wurde damit nicht dokumentiert.

Die wesentlichen Korrekturen betrafen Konfiguration, Anleitung und Erststartprüfung. Für die Behebung der langen Interpreter-Vorbereitung war keine Änderung am Mod-Quellcode erforderlich.
