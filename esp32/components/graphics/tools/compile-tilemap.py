#!/usr/bin/env python
"""Compile scene definition from SVG into binary tilemap format.

The source is a standard SVG file that can be edited with Inkscape, but with
several restrictions:

- The document origin is at the top-left corner of the document, so older
  Inkscape versions may not work.

- The width and the height must be divisible by the tile size - 24px - and no
  unit scaling is allowed.

- All objects must be placed in layers named l0 through l8.

- The only supported kind of element is the <image /> element.  Images must be
  "linked" and not "embedded" into the document.

- It is recommended to keep the SVG and all images in the same directory, so
  that the "xlink:href" attribute contains only the filename, without the path
  component.  Inkscape keeps track of the absolute path in the
  "sodipodi:absref" attribute but its value is ignored.

- All images must be aligned to the grid, i.e. x and y coordinates must be
  multiples of the tile size.  Image width and height may be arbitrary and they
  can span several tiles, but not more than three tiles.  Images larger than
  that must be split into multiple tiles, and it is generally recommended to
  avoid the use of many multi-tile images.  No element scaling is allowed;
  while the compilation will probably work with scaled images, the rendering
  will likely produce an unexpected result.

- One layer cannot contain more than one element at the same (x,y) coordinate.
  Overlapping images must be put into different layer.

The following document may be used as a template:

<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg
   xmlns:dc="http://purl.org/dc/elements/1.1/"
   xmlns:cc="http://creativecommons.org/ns#"
   xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
   xmlns:svg="http://www.w3.org/2000/svg"
   xmlns="http://www.w3.org/2000/svg"
   xmlns:sodipodi="http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
   xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape"
   width="1200"
   height="1200"
   viewBox="0 0 1200 1200"
   version="1.1"
   id="svg1"
   inkscape:version="1.0.1 (3bc2e813f5, 2020-09-07, custom)">
  <defs id="defs2" />
  <sodipodi:namedview
     id="base"
     pagecolor="#ffffff"
     bordercolor="#666666"
     borderopacity="1.0"
     inkscape:pageopacity="0.0"
     inkscape:pageshadow="2"
     inkscape:document-units="px"
     showgrid="true"
     units="px"
     borderlayer="true">
    <inkscape:grid type="xygrid" id="grid833" spacingx="24" spacingy="24" dotted="true" />
  </sodipodi:namedview>
  <metadata id="metadata5">
    <rdf:RDF>
      <cc:Work rdf:about="">
        <dc:format>image/svg+xml</dc:format>
        <dc:type rdf:resource="http://purl.org/dc/dcmitype/StillImage" />
        <dc:title />
        <cc:license rdf:resource="http://creativecommons.org/licenses/by/4.0/" />
      </cc:Work>
      <cc:License rdf:about="http://creativecommons.org/licenses/by/4.0/">
        <cc:permits rdf:resource="http://creativecommons.org/ns#Reproduction" />
        <cc:permits rdf:resource="http://creativecommons.org/ns#Distribution" />
        <cc:requires rdf:resource="http://creativecommons.org/ns#Notice" />
        <cc:requires rdf:resource="http://creativecommons.org/ns#Attribution" />
        <cc:permits rdf:resource="http://creativecommons.org/ns#DerivativeWorks" />
      </cc:License>
    </rdf:RDF>
  </metadata>
  <g inkscape:groupmode="layer" id="layer1" inkscape:label="l0" />
  <g inkscape:groupmode="layer" id="layer2" inkscape:label="l1" />
  <g inkscape:groupmode="layer" id="layer3" inkscape:label="l2" />
  <g inkscape:groupmode="layer" id="layer4" inkscape:label="l3" />
  <g inkscape:groupmode="layer" id="layer5" inkscape:label="l4" />
  <g inkscape:groupmode="layer" id="layer6" inkscape:label="l5" />
  <g inkscape:groupmode="layer" id="layer7" inkscape:label="l6" />
  <g inkscape:groupmode="layer" id="layer8" inkscape:label="l7" />
</svg>
"""

import math
import sys
import xml.etree.ElementTree as ET
from dataclasses import dataclass

