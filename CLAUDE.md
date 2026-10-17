@AGENTS.md

# Stand der Arbeit (Fork von Schumey, Stand 2026-10-17)

Notizen für die Arbeit an diesem Fork. Diese Datei gehört nicht in einen Pull Request an StofLE: nicht auf `arc`
committen (siehe unten).

## Repos und Branches

- `origin` ist der Fork `Schumey/tessera-plugin-custom-card`, `upstream` das Original `StofLE/tessera-plugin-custom-card`.
  Das Plugin steht schon in der Community-Liste von `tessera-plugins` (`community/custom_card.yaml`, ohne `ref`) und
  zeigt auf StofLEs Repo; neue Versionen dort brauchen keinen PR an `tessera-plugins`.
- `main` (Fork): 0.7.0 als `0efe050`, auf GitHub. Seine Historie ist unsauber: ein Merge `4acdca2` hat den alten
  0.6.0-Commit `2a2d963` mit falschem Autor (StofLE) wieder hereingeholt. Nicht umschreiben, für den PR nicht nehmen.
- `arc`: der saubere Branch für den PR an `StofLE:main`, auf `upstream/main` (`dae1285`, 0.5.0) mit drei Commits von
  Schumey: `fb497b9` 0.6.0, `32ef6b9` 0.6.1, `e6602e4` 0.7.0. Die Dateien sind identisch mit `main`
  (`git diff main arc` ist leer). `e6602e4` ist noch nicht gepusht.
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

- 0.7.0 ist noch nicht auf dem echten Gerät getestet (Guition 4″). Getestet war nur der Stand vor dem Wert in der
  Ringmitte; dabei fehlten auf 2x2 die Labels, was 0.7.0 beheben soll.
- `arc` ist noch nicht mit `e6602e4` gepusht; ob der PR an StofLE schon offen ist, ist nicht bestätigt.
- Die Detail-Karte zeigt auf der Bench „Choose a sensor“. Das war schon vor allen Änderungen so (mit dem
  unveränderten Stand nachgeprüft), im `store.png` zeigt sie aber Werte.
- Auf 1x1 mit zwei Ringen ist es eng; früher fehlte dort der zweite Ring kommentarlos, nach 0.6.1 passen auf dem
  Guition beide. Auf anderen Bildschirmen nicht einzeln geprüft.

## Nächste Schritte

1. In GitHub Desktop `arc` wählen und pushen (**Push origin**), dann den PR prüfen: base `StofLE/...:main` ← head
   `Schumey/...:arc`, genau drei Commits von Schumey.
2. Release 0.7.0 im Fork machen, auf dem Gerät testen; im Inspector **Namen zeigen** aus- und **Werte in ihrer Farbe**
   einschalten. Ein Foto für den PR machen.
3. Im PR beschreiben: Syntax mit Beispiel, Bench auf vier Bildschirmen ohne Warnung, auf welchem Gerät getestet,
   Hinweis auf die Detail-Karte auf der Bench, Versionssprung auf 0.7.0 (StofLE kann anders zählen).

## Werkzeuge auf diesem Rechner (Windows)

- In PowerShell gibt es kein `git`; Git läuft in WSL (Ubuntu). Immer mit `git -c core.autocrlf=true …`, sonst sieht
  das WSL-Git wegen der Windows-Zeilenenden fast jede Datei als geändert. Gepusht wird über GitHub Desktop: Ubuntu ist
  bei GitHub nicht angemeldet.
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
