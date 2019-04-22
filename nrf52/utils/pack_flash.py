#!/usr/bin/env python3

#
# Copyright 2019 Simon Marchi <simon.marchi@polymtl.ca>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

# Concatenate the files passed as arguments in a single file, and produce a
# C 'table of contents'.

import argparse
import re
import os


def concat(out, files):
    """Concaternate all files in out.

    Return a list where each element is a tuple:

      (filename, offset, size)
    """
    file_list = []

    with open(out, 'wb') as o:
        for f in files:
            with open(f, 'rb') as i:
                # Seek to end
                i.seek(0, 2)
                size = i.tell()
                # Seek to beginning
                i.seek(0, 0)

                file_list.append((f, o.tell(), size))
                o.write(i.read())

    return file_list


def clean_filename(filename, keep_dir):
    """Clean filenames for use in generated source code.

    Replace all non-alphanumeric characters by underscores.  If keep_dir is
    False, remove the directory portion of the filename."""

    if not keep_dir:
        filename = os.path.basename(filename)

    return re.sub('[^a-zA-Z0-9]', '_', filename)


def generate_h(h_file, metadata, flava):
    # Little exception for soldering...
    if flava == 'soldering':
        flava = 'ctf'

    with open(h_file, 'w') as h:
        h.write('/* This file is generated.  */\n')
        h.write('\n')
        h.write('#ifndef {}\n'.format(clean_filename(h_file, True)))
        h.write('#define {}\n'.format(clean_filename(h_file, True)))
        h.write('\n')
        h.write('#ifndef NSEC_FLAVOR_{}\n'.format(flava.upper()))
        h.write('#error "Flavor mismatch: this external flash header is generated for {}"\n'.format(flava))
        h.write('#endif\n')
        h.write('\n')
        h.write('\n')
        h.write('struct external_flash_data {\n')
        h.write('    unsigned int offset, size;\n')
        h.write('};\n')
        h.write('\n')
        for f in metadata:
            fname = clean_filename(f[0], False)
            offset = f[1]
            size = f[2]
            h.write('__attribute__((weak))\n')
            h.write('const struct external_flash_data external_flash_{} = {{\n'.format(fname))
            h.write('    .offset = {},\n'.format(offset))
            h.write('    .size = {},\n'.format(size))
            h.write('};\n')
            h.write('\n')

        h.write('#endif\n')


def main():
    argparser = argparse.ArgumentParser()
    argparser.add_argument('output-base')
    argparser.add_argument('flavor', choices=['ctf', 'speaker', 'admin', 'conf', 'trainer', 'bar_beacon', 'soldering'])
    argparser.add_argument('input-files', nargs='*')

    args = vars(argparser.parse_args())

    flava = args['flavor']
    output_base = args['output-base']
    output_base += '_' + flava
    h_file = output_base + '.h'
    bin_file = output_base + '.flashbin'

    input_files = args['input-files']

    metadata = concat(bin_file, input_files)
    generate_h(h_file, metadata, flava)


if __name__ == '__main__':
    main()