from images_registry import load_images_registry

DEFINED_LAYERS = 8
TILE_SIZE = 24


class Empty:
    """Representation of an unoccupied position on the grid."""
    pass


@dataclass
class Image:
    """Parsed reference to an image file."""
    href: str = None
    width: int = 0
    height: int = 0


class Tile:
    """A single tile reference with merged layers."""

    def __init__(self, x: int, y: int):
        self.components = [Empty] * DEFINED_LAYERS
        self.dependency = {
            'backward_x': 0,
            'backward_y': 0,
            'forward_x': 0,
            'forward_y': 0,
        }
        self.flags = {
            'jpeg': False,
        }
        self.x = x
        self.y = y

    def __getitem__(self, key):
        return self.components[key]

    def __setitem__(self, key, value):
        self.components[key] = value


class Tilemap:
    """A grid of tiles.

    This class defines the sequential layout of tiles in the output binary.
    """

    def __init__(self, width_tiles: int, height_tiles: int):
        self.width_tiles = width_tiles
        self.height_tiles = height_tiles

        self.tiles = []
        for y in range(height_tiles):
            for x in range(width_tiles):
                self.tiles.append(Tile(x=x, y=y))

    def __len__(self):
        return len(self.tiles)

    def __getitem__(self, key):
        if isinstance(key, int):
            return self.tiles[key]

        if isinstance(key, tuple) and len(key) == 2:
            if key[0] < 0 or key[1] < 0:
                raise KeyError()

            if key[0] >= self.width_tiles or key[1] >= self.height_tiles:
                raise KeyError()

            return self.tiles[key[1] * self.width_tiles + key[0]]

        raise TypeError(
            'Tilemap supports two indexing modes: traditional list indexing '
            'with an integer, or two integer tuple with x and y coordinates '
            'on the grid.')


def parse_graphic_layer_into_tilemap(layer_id, layer, tilemap):
    """Read all images from a single layer and place the on the grid."""
    if any(i.tag != '{http://www.w3.org/2000/svg}image' for i in layer):
        raise ValueError(
            f'Layer {layer_id} contains some non-image elements. Graphic '
            f'layers may contain only <image/> elements that reference PNG '
            f'files.')

    for image in layer:
        id_, href, width, height, x, y = (
            image.attrib['id'],
            image.attrib['{http://www.w3.org/1999/xlink}href'],
            float(image.attrib['width']),
            float(image.attrib['height']),
            float(image.attrib['x']),
            float(image.attrib['y']),
        )

        if x % TILE_SIZE != 0.0 or y % TILE_SIZE != 0.0:
            raise ValueError(
                f'Image id="{id_}" in layer {layer_id} is not alligned to '
                f'grid: x={x}, y={y}')

        if width == 0.0 or height == 0.0:
            raise ValueError(
                f'Image id="{id_}" in layer {layer_id} at x={x}, y={y} has zero size.')

        if width % 1 != 0.0 or height % 1 != 0.0:
            raise ValueError(
                f'Image id="{id_}" in layer {layer_id} does not have an exact '
                f'size: width={width}, height={height}. It is not recommended, '
                f'but the size of images does not have to be a multiple of tile '
                f'size. However, images cannot be scaled to a fraction of a pixel.')

        width, height, x, y = int(width), int(height), int(x), int(y)
        tile_x, tile_y = x // TILE_SIZE, y // TILE_SIZE

        if width > TILE_SIZE * 3 or height > TILE_SIZE * 3:
            raise ValueError(
                f'Image id="{id_}" in layer {layer_id} is larger than three '
                f'tiles. The maximum allowed size is {TILE_SIZE * 3}px in '
                f'either dimention. Larger images must be split into several '
                f'smaller ones.')

        try:
            if tilemap[tile_x, tile_y][layer_id] is not Empty:
                raise ValueError(
                    f'Layer {layer_id} has more than one tile at x={x}, y={y}')
        except KeyError:
            raise ValueError(
                f'Image id="{id_}" in layer {layer_id} is outside of the '
                f'grid at x={x}, y={y}')

        tilemap[tile_x, tile_y][layer_id] = Image(href, width, height)

        overlap_x, overlap_y = math.ceil(width / TILE_SIZE), math.ceil(height / TILE_SIZE)

        tilemap[tile_x, tile_y].dependency['forward_x'] = max(
            overlap_x - 1, tilemap[tile_x, tile_y].dependency['forward_x'])

        tilemap[tile_x, tile_y].dependency['forward_y'] = max(
            overlap_y - 1, tilemap[tile_x, tile_y].dependency['forward_y'])

        for i in range(overlap_x):
            for j in range(overlap_y):
                tilemap[tile_x + i, tile_y + j].dependency['backward_x'] = max(
                    i,
                    tilemap[tile_x + i, tile_y + j].dependency['backward_x'])

                tilemap[tile_x + i, tile_y + j].dependency['backward_y'] = max(
                    j,
                    tilemap[tile_x + i, tile_y + j].dependency['backward_y'])


