#!/usr/bin/env python3
"""Write components/custom_card/icons.h: the icon names of Tessera's set (the editor's icon picker) and their
codepoints, sorted by name for a binary search. The screen's icon font holds this set only, so an element's icon is
one of these names. Run it again when Tessera's set grows:

    python3 tools/make_icons.py ../homeassistant_espscreen
"""
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent.parent


def main():
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    sys.path.insert(0, str(Path(sys.argv[1]).resolve() / 'screen_manager' / 'app'))
    import tile_icons
    rows = sorted((name, int(codepoint, 16)) for name, (codepoint, _) in tile_icons.ICONS.items())
    lines = [
        '#pragma once',
        '// Written by tools/make_icons.py from Tessera\'s icon set (screen_manager/app/tile_icons.py): do not edit.',
        '#include <cstdint>',
        '',
        'namespace esphome::custom_card {',
        '',
        'struct IconName { const char *name; uint32_t codepoint; };',
        '',
        '// Sorted by name.',
        'inline constexpr IconName ICON_NAMES[] = {',
        *(f'    {{"{name}", 0x{codepoint:X}}},' for name, codepoint in rows),
        '};',
        '',
        '}  // namespace esphome::custom_card',
        '',
    ]
    (HERE / 'components' / 'custom_card' / 'icons.h').write_text('\n'.join(lines))
    print(f'icons.h: {len(rows)} icons')


if __name__ == '__main__':
    main()
