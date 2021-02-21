"""Shared functions for images registry manipulation."""

import json
from collections import OrderedDict


def images_registry_add_jpeg(images_registry, jpeg_registry):
    """Merge JPEG registry entries into the images registry.

    For performance reasons, all generated JPEG files are concatenated into one
    single file. Because JPEG images are generated at build time, their length
    cannot be know in advance, so this information is not part of the images
    registry. When conversion happens, the length and offset of all concatenated
    images are written into jpeg_registry.json file.
    """
    for name, jpeg_data in jpeg_registry.items():
        images_registry[name]['jpeg_length'] = jpeg_data['length']
        images_registry[name]['jpeg_offset'] = jpeg_data['offset']


def load_images_registry(images_registry_path):
    """Parse the images registry into a dictionary."""
    images_registry = OrderedDict()

    for i, line in enumerate(open(images_registry_path)):
        try:
            format_, width, height, palette, sinkline, name = line.strip().split('\t')
        except:
            raise ValueError(
                'Images registry file is not well formed on line #{i+1}.')

        images_registry[name] = {
            'format': format_,
            'height': int(height),
            'index': i + 1,
            'jpeg_length': 0,
            'jpeg_offset': 0,
            'map_offset': 0,
            'name': name,
            'palette': int(palette),
            'sinkline': int(sinkline),
            'width': int(width),
        }

    # Do two passes through the list to assign the offset value: first to the
    # FAST images, then to the rest of the MAP images.
    map_offset = 0

    for image in images_registry.values():
        if image['format'] != 'FAST':
            continue

        image['map_offset'] = map_offset
        map_offset += image['width'] * image['height']

    for image in images_registry.values():
        if image['format'] != 'MAP':
            continue

        image['map_offset'] = map_offset
        map_offset += image['width'] * image['height']

    return images_registry


def load_jpeg_registry(jpeg_registry_path):
    """Load the jpeg registry JSON into a dictionary."""
    with open(jpeg_registry_path) as f:
        return json.load(f)
