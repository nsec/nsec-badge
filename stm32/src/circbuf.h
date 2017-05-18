/*
 * Copyright (c) 2015, Benjamin Vanheuverzwijn <bvanheu@gmail.com>
 * License: MIT (see LICENSE for details)
 */

#ifndef CIRCBUF_H
#define CIRCBUF_H

#include <stdint.h>

struct circbuf {
    uint8_t *buf;
    uint32_t buf_size;
    volatile uint8_t read_pos;
    volatile uint8_t write_pos;
};

void circbuf_init(struct circbuf *circbuf, uint8_t *buf, uint32_t buf_size);

void circbuf_put(struct circbuf *circbuf, uint8_t byte);

void circbuf_head(struct circbuf *circbuf, uint8_t *byte);

uint8_t circbuf_get(struct circbuf *circbuf, uint8_t *byte);

#endif

