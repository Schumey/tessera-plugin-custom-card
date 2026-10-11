# Eigene Karte

Ein Tile, das du selbst zusammenstellst: Werte, Balken und Textzeilen, die ein Template-Sensor in Home Assistant
beschreibt, gezeichnet vom Bildschirm in seinen eigenen Schriften und Farben. Das Raumklima eines Zimmers auf einem
Tile, der Strom im Haus, was offen ist: alles, was ein Template sagen kann.

Auf einem Tile, das eine Zeile hoch und zwei oder mehr Spalten breit ist, stehen die Teile nebeneinander, jeder mit
Label und Icon darüber und einem Balken unter dem Wert. Jedes andere Tile stapelt sie: Werte nebeneinander, ein Balken
oder eine Textzeile über die ganze Breite. Die Werte bekommen die größte Schrift, die neben dem Namen des Tiles passt,
auf einem großen Tile die großen Ziffern der Klimakarte, wenn alle Werte Zahlen sind; die Labels darüber bleiben nur,
wo sie die Werte nichts kosten, sonst steht jedes Icon vor seiner Zahl.
Ein Label ohne Icon und ohne Farbe bleibt immer.
Ein Tippen öffnet die Teile groß auf einer Karte über der Seite, mit ihren Labels.

## Einrichten

1. Lege in Home Assistant einen Template-Sensor an, dessen Attribut `elements` die Teile der Karte aufzählt, einen
   Text je Teil (unten), füge ihn in die `configuration.yaml` ein und lade die Template-Entitäten neu.
   [examples/living_room.yaml](https://github.com/StofLE/tessera-plugin-custom-card/blob/main/examples/living_room.yaml)
   ist ein größeres Beispiel, mit Stufen für Feuchte und CO2.
2. Öffne in Tessera **Plugins**, wähle **Eigene Karte** und hake deine Bildschirme an. Jeder Bildschirm baut einmal neu.
3. Platziere im Layout das Tile **Eigene Karte** aus der Gruppe Plugins der Bibliothek, in jeder Größe, und wähle im
   Inspector den Template-Sensor. Er bietet die Sensoren an, die ein Attribut `elements` haben.

## Ein Beispiel

```yaml
template:
  - sensor:
      - name: Raumkarte
        state: "{{ states('sensor.room_co2') }}"
        attributes:
          elements: >
            {% set t = states('sensor.room_temp') %}
            {% set c = states('sensor.room_co2')
                 | int(0) %}
            {{ [
              'value|' ~ t ~ '|°C|Temperatur',
              'bar|' ~ c ~ '|ppm|CO2|'
                ~ ('green' if c < 800 else 'red')
                ~ '|molecule-co2|400|2000'
            ] }}
```

## Die Teile

Jeder Teil ist ein Text, die Felder getrennt durch `|`, in dieser Reihenfolge: die Art, der Wert, die Einheit, das
Label, die Farbe, das Icon und bei einem Balken sein kleinster und größter Wert. Felder am Ende dürfen fehlen.

- `value`: eine Zahl mit Einheit, etwa `value|21,4|°C|Temperatur||thermometer`.
- `bar`: eine Zahl mit einem Balken darunter, vom kleinsten bis zum größten Wert (ohne Angabe 0 und 100), etwa
  `bar|820|ppm|CO2|yellow||400|2000`.
- `text`: eine Textzeile, etwa `text|Tür offen||||door-open`.

Eine Farbe ist `red`, `orange`, `yellow`, `green`, `mint`, `blue`, `purple`, `pink` oder `gray`. Wie auf Tesseras
eigenen Tiles markiert sie einen Teil und färbt nie seine Worte: Sie färbt das Icon (einen Punkt, wo der Teil keins hat)
und den Balken. Die Farben, die sagen, wie gut ein Wert ist, sind Tesseras eigene, hell und dunkel lesbar: `green`
gut, `yellow` und `orange` mäßig, `red` schlecht, `blue` die Akzentfarbe. Ein Icon ist ein Name aus der Icon-Auswahl des
Editors, mit oder ohne `mdi:`.

## Gut zu wissen

- Home Assistant rendert das Template; das Tile ändert sich, sobald sich der Sensor ändert. Stufen und Farben
  berechnest du im Template, dort rundest du die Werte und wählst das Dezimalzeichen.
- Höchstens sechs Teile, jeder höchstens 48 Bytes: Tessera kürzt längere Texte, also Labels kurz halten.
- Die Schrift eines Werts richtet sich nach seiner Form, nicht nach den Ziffern, so behält das Tile seine Größe, wenn
  sich ein Wert ändert.
- Ist der Sensor eine Weile nicht verfügbar, behält das Tile ihn und zeigt seinen Zustand, bis `elements` zurück ist.
- Die Hintergrundfarbe des Tiles stellst du im Editor ein, wie bei jedem Tile.

## So funktioniert es

- `tessera-plugin.yaml`: ein Tile, das zu einem Sensor gehört und dessen `elements` und `unit_of_measurement` bekommt.
- `components/custom_card/`: die ESPHome-Komponente, die die Teile anordnet und zeichnet, auf dem Tile und auf der
  Karte, die ein Tippen öffnet.
- `translations/`: die Worte auf dem Bildschirm (`screen`) und in der App (`app`).
- `CHANGELOG.md`: was jede Version geändert hat; die App zeigt es bei einem Update.
- `tools/make_icons.py`: schreibt die Namen von Tesseras Icon-Satz in `icons.h`.
