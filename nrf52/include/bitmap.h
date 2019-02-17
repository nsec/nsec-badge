#ifndef bitmap_h
#define bitmap_h

#include <stdint.h>

enum image_encoding { image_encoding_1bit, image_encoding_565bits };

struct bitmap {
    const uint8_t *image;
    uint32_t width;
    uint32_t height;
    enum image_encoding encoding;
};

#endif // bitmap_h
