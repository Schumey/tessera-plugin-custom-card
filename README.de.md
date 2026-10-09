# Eigene Karte

Ein Tile, das du selbst zusammenstellst: Werte, Balken und Textzeilen, gezeichnet vom Bildschirm in seinen eigenen
Schriften und Farben. Was die Karte zeigt, kommt aus einem Template-Sensor in Home Assistant. Ein Template kann alles
sagen: das Raumklima eines Zimmers auf einer Karte, den Strom im Haus, was offen ist.

## Einrichten

1. Lege in Home Assistant einen Template-Sensor an, dessen Attribut `elements` die Teile der Karte aufzählt (das
   Beispiel unten gehört in die `configuration.yaml` oder ein Package; danach die Template-Entitäten neu laden).
2. Öffne Tessera, geh zu Plugins und füge **Eigene Karte** einem Bildschirm hinzu. Der Bildschirm baut einmal neu.
3. Platziere im Layout das Tile **Eigene Karte** aus der Gruppe Plugins der Bibliothek, in jeder Größe, und wähle den
   Template-Sensor als Entität.

## Ein Beispiel

```yaml
template:
  - sensor:
      - name: Wohnzimmer Karte
        state: "{{ states('sensor.wohnzimmer_temperatur') }}"
        attributes:
          elements: >
            {% set t = states('sensor.wohnzimmer_temperatur') | float(0) %}
            {% set h = states('sensor.wohnzimmer_luftfeuchtigkeit') | float(0) %}
            {% set c = states('sensor.wohnzimmer_co2') | int(0) %}
            {{ [
              'value|' ~ (t | round(1) | string | replace('.', ',')) ~ '|°C|Temperatur||thermometer',
              'value|' ~ (h | round(0) | int) ~ '|%|Feuchte|' ~ ('orange' if h < 40 else 'green' if h < 60 else 'blue') ~ '|water-percent',
              'bar|' ~ c ~ '|ppm|CO2|' ~ ('green' if c < 800 else 'yellow' if c < 1200 else 'red') ~ '|molecule-co2|400|2000'
            ] }}
```

## Die Teile

`elements` ist eine Liste von Texten, ein Teil je Text, die Felder getrennt durch `|`. Felder am Ende dürfen fehlen.

| Teil | Felder | Was er zeichnet |
| --- | --- | --- |
| `value` | `value\|<Wert>\|<Einheit>\|<Label>\|<Farbe>\|<Icon>` | Eine große Zahl mit ihrer Einheit, darüber Label und Icon. Aufeinanderfolgende Werte stehen nebeneinander, höchstens vier. |
| `bar` | `bar\|<Wert>\|<Einheit>\|<Label>\|<Farbe>\|<Icon>\|<min>\|<max>` | Das Label mit dem Wert rechts daneben, darunter ein Balken von `min` (0) bis `max` (100). |
| `text` | `text\|<Worte>\|\|\|<Farbe>\|<Icon>` | Eine Zeile Text mit einem Icon davor. |

- **Farbe**: `red`, `orange`, `yellow`, `green`, `mint`, `blue`, `purple`, `pink` oder `gray`, in Hell und Dunkel
  gleich. Wie auf Tesseras eigenen Karten markiert sie einen Teil und färbt nie seine Worte: Sie färbt das Icon (einen
  Punkt, wo der Teil kein Icon hat) und den Balken. Stufen berechnest du im Template, wie oben.
- **Icon**: ein Name aus der Icon-Auswahl des Editors, mit oder ohne `mdi:`. Ein anderes Icon bleibt leer.
- Höchstens sechs Teile, und jeder Teil höchstens 48 Bytes: Tessera kürzt längere Texte. Halte Labels kurz.
- Der Bildschirm schreibt einen Wert so, wie das Template ihn liefert. Runde ihn dort und wähle dort das Dezimalzeichen.

Ein Sensor ohne `elements` zeigt seinen eigenen Zustand und seine Einheit, so passt das Tile zu jedem Sensor.

## Gut zu wissen

- Die Karte wird neu gezeichnet, sobald sich der Template-Sensor ändert; Home Assistant rendert das Template.
- Der Wert bekommt die größte Schrift des Bildschirms, die ins Tile passt, gewählt nach der Form des Werts, so dass
  die Karte ihre Größe behält, wenn sich eine Ziffer ändert. Auf einem kleinen Tile weichen zuerst die Labels (das Icon
  eines Werts steht dann vor der Zahl), die Zahlen bleiben, und der Name des Tiles geht zuletzt.
- Ein Tile, das eine Zeile hoch und zwei oder mehr Spalten breit ist, stellt die Teile nebeneinander, jeden als Spalte
  mit seinem Balken unter dem Wert; jedes andere Tile stapelt sie.
- Die Hintergrundfarbe des Tiles stellst du im Editor ein, wie bei jedem Tile.
