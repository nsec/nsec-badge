#!/usr/bin/env python
"""Convert the images registry text file into C source code."""

import sys

from images_registry import load_images_registry


def main(images_registry_path, header_path, c_path):
    """Generate C source files based on the image registry definitions."""
    images_registry = load_images_registry(images_registry_path)
    size = len(images_registry)

    # Add one more dummy entry to represent an "empty" image, real images start
    # with index 1.
    size += 1

    # All MAP images are put after FAST images, so the smallest offset of a MAP
    # image will tell how much space is required for FAST images.
    try:
        fast_size = min(image['map_offset'] for image in images_registry.values()
                        if image['format'] == 'MAP')
    except ValueError:
        fast_size = 1

    header_out = open(header_path, 'w')
    header_out.write('#pragma once\n')
    header_out.write('#include <stdint.h>\n\n')
    header_out.write(f'#define IMAGE_REGISTRY_FAST_SIZE {fast_size}\n')
    header_out.write('''
#define IMAGE_REGISTRY_FAST 0
#define IMAGE_REGISTRY_JPEG 1
#define IMAGE_REGISTRY_MAP 2

typedef struct {
    uint8_t type;
    uint8_t width;
    uint8_t height;
    uint8_t palette;
    uint8_t sinkline;
    uint32_t map_offset;
    char filename[24];
} ImagesRegistry_t;\n
''')
    header_out.write(f'extern const ImagesRegistry_t graphics_static_images_registry[{size}];\n\n')

    for i, image in enumerate(images_registry.values()):
        name = make_name(image).upper().replace('-', '_')
        header_out.write(f'#define LIBRARY_IMAGE_{name} {i + 1}\n')

    header_out.close()

    c_out = open(c_path, 'w')
    c_out.write('#include "images_registry.h"\n\n')
    c_out.write('const ImagesRegistry_t graphics_static_images_registry[] = {\n')
    c_out.write('   {},\n')

    for image in images_registry.values():
        if image['format'] == 'FAST':
            format_ = 'IMAGE_REGISTRY_FAST'
        elif image['format'] == 'JPEG':
            format_ = 'IMAGE_REGISTRY_JPEG'
        elif image['format'] == 'MAP':
            format_ = 'IMAGE_REGISTRY_MAP'
        else:
            raise Exception(
                f'Invalid image type "{image["format"]}".')

        name = make_name(image)

        c_out.write('   {\n')
        c_out.write(f'      .type={format_},\n')
        c_out.write(f'      .width={image["width"]},\n')
        c_out.write(f'      .height={image["height"]},\n')
        c_out.write(f'      .palette={image["palette"]},\n')
        c_out.write(f'      .sinkline={image["sinkline"]},\n')
        c_out.write(f'      .map_offset={image["map_offset"]},\n')
        c_out.write(f'      .filename="{name}",\n')
        c_out.write('   },\n')

    c_out.write('};\n')
    c_out.close()


def make_name(image):
    """Format output filename of the image."""
    return image['name'][:-4]


if __name__ == '__main__':
    if len(sys.argv) != 4:
        print(f'Usage: {sys.argv[0]} <images registry> <C header> <C file>')
        sys.exit(1)

    main(sys.argv[1], sys.argv[2], sys.argv[3])
