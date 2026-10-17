@AGENTS.md

# Stand der Arbeit (Fork von Schumey, Stand 2026-10-17, nach PR #2)

Notizen für die Arbeit an diesem Fork. Diese Datei gehört nicht in einen Pull Request an StofLE: nicht auf `arc`
committen (siehe unten).

## Repos und Branches

- `origin` ist der Fork `Schumey/tessera-plugin-custom-card`, `upstream` das Original `StofLE/tessera-plugin-custom-card`.
  Das Plugin steht schon in der Community-Liste von `tessera-plugins` (`community/custom_card.yaml`, ohne `ref`) und
  zeigt auf StofLEs Repo; neue Versionen dort brauchen keinen PR an `tessera-plugins`.
- `main` (Fork): 0.7.0 als `0efe050`, auf GitHub. Seine Historie ist unsauber: ein Merge `4acdca2` hat den alten
  0.6.0-Commit `2a2d963` mit falschem Autor (StofLE) wieder hereingeholt. Nicht umschreiben, für den PR nicht nehmen.
- `arc`: der saubere Branch für den PR an `StofLE:main`, auf `upstream/main` (`dae1285`, 0.5.0) mit drei Commits von
  Schumey: `fb497b9` 0.6.0, `32ef6b9` 0.6.1, `7f586f3` 0.7.0 (Cherry-Pick von `0efe050`; der früher notierte
  `e6602e4` war in keinem Klon mehr vorhanden). Gepusht. Die Dateien sind bis auf diese CLAUDE.md identisch mit `main`.
- PR: StofLE/tessera-plugin-custom-card#2 (https://github.com/StofLE/tessera-plugin-custom-card/pull/2), offen seit
  2026-10-17, `Schumey:arc` → `StofLE:main`.
- `upstream/next` (StofLE, 2026-10-14) hat schon einen eigenen Part `ring` mit demselben Feldaufbau, ebenfalls als
  0.6.0, dazu Tabellen-Zeilen, Badges und PSRAM. `arc` und `next` haben Konflikte in sechs Dateien. Der PR weist
  darauf hin und bietet an, die Teile, die `next` fehlen (Wert in der Ringmitte, die zwei Optionen), auf `next` neu
  aufzusetzen.
- Git-Autor in diesem Repo: `Schumey <avshsk@gmail.com>` (lokale `git config`).

## Was gebaut wurde (0.6.0 bis 0.7.0)

- Neue Part-Art `arc`: `arc|<Wert>|<Einheit>|<Label>|<Farbe>|<Icon>|<min>|<max>`, ein Dreiviertelring (`lv_arc`,
  unten offen, 0 bis 1000), gefüllt von min bis max in der Farbe bzw. dem Tone des Teils.
- Wert und Einheit stehen mittig im Ring; der Ring ist so groß, dass die Form des Werts hineinpasst (`arc_size`),
  mindestens `ui::px(48)`. Alle Ringe einer Zeile sind so groß wie der größte.
- Icon und Label stehen zentriert unter dem Ring.
- Wo kein Ring passt (zu niedrige Kachel), zeigt das Layout die Ringe als normale Werte mit Icon oder Farbpunkt davor
  (`plain_arcs_`).
- Sind Ringe dabei, probiert das Layout zusätzlich zwei bzw. einen Wert pro Zeile (`stacked(2)`, `stacked(1)`).
- Kachel-Optionen im Inspector (0.7.0):
  - `show_name` (an): der Name der Kachel über den Teilen;
  - `coloured_values` (aus): Zahlen in der Farbe ihres Teils, lesbar gemacht mit `readable()`. Die Detail-Karte bekommt
    keine Optionen und bleibt bei normaler Schriftfarbe.
- `memory` der Kachel: 6800 (geschätzt, nicht gemessen).

## Entscheidungen

- Ein Ring ohne Icon behält immer sein Label, wo es irgend passt: Die Ringfarbe sagt, wie gut ein Wert ist, nicht was
  er ist.
