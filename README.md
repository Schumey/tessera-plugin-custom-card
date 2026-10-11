# Custom card

A tile you put together yourself: values, bars and lines of words that a template sensor in Home Assistant describes,
drawn by the screen in its own fonts and colours. A room's climate on one tile, the house's power, what is open:
whatever a template can say.

On a tile one row high and two columns or more, the parts stand side by side, each with its label and icon above it
and a bar under its value. Any other tile stacks them: values side by side, a bar or a line of words across the width.
The values take the largest face that fits beside the tile's name, on a large tile the climate card's large digits
when every value is a number; the labels above them stay only where they cost the values nothing, else each icon
stands before its number. A label without an icon or a colour always stays.
A tap opens the parts large on a card over the page, with their labels.

## Set up

1. In Home Assistant, make a template sensor whose attribute `elements` lists the card's parts, one text per part
   (below), put it in `configuration.yaml`, and reload Template entities. [examples/living_room.yaml](https://github.com/StofLE/tessera-plugin-custom-card/blob/main/examples/living_room.yaml)
   is a larger one, with levels for the humidity and the CO2.
2. In Tessera, open **Plugins**, choose **Custom card** and tick your screens. Each screen builds once.
3. In Layout, place the **Custom card** tile from the library's Plugins group, at any size, and choose the template
   sensor in its inspector. It offers the sensors that have an `elements` attribute.

## An example

```yaml
template:
  - sensor:
      - name: Room card
        state: "{{ states('sensor.room_co2') }}"
        attributes:
          elements: >
            {% set t = states('sensor.room_temp') %}
            {% set c = states('sensor.room_co2')
                 | int(0) %}
            {{ [
              'value|' ~ t ~ '|°C|Temperature',
              'bar|' ~ c ~ '|ppm|CO2|'
                ~ ('green' if c < 800 else 'red')
                ~ '|molecule-co2|400|2000'
            ] }}
```

## The parts

Each part is one text, its fields split by `|`, in this order: the kind, the value, the unit, the label, the colour,
the icon, and for a bar its lowest and highest value. Fields at the end may be left out.

- `value`: a number with its unit, such as `value|21.4|°C|Temperature||thermometer`.
- `bar`: a number with a bar under it, from its lowest to its highest value (0 and 100 when left out), such as
  `bar|820|ppm|CO2|yellow||400|2000`.
- `text`: a line of words, such as `text|Door open||||door-open`.

A colour is `red`, `orange`, `yellow`, `green`, `mint`, `blue`, `purple`, `pink` or `gray`. As on Tessera's own tiles it
marks a part and never paints its words: it colours the icon (a dot where the part has none) and the bar. The colours
that say how good a level is are Tessera's own, readable in light and dark: `green` good, `yellow` and `orange`
moderate, `red` bad, `blue` the accent. An icon is a name from the editor's icon picker, with or without `mdi:`.

## Good to know

- Home Assistant renders the template; the tile changes whenever the sensor does. Work out levels and colours in the
  template, round values there, and write the decimal sign you want.
- At most six parts, each at most 48 bytes: Tessera cuts longer texts, so keep labels short.
- The face of a value is chosen for its shape, not its digits, so the tile keeps its size when a value ticks.
- When the sensor is unavailable for a while, the tile keeps it and shows its state until `elements` is back.
- The tile's background colour is set in the editor, as for every tile.

## How it works

- `tessera-plugin.yaml`: one tile that belongs to a sensor and gets its `elements` and `unit_of_measurement`.
- `components/custom_card/`: the ESPHome component that lays the parts out and draws them, on the tile and on the
  card a tap opens.
- `translations/`: the words on the screen (`screen`) and in the app (`app`).
- `CHANGELOG.md`: what each version changed; the app shows it with an update.
- `tools/make_icons.py`: writes the names of Tessera's icon set into `icons.h`.
