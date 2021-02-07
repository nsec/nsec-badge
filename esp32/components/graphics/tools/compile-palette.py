#!/usr/bin/env python
"""Convert indexed image palette into C source files.

RGB colors are converted and packed into format directly accepted by the
graphics component API.
"""

import sys


def format_palette_declaration(palette_id, palette):
    """Generate header file declarations for a palette."""
    size = len(palette)

    yield f'''
extern const int graphics_static_palette_{palette_id}_size;
'''
    yield f'''
extern const uint16_t graphics_static_palette_{palette_id}[{size}];
'''


def format_palette_definition(palette_id, palette):
    """Generate static array definitions for a palette."""
    size = len(palette)

    colors = ['0']
    for r, g, b in palette[1:]:
        rgb565 = (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))
        reverse = (rgb565 >> 8) + ((rgb565 & 0xFF) << 8)

        colors.append('0x{:04x}'.format(reverse))

    colors_string = ', '.join(colors)

    yield f'''
const int graphics_static_palette_{palette_id}_size = {size};
'''
    yield f'''
const uint16_t graphics_static_palette_{palette_id}[{size}] = {{
{colors_string}
}};
'''


def main(palette_path, header_path, c_path):
    """Convert color list made with generate-palette.py into C source files.

    All palettes are converted into global const array. Both the header file
    and the source file are generated.
    """
    header_out = open(header_path, 'w')
    c_out = open(c_path, 'w')

    header_out.write('#pragma once\n')
    header_out.write('#include <stdint.h>\n')
    c_out.write('#include "palette.h"\n')

    for i, line in enumerate(open(palette_path)):
        palette = [tuple(bytes.fromhex(i)) for i in
                   line.strip().split(',')]

        for out_line in format_palette_declaration(i, palette):
            header_out.write(out_line)

        for out_line in format_palette_definition(i, palette):
            c_out.write(out_line)

    header_out.close()
    c_out.close()


if __name__ == '__main__':
    if len(sys.argv) != 4:
        print(f'Usage: {sys.argv[0]} <palette> <C header> <C file>')
        sys.exit(1)

    main(sys.argv[1], sys.argv[2], sys.argv[3])
