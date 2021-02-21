#!/usr/bin/env python
"""Convert all images listed in the registry into target format."""

import io
import json
import os.path
import sys

from PIL import Image

from images_registry import load_images_registry


def convert_to_jpeg(specs, source_path, destination):
    """Convert into JPEG format, with a black background."""
    image = Image.open(source_path)
    width = image.size[0]
    height = image.size[1]

    out = Image.new('RGB', (width, height), (0, 0, 0))
    out.paste(image, (0, 0))

    image_out = io.BytesIO()
    out.save(image_out, 'JPEG', quality=92)

    image_out.seek(0, 0)
    image_bytes = image_out.read()
    destination.write(image_bytes)

    return len(image_bytes)


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


def main(images_registry_path, palette_path, source_path, destination_path, jpeg_registry_path):
    """Read the list of images in the registry and convert each one."""
    # Sort the images by the offset value to make sure that they are written
    # into the maps file in the correct order.
    images_registry = load_images_registry(images_registry_path)
    images = list(images_registry.items())
    images.sort(key=lambda x: x[1]['map_offset'])

    jpeg_offset = 0
    jpeg_registry = {}

    jpeg_destination = open(f'{destination_path}/jpeg', 'wb')
    maps_destination = open(f'{destination_path}/maps', 'wb')

    for filename, image in images:
        image_destination_path = '{}/{}'.format(destination_path, filename)
        image_source_path = '{}/{}'.format(source_path, filename)

        if not os.path.exists(image_source_path):
            raise Exception(
                f'Source image {image_source_path} does not exist.')

        format_ = image['format']
        if format_ not in ('FAST', 'JPEG', 'MAP'):
            raise Exception(
                f'Images registry is incorrect: unknown destination format {format_}.')

        if format_ == 'JPEG':
            jpeg_length = convert_to_jpeg(
                image, image_source_path, jpeg_destination)

            jpeg_registry[image['name']] = {
                'length': jpeg_length,
                'offset': jpeg_offset,
            }
            jpeg_offset += jpeg_length

        if format_ in ('FAST', 'MAP'):
            convert_to_pixel_map(
                image, palette_path, image_source_path, maps_destination)

    maps_destination.close()

    # Verify the contents of the JPEG registry file and actually write to disk
    # only if the new result is different. To allows to avoid unnecessary
    # triggering other build targets because of the changed file modification
    # time.
    try:
        with open(jpeg_registry_path, 'r') as f:
            jpeg_registry_json_old = f.read()
    except FileNotFoundError:
        jpeg_registry_json_old = ''

    jpeg_registry_json_new = json.dumps(jpeg_registry)
    if jpeg_registry_json_new != jpeg_registry_json_old:
        with open(jpeg_registry_path, 'w') as f:
            f.write(jpeg_registry_json_new)


if __name__ == '__main__':
    if len(sys.argv) != 6:
        print(f'Usage: {sys.argv[0]} <images registry> <palette> <source> <destination> <jpeg registry>')
        sys.exit(1)

    main(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])
