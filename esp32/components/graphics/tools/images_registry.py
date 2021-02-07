"""Shared functions for images registry manipulation."""


def load_images_registry(images_registry_path):
    """Parse the images registry into a dictionary."""
    images_registry = {}

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
            'name': name,
            'palette': int(palette),
            'width': int(width),
        }

    return images_registry