- Labels unter den Ring statt daneben: So kosten sie nur Höhe, nicht die Breite, die eine Reihe Ringe braucht.
- Farbige Zahlen nur als Option, standardmäßig aus: Das Plugin färbt sonst bewusst nur Markierungen, nie Worte.
  Für andere Nutzer ändert sich so nichts.
- Versionen in kleinen Schritten mit Changelog je Version (`CHANGELOG.md` und `CHANGELOG.de.md`); README auf Englisch
  und Deutsch pflegen.

## Offene Punkte

- Antwort von StofLE auf PR #2 abwarten, vor allem zur Überschneidung mit `ring` auf `next`.
- Die Detail-Karte zeigt auf der Bench „Choose a sensor“. Das war schon vor allen Änderungen so (mit dem
  unveränderten Stand nachgeprüft), im `store.png` zeigt sie aber Werte.
- Auf 1x1 mit zwei Ringen ist es eng; früher fehlte dort der zweite Ring kommentarlos, nach 0.6.1 passen auf dem
  Guition beide. Auf anderen Bildschirmen nicht einzeln geprüft.

## Nächste Schritte

Erledigt am 2026-10-17: `arc` gepusht, 0.7.0 auf dem Guition 4″ getestet (läuft, mit beiden Optionen), Bench
`--strict` auf vier Bildschirmen (cyd, guition, waveshare43-portrait, jc8012p4a1) mit 0 Warnungen (nur Info-Hinweise
zu Rastergrößen), PR #2 eröffnet.

1. Auf StofLEs Antwort zu PR #2 reagieren. Will er seinen `ring` behalten: einen neuen Branch auf `upstream/next`
   anlegen und nur den Wert in der Ringmitte und die Optionen `show_name` und `coloured_values` dorthin übertragen.
2. Optional ein Foto vom Gerät im PR nachreichen.

## Werkzeuge auf diesem Rechner (Windows)

- In PowerShell gibt es kein `git`, im Git Bash von Claude Code aber schon; dort klappen auch `git push` und `gh`
  (StofLE-PRs mit `gh pr … -R StofLE/tessera-plugin-custom-card`). Das Git in WSL (Ubuntu) immer mit
  `git -c core.autocrlf=true …` aufrufen, sonst sieht es wegen der Windows-Zeilenenden fast jede Datei als geändert.
  Ubuntu ist bei GitHub nicht angemeldet.
- Die Bench braucht in WSL `libsdl2-dev` (seit 2026-10-17 installiert). Die erste Bench nach einem leeren Cache baut
  das venv mit ESPHome neu (einige Minuten).
- Den Autor oder die Historie eines schon gepushten Commits umschreiben (amend, Force-Push) ist für Claude gesperrt;
  das macht Schumey selbst.
- `check.py` geht aus PowerShell (`python ../tessera-plugins/tools/check.py .`); es schreibt dann `: ok` statt
  `custom_card: ok`, weil der Pfad `.` ist.
- Bench und `store_image.py` direkt in WSL starten. Aus PowerShell mit `python ../tessera-plugins/tools/preview.py`
  zerlegt der WSL-Wrapper den relativen Pfad:
  `wsl -e bash -lc "cd /mnt/c/Users/avshs/Documents/GitHub/tessera-plugin-custom-card && python3 ../tessera-plugins/tools/preview.py . --strict"`
- Nur eine Bench gleichzeitig: Sie teilen sich Port und Build-Ordner (`~/.cache/tessera-plugins/bench-build`). Schumey
  benchet nebenher oft `../SCMT`; dann warten, bis es fertig ist:
  `while pgrep -f 'tools/[b]ench.py' >/dev/null; do sleep 10; done` (die eckigen Klammern verhindern, dass die Schleife
  sich selbst findet). Zwei gleichzeitige Builds desselben Boards brechen mit fehlenden `.o`-Dateien ab.
- `store_image.py --from .preview` nimmt nur die Bildschirme der letzten Bench. Nach einem Lauf mit `--board` deshalb
  erst die volle Bench oder `store_image.py` ohne `--from`.
- Ein Ordnername ist für die Bench die Plugin-Id: Eine Kopie zum Vergleichen muss in einem Ordner `custom_card` liegen.
