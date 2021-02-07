#!/usr/bin/env python
"""Generate the color palettes file.

The palettes file serves as a base for all other image convert operations. Each
palette is limited to 255 colors (plus the transparent color) but it is
possible to generate any number of palettes.

The palette ID of each image is specified in the 4th column of the registry.
Colors from all images with the same palette ID will be merged together. The
0th entry in the palette is set to 0, it is reserved for 1 bit of transparency.

Images that are converted to JPEG are skipped.
"""

import sys

from PIL import Image

from images_registry import load_images_registry


def generate_palette(library_path, palette_id, images):
    """Extract colors from all images into a list."""
    colors = set()
    for specs in images:
        image = Image.open('{}/{}'.format(library_path, specs['name']))

        for y in range(image.height):
            for x in range(image.width):
                pixel = image.getpixel((x, y))

                if len(pixel) == 4 and pixel[3] == 0:
                    # Do not include transparent pixels into the palette.
                    continue

                color = '{:02x}{:02x}{:02x}'.format(pixel[0], pixel[1], pixel[2])
                colors.add(color)

    colors = sorted(colors)
    if len(colors) > 255:
        raise Exception(
            'There are more than 255 colors in palette {palette_id}')

    # First entry is a dummy reserved for transparent pixels.
    return ['00'] + colors


def main(library_path, images_registry_path, palette_path):
    """Group images in the registry by the palette ID and generate each."""
    images_registry = load_images_registry(images_registry_path)
    palette_groups = {
        0: [], 1: [], 2: [], 3: [], 4: [],
    }

    for image in images_registry.values():
        if image['format'] == 'JPEG':
            continue

        palette_id = image['palette']
        if palette_id not in palette_groups:
            raise Exception(
                f'Unknown palette "{palette_id}". The number of palettes '
                f'is hardcoded the generate-palette.py script. If you need a '
                f'new palette, modify this script and the graphics.c source.')

        palette_groups[palette_id].append(image)

    palette_groups = sorted(palette_groups.items(), key=lambda x: x[0])

    with open(palette_path, 'w') as f:
        for palette_id, images in palette_groups:
            palette = generate_palette(library_path, palette_id, images)
            f.write(','.join(palette))
            f.write('\n')


if __name__ == '__main__':
    if len(sys.argv) != 4:
        print(f'Usage: {sys.argv[0]} <library path> <images registry> <palette>')
        sys.exit(1)

    main(sys.argv[1], sys.argv[2], sys.argv[3])
