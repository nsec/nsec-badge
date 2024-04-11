#!/bin/sh

echo "/* Generated automatically. Do not modify. */"
# gzip --stdout --best $1 | base64 --wrap 0 | xxd -i -C -n badge_png
base64 --wrap 0 $1 | xxd -i -C -n badge_png
