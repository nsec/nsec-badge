//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include "circbuf.h"

void circbuf_init(struct circbuf *circbuf, uint8_t *buf, uint32_t buf_size) {
    circbuf->buf         = buf;
    circbuf->buf_size      = buf_size - 1;
    circbuf->read_pos      = 0;
    circbuf->write_pos     = 0;
}


void circbuf_put(struct circbuf *circbuf, uint8_t byte) {

    if (circbuf->write_pos == ((circbuf->read_pos - 1 + circbuf->buf_size) % circbuf->buf_size)) {
        // Full
        return;
    }

    circbuf->buf[circbuf->write_pos] = byte;
    circbuf->write_pos = (circbuf->write_pos + 1) % circbuf->buf_size;
}


uint8_t circbuf_get(struct circbuf *circbuf, uint8_t *byte) {
    if (circbuf->read_pos == circbuf->write_pos) {
        // Empty
        return 0;
    }

    *byte = circbuf->buf[circbuf->read_pos];
    circbuf->read_pos = (circbuf->read_pos + 1) % circbuf->buf_size;

    return 1;
}

void circbuf_head(struct circbuf *circbuf, uint8_t *byte) {
    if (circbuf->write_pos == 0) {
        *byte = circbuf->buf[circbuf->buf_size-1];
    }
    else {
        *byte = circbuf->buf[circbuf->write_pos-1];
    }
}

