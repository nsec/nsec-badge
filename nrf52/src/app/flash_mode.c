/*
 * Copyright 2019 Simon Marchi <simon.marchi@polymtl.ca>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "flash_mode.h"

// Standard includes.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Includes from Nordic.
#include <crc32.h>

// Includes from our code.
#include "gfx_effect.h"
#include "images/external/the_flash_bitmap.h"
#include <drivers/flash.h>
#include <drivers/uart.h>

static const char *skip_spaces(const char *p) {
    while (*p == ' ') {
        p++;
    }

    return p;
}

static bool hex_nibble_decode(char nibble_hex, uint8_t *nibble) {
    if (nibble_hex >= 'a' && nibble_hex <= 'f') {
        *nibble = nibble_hex - 'a' + 0xa;
        return true;
    }

    if (nibble_hex >= 'A' && nibble_hex <= 'F') {
        *nibble = nibble_hex - 'A' + 0xa;
        return true;
    }

    if (nibble_hex >= '0' && nibble_hex <= '9') {
        *nibble = nibble_hex - '0';
        return true;
    }

    return false;
}

static bool hex_decode(const char *hex, uint8_t *bin, size_t n_bytes) {
    for (size_t i = 0; i < n_bytes; i++) {
        char high_c = hex[i * 2];
        char low_c = hex[i * 2 + 1];

        uint8_t high, low;
        if (!hex_nibble_decode(high_c, &high)) {
            return false;
        }

        if (!hex_nibble_decode(low_c, &low)) {
            return false;
        }

        bin[i] = high << 4 | low;
    }

    return true;
}

// Write a 128-bytes block at the given address.  The data is read back from
// the flash and validated.  The CRC32 of the 128 bytes is sent back.  The
// address must be a multiple of 128.
//
// Synopsis: write <address in hex> <128 hex-encoded bytes>
// Response: write ok <crc32 in hex>
//
// Example:
//   -->  write 0x200 aabbcc...ff
//   <--  write ok 0x12345678
static void handle_write(const char *args) {
    const char *start = args;
    char *end;

    long address = strtoul(start, &end, 16);

    if (address % 128 != 0) {
        uart_printf(
            "write error The address is not a multiple of 128 (got %d).\n",
            address);
        return;
    }

    start = skip_spaces(end);

    // The remainder should contain exactly 256 chars (128 data bytes)
    int hex_encoded_len = strlen(start);
    if (hex_encoded_len != 256) {
        uart_printf(
            "write error Unexpected length of hex-encoded byte string. Got %d, "
            "expected 256.\n",
            hex_encoded_len);
        return;
    }

    uint8_t binary[128];
    if (!hex_decode(start, binary, 128)) {
        uart_printf("write error Couldn't decode hex-encoded binary.\n");
        return;
    }

    ret_code_t ret = flash_write_128(address, binary);
    if (ret != NRF_SUCCESS) {
        uart_printf("write error flash_write_128 failed with code %d.\n", ret);
        return;
    }

    uint8_t validation[128];
    ret = flash_read_128(address, validation);
    if (ret != NRF_SUCCESS) {
        uart_printf("write error flash_read_128 failed with code %d.\n", ret);
        return;
    }

    if (memcmp(binary, validation, 128) != 0) {
        uart_printf(
            "write error The data read from the flash does not match what we "
            "wrote.\n");
        return;
    }

    uint32_t crc = crc32_compute(validation, 128, NULL);

    uart_printf("write ok 0x%lx\n", crc);
}

// Reads 128 bytes of data.  The address does not have to be aligned.
//
// Synopsis: read <address in hex>
// Response: read ok <crc32 in hex>
//
// Example:
//   -->  read 0x200
//   <--  read ok aabbcc...dd
static void handle_read(const char *args) {
    long address = strtoul(args, NULL, 16);

    uint8_t data[128];
    ret_code_t ret = flash_read_128(address, data);
    if (ret != NRF_SUCCESS) {
        uart_printf("read error flash_read_128 failed with code %d.\n", ret);
        return;
    }

    uart_puts("read ok ");

    for (int i = 0; i < 128; i++) {
        uart_printf("%02x", data[i]);
    }

    uart_puts("\n");
}

// Erase the whole flash chip.
//
// Synopsis: erase
// Response: erase ok
static void handle_erase(void) {
    ret_code_t ret = flash_erase_chip();
    if (ret != NRF_SUCCESS) {
        uart_printf("erase error flash_erase_chip failed with code %d\n", ret);
        return;
    }

    uart_printf("erase ok\n");
}

// Compute the CRC32 of a region of flash.  The length of the region must be a
// multiple of 128 bytes.
//
// Synopsis: checksum <address in hex> <length in hex>
// Response: checksum ok <crc32 in hex>
//
// Example:
//   -->  checksum 0x1000 0x100
//   <--  checksum ok 0x12345678
static void handle_checksum(const char *args) {
    const char *start = args;
    char *end;
    long startAddress = strtoul(start, &end, 16);
    start = end;
    long length = strtoul(start, &end, 16);

    if (length % 128 != 0) {
        uart_printf(
            "checksum error The length is not a multiple of 128 (got %d).\n",
            length);
        return;
    }

    uint32_t crc;
    uint32_t *p_crc = NULL;
    for (int address = startAddress; address < (startAddress + length);
         address += 128) {
        uint8_t data[128];
        ret_code_t ret = flash_read_128(address, data);
        if (ret != NRF_SUCCESS) {
            uart_printf("read error flash_read_128 failed with code %d.\n",
                        ret);
            return;
        }

        crc = crc32_compute(data, 128, p_crc);

        p_crc = &crc;
    }

    uart_printf("checksum ok 0x%x\n", crc);
}

static void handle_command(const char *command) {
    if (strncmp(command, "write ", strlen("write ")) == 0) {
        handle_write(command + strlen("write "));
    } else if (strncmp(command, "read ", strlen("read ")) == 0) {
        handle_read(command + strlen("read "));
    } else if (strcmp(command, "erase") == 0) {
        handle_erase();
    } else if (strncmp(command, "checksum ", strlen("checksum ")) == 0) {
        handle_checksum(command + strlen("checksum "));
    } else {
        uart_puts("error: unknown command: ");
        uart_puts(command);
        uart_puts("\n");
    }
}

// Enter flash mode.  In this mode, we are blocked waiting on the UART.
// A client on the development machine can send commands to erase, write or
// read the flash.
//
// The only way to get out of this mode is to reset the chip.
void flash_mode() {
    uart_init();

    uart_puts("Entering flash mode!\n");
    gfx_draw_bitmap_ext_flash(0, 0, &the_flash_bitmap);
    gfx_update();

    // The space required for the flash write command is 256 bytes (hex encoded
    // data) plus the text of the command before that.  300 bytes should be more
    // than enough.
    char command[300];
    int size = 0;
    while (1) {
        uint8_t c;
        ret_code_t ret = uart_read(&c);
        if (ret != NRF_SUCCESS) {
            uart_printf("error 0x%x\n", ret);
            uart_clear_errors();
            size = 0;
            continue;
        }

        if (c == '\n') {
            // End of line/command.
            command[size] = '\0';
            handle_command(command);
            size = 0;
        } else if (c == '\0') {
            // Magic byte to clear buffer.
            size = 0;
        } else if (size < sizeof(command)) {
            command[size] = c;
            size++;
        } else {
            // Drop character, buffer is full.
        }
    }
}