def parse_tilemap(source_path):
    """Prase the SVG into a tilemap grid."""
    source = ET.parse(source_path)
    source_root = source.getroot()

    width = int(source_root.attrib['width'])
    height = int(source_root.attrib['height'])
    if width % TILE_SIZE != 0 or height % TILE_SIZE != 0:
        raise ValueError(
            f'Canvas width and height must be divisible by {TILE_SIZE} to '
            f'create a whole number of tiles.')

    if source_root.attrib['viewBox'] != f'0 0 {width} {height}':
        raise ValueError(
            f'Document viewBox attribute must be set to "0 0 {width} '
            f'{height}" to avoid coordinates scaling.')

    width_tiles = width // TILE_SIZE
    height_tiles = height // TILE_SIZE
    tilemap = Tilemap(width_tiles, height_tiles)

    for layer_id in range(0, DEFINED_LAYERS):
        for layer in source_root:
            if layer.attrib.get('{http://www.inkscape.org/namespaces/inkscape}label') == f'l{layer_id}':
                break
        else:
            raise ValueError(
                f'Layer group "l{layer_id}" is not found in the document.')

        parse_graphic_layer_into_tilemap(layer_id, layer, tilemap)

    return tilemap


def main(images_registry_path, source_path, destination_path):
    images_registry = load_images_registry(images_registry_path)
    tilemap = parse_tilemap(source_path)

    with open(destination_path, 'wb') as f:
        # Add two dummy rows at the beginning.
        for y in range(2):
            f.write(bytes([0] * ((DEFINED_LAYERS + 2) * (tilemap.width_tiles + 2))))

        for y in range(tilemap.height_tiles):
            # Add two dummy columns at the beginning of each row.
            f.write(bytes([0] * (DEFINED_LAYERS + 2)))
            f.write(bytes([0] * (DEFINED_LAYERS + 2)))

            for x in range(tilemap.width_tiles):
                tile = tilemap[x, y]

                for layer_id in range(DEFINED_LAYERS):
                    image = tile[layer_id]

                    if image is Empty:
                        f.write(bytes([0]))

                    elif image.href in images_registry:
                        if images_registry[image.href]['format'] == 'JPEG':
                            tile.flags['jpeg'] = True

                        f.write(bytes([images_registry[image.href]['index']]))

                    else:
                        raise Exception(
                            f'Image {image.href} is not in the registry.')

                # Sanity check.
                for i in tile.dependency.values():
                    assert(0 <= i < 3)

                dependency = 0
                dependency |= tile.dependency['backward_x'] << 6
                dependency |= tile.dependency['backward_y'] << 4
                dependency |= tile.dependency['forward_x'] << 2
                dependency |= tile.dependency['forward_y']

                flags = 0
                flags |= 2 if tile.flags['jpeg'] else 0
                flags |= 4 if dependency else 0

                f.write(bytes([flags, dependency]))


if __name__ == '__main__':
    if len(sys.argv) != 4:
        print(f'Usage: {sys.argv[0]} <images registry> <source> <destination>')
        sys.exit(1)

    main(sys.argv[1], sys.argv[2], sys.argv[3])
