from PIL import Image
import argparse
import pycparser
import sys
import os

C_TEMPLATE="""/*
    This file was automatically genereted by """ + __file__ + """
*/
const unsigned char {var_name:s}[] = {{
    {byte_array:s}
}};
const unsigned int {var_name:s}_width = {width:d};

const unsigned int {var_name:s}_height = {height:d};
"""

def encode_image(input_file_path, output_file_path, rotate=False):
    image = Image.open(input_file_path)
    if image.mode != '1':
        raise Exception("Image must be in 1-bit color mode.")
    if rotate:
        image = image.transpose(Image.ROTATE_270)
    array = ",".join([hex(ord(b)) for b in image.tobytes()])
    
    with file(output_file_path, "w") as f:
        f.write(C_TEMPLATE.format(
            byte_array=array,
            var_name=os.path.splitext(os.path.basename(output_file_path))[0],
            width=image.width,
            height=image.height
        ))

def decode_image(input_file_path, output_file_path, width, rotate=False):
    tree = pycparser.parse_file(input_file_path, use_cpp=True, cpp_path="arm-none-eabi-cpp")
    if len(tree.ext) != 1:
        raise Exception("C file has more than one element.")
    font = tree.ext[0]
    if not "char" in font.type.type.type.names:
        raise Exception("Array must be of type char.")
    buf = b""
    for con in font.init.exprs:
        buf += chr(int(con.value, 0))
    
    height = (len(buf) * 8) / width
    
    image = Image.frombytes("1", (width, height), buf)
    if rotate:
        image = image.transpose(Image.ROTATE_90)
    image.save(output_file_path)
    

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Transform images from 1 color bitmap to C array and vice-versa.')
    parser.add_argument('-d', '--decode', action='store_true',
                        help='Decode C file to image')
    parser.add_argument('-w', '--width', type=int,
                        help='Width of the image to decode')
    parser.add_argument('-r', '--rotate', action='store_true',
                        help='Rotate image by 90 degrees')
    parser.add_argument('-i', '--infile',
                        help='Input file')
    parser.add_argument('-o', '--outfile',
                        help='Output file')

    args = parser.parse_args()
    if args.decode:
        decode_image(args.infile, args.outfile, args.width, args.rotate)
    else:
        encode_image(args.infile, args.outfile, args.rotate)
