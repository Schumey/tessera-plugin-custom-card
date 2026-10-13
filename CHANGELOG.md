# Changelog

## 0.5.0
- Large numbers on a large tile: where they fit and every value is a number, the values take the large digits of
  Tessera's climate card. A value with a word in it keeps the face it had.
- Green, yellow, orange, red and blue are Tessera's own colours by meaning (good, moderate, bad, the accent), readable
  on the card in light and dark, as Tessera colours a level.
- Where nothing fits a small tile, its values stand two to a row, or one, instead of being cut short in one row.
- A text beside other parts on a flat tile keeps its words whole where the parts can stand under each other
  instead; labels still come first.
- Needs Tessera with plugin API 0.9 (the release after 0.4.87).

## 0.4.3 - 2026-10-12
- Up to eight parts on a card instead of six: two full rows of four values. Each tile of the card costs the screen a
  little more memory for it.
- Lines of text and bars across the card get smaller with the values where the tile is short, instead of running out
  at the bottom; where even the smallest size is too large, the card shows the lines that fit whole.

## 0.4.2 - 2026-10-12
- A picture on the Plugins page.

## 0.4.1 - 2026-10-11
- A coloured icon is darker in the light look where it would not read on the card: a yellow one turns ochre on white.

## 0.4.0 - 2026-10-10
- A tap on the tile opens its parts large on a card over the page, with their labels.
- Needs a Tessera from 2026-10-10 or later: an older one shows the card empty under its top bar.

## 0.3.3 - 2026-10-09
- Listed under the topic Home on the Plugins page.
- Needs a Tessera with plugin API 0.7: update the app first.

## 0.3.2 - 2026-10-09
- A label without an icon or a colour always stays, so every number says what it is.
- An icon or a dot in front of a number keeps a little more room from it.

## 0.3.1 - 2026-10-09
- The tile's inspector offers only the sensors that describe a card (with an `elements` attribute).

## 0.3.0 - 2026-10-09
- First release: values, bars and lines of words that a template sensor describes, side by side on a wide tile and
  stacked on any other, in the screen's fonts and colours.
