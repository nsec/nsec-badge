"""Shared functions for images registry manipulation."""

from collections import OrderedDict


def load_images_registry(images_registry_path):
    """Parse the images registry into a dictionary."""
    images_registry = OrderedDict()
    map_offset = 0

    for i, line in enumerate(open(images_registry_path)):
        try:
            format_, width, height, palette, name = line.strip().split('\t')
        except:
            raise ValueError(
                'Images registry file is not well formed on line #{i+1}.')

        images_registry[name] = {
            'format': format_,
            'height': int(height),
            'index': i + 1,
            'map_offset': map_offset,
            'name': name,
            'palette': int(palette),
            'width': int(width),
        }

        if format_ == 'MAP':
            map_offset += int(width) * int(height)

    return images_registry
