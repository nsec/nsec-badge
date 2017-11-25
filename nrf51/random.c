//  Copyright (c) 2017
//  Benjamin Vanheuverzwijn <bvanheu@gmail.com>
//  Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
//  License: MIT (see LICENSE for details)

#include <stdint.h>

#include <nrf51.h>
#include <nrf_soc.h>

#include "random.h"

static uint32_t z1, z2, z3, z4;
static uint8_t _init_done = 0;
static struct {
    size_t bytes_left;
    union {
        uint32_t uint32;
        uint8_t bytes[4];
    } data;
} _rand_buff = {
    .bytes_left = 0
};

static void _get_sd_rnd_buf(void * buf, uint8_t len)
{
    uint8_t bytes_available;
    do
    {
        sd_rand_application_bytes_available_get(&bytes_available);
        if(bytes_available >= len)
        {
            sd_rand_application_vector_get(buf, len);
            len = 0;
        }
        else
        {
            sd_rand_application_vector_get(buf, bytes_available);
            buf += bytes_available;
            len -= bytes_available;
            sd_app_evt_wait();
        }
    } while(len > 0);
}

/*
 * The next function only
 *
 * 32-bits Random number generator U[0,1): lfsr113
 * Author: Pierre L'Ecuyer,
 * Source: http://www.iro.umontreal.ca/~lecuyer/myftp/papers/tausme2.ps
 * ---------------------------------------------------------
 * VERY IMPORTANT **** :
 *  The initial seeds z1, z2, z3, z4  MUST be larger than
 *  1, 7, 15, and 127 respectively.
 */
static uint32_t lfsr113_bits(void) {
   uint32_t b;
   b  = ((z1 << 6) ^ z1) >> 13;
   z1 = ((z1 & 4294967294U) << 18) ^ b;
   b  = ((z2 << 2) ^ z2) >> 27;
   z2 = ((z2 & 4294967288U) << 2) ^ b;
   b  = ((z3 << 13) ^ z3) >> 21;
   z3 = ((z3 & 4294967280U) << 7) ^ b;
   b  = ((z4 << 3) ^ z4) >> 12;
   z4 = ((z4 & 4294967168U) << 13) ^ b;

   return (z1 ^ z2 ^ z3 ^ z4);
}

static void random_init(void) {
    z1 = z2 = z3 = z4 = 0;

    while (z1 < 1) {
        _get_sd_rnd_buf(&z1, sizeof(z1));
    }
    while (z2 < 7) {
        _get_sd_rnd_buf(&z2, sizeof(z2));
    }
    while (z3 < 15) {
        _get_sd_rnd_buf(&z3, sizeof(z3));
    }
    while (z4 < 127) {
        _get_sd_rnd_buf(&z4, sizeof(z4));
    }
}

void nsec_random_get(uint8_t * buffer, size_t buffer_size) {
    if(!_init_done) {
        random_init();
        _init_done = 1;
    }
    while(buffer_size > 0) {
        if(_rand_buff.bytes_left == 0) {
            _rand_buff.data.uint32 = lfsr113_bits();
            _rand_buff.bytes_left = 4;
        }
        buffer[--buffer_size] = _rand_buff.data.bytes[--_rand_buff.bytes_left];
    }
}
