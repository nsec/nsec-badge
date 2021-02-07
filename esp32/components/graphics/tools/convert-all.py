#!/usr/bin/env python
"""Convert all images listed in the registry into target format."""

import os.path
import sys

from PIL import Image

from images_registry import load_images_registry


def convert_to_jpeg(specs, source_path, destination_path):
    """Convert into JPEG format, with a black background."""
    destination_path = destination_path[:-4] + '.jpeg'

    image = Image.open(source_path)
    width = image.size[0]
    height = image.size[1]

    out = Image.new('RGB', (width, height), (0, 0, 0))
    out.paste(image, (0, 0))
    out.save(destination_path, quality=92)


def convert_to_pixel_map(specs, palette_path, source_path, destination):
    """Convert into the list of indexed palette colors.

    This format simply lists the color palette indices of each pixel in
    sequence, without any file headers or metadata. All mapped images are
    concatenated into a single file as an optimization.
    """
    palettes = open(palette_path, 'r').readlines()
    if specs['palette'] >= len(palettes):
        raise Exception(
            'Image references a palette that does not exist. You may need to '
            'regenerate the palettes file.')

    palette = [tuple(bytes.fromhex(i)) for i in
               palettes[specs['palette']].strip().split(',')]

    image = Image.open(source_path)
    out = []
    for y in range(image.height):
        for x in range(image.width):
            pixel = image.getpixel((x, y))

            if len(pixel) == 4 and pixel[3] == 0:
                out.append(0)
                continue

            pixel = pixel[:3]
            for i, color in enumerate(palette):
                if pixel == color:
                    out.append(i)
                    break
            else:
                raise Exception(
                    f'Color {pixel} is not present in palette {specs["palette"]}.')

    destination.write(bytes(out))


def main(images_registry_path, palette_path, source_path, destination_path):
    """Read the list of images in the registry and convert each one."""
    images_registry = load_images_registry(images_registry_path)
    maps_destination = open(f'{destination_path}/maps', 'wb')

    for filename, image in images_registry.items():
        image_destination_path = '{}/{}'.format(destination_path, filename)
        image_source_path = '{}/{}'.format(source_path, filename)

        if not os.path.exists(image_source_path):
            raise Exception(
                f'Source image {image_source_path} does not exist.')

        format_ = image['format']
        if format_ == 'JPEG':
            convert_to_jpeg(
                image, image_source_path, image_destination_path)
        elif format_ == 'MAP':
            convert_to_pixel_map(
                image, palette_path, image_source_path, maps_destination)
        else:
            raise Exception(
                f'Images registry is incorrect: unknown destination format {format_}.')

    maps_destination.close()


if __name__ == '__main__':
    if len(sys.argv) != 5:
        print(f'Usage: {sys.argv[0]} <images registry> <palette> <source> <destination>')
        sys.exit(1)

    main(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
