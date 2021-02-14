"""Shared functions for images registry manipulation."""

from collections import OrderedDict


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
