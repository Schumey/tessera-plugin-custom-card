# Custom card

A tile you put together yourself: values, bars and lines of words, drawn by the screen in its own fonts and colours.
What the card shows comes from one template sensor in Home Assistant, so a template can say anything: a room's climate
on one card, the house's power, what is open.

## Set up

1. Make a template sensor in Home Assistant whose attribute `elements` lists the card's parts (the example below goes
   into `configuration.yaml`, or a package, then reload Template entities).
2. Open Tessera, go to Plugins, add **Custom card** to a screen. The screen builds once.
3. In Layout, place the **Custom card** tile from the library's Plugins group, at any size, and choose the template
   sensor as its entity.

## An example

```yaml
template:
  - sensor:
      - name: Living room card
        state: "{{ states('sensor.living_room_temperature') }}"
        attributes:
          elements: >
            {% set t = states('sensor.living_room_temperature') | float(0) %}
            {% set h = states('sensor.living_room_humidity') | float(0) %}
            {% set c = states('sensor.living_room_co2') | int(0) %}
            {{ [
              'value|' ~ (t | round(1) | string | replace('.', ',')) ~ '|°C|Temperature||thermometer',
              'value|' ~ (h | round(0) | int) ~ '|%|Humidity|' ~ ('orange' if h < 40 else 'green' if h < 60 else 'blue') ~ '|water-percent',
              'bar|' ~ c ~ '|ppm|CO2|' ~ ('green' if c < 800 else 'yellow' if c < 1200 else 'red') ~ '|molecule-co2|400|2000'
            ] }}
```

## The parts

`elements` is a list of texts, one part each, its fields split by `|`. Fields at the end may be left out.

| Part | Fields | What it draws |
| --- | --- | --- |
| `value` | `value\|<value>\|<unit>\|<label>\|<colour>\|<icon>` | A large number with its unit, its label and icon above it. Values that follow each other stand side by side, four at the most. |
| `bar` | `bar\|<value>\|<unit>\|<label>\|<colour>\|<icon>\|<min>\|<max>` | The label with the value at its right, a bar from `min` (0) to `max` (100) under them. |
| `text` | `text\|<words>\|\|\|<colour>\|<icon>` | One line of words with an icon before it. |

- **colour**: `red`, `orange`, `yellow`, `green`, `mint`, `blue`, `purple`, `pink` or `gray`, the same in light and
  dark. As on Tessera's own cards it marks a part and never paints its words: it colours the icon (a dot where the part
  has no icon) and the bar. Work out levels in the template, as above.
- **icon**: a name from the editor's icon picker, with or without `mdi:`. Another icon shows as nothing.
- At most six parts, and each part at most 48 bytes: Tessera cuts longer texts. Keep labels short.
- The screen writes a value as the template renders it, so round it and choose the decimal sign there.

A sensor without `elements` shows its own state and unit, so the tile works with any sensor.

## Good to know

- The card is drawn again whenever the template sensor changes; Home Assistant renders the template.
- The value takes the largest of the screen's fonts that fits the tile, chosen for the shape of the value, so the
  card keeps its size when a digit changes. On a small tile the labels give way before the numbers (a value's icon
  then stands before it), and the tile's name goes last.
- A tile one row high and two or more columns wide puts the parts side by side, every part a column with its bar under
  its value; every other tile stacks them.
- The tile's background colour is set in the editor, as for every tile.
