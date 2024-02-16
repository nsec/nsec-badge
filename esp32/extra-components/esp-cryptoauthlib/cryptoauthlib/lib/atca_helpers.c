/**
 * \file
 * \brief Helpers to support the CryptoAuthLib Basic API methods
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cryptoauthlib.h"
#include "atca_helpers.h"



/* Ruleset:
    Index   -   Meaning
    0       -   62 Character
    1       -   63 Character
    2       -   Pad Character (none if 0)
    3       -   Maximum line length (no limit if 0) */
uint8_t atcab_b64rules_default[4]   = { '+', '/', '=', 64 };
uint8_t atcab_b64rules_mime[4]      = { '+', '/', '=', 76 };
uint8_t atcab_b64rules_urlsafe[4]   = { '-', '_', 0, 0 };


/** \brief Convert a binary buffer to a hex string for easy reading.
 *  \param[in]    bin        Input data to convert.
 *  \param[in]    bin_size   Size of data to convert.
 *  \param[out]   hex        Buffer that receives hex string.
 *  \param[in,out] hex_size   As input, the size of the hex buffer.
 *                           As output, the size of the output hex.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_bin2hex(const uint8_t* bin, size_t bin_size, char* hex, size_t* hex_size)
{
    return atcab_bin2hex_(bin, bin_size, hex, hex_size, true, true, true);
}

static void uint8_to_hex(uint8_t num, char* hex_str)
{
    uint8_t nibble = (num >> 4) & 0x0F;

    if (nibble < 10)
    {
        *(hex_str++) = '0' + nibble;
    }
    else
    {
        *(hex_str++) = 'A' + (nibble - 10);
    }
    nibble = num & 0x0F;
    if (nibble < 10)
    {
        *(hex_str++) = '0' + nibble;
    }
    else
    {
        *(hex_str++) = 'A' + (nibble - 10);
    }
}

static void hex_to_lowercase(char *buffer, size_t length)
{
    size_t index;

    if ((buffer != NULL) && (length > 0))
    {
        for (index = 0; index < length; index++)
        {
            buffer[index] = (uint8_t)(tolower(buffer[index]));
        }
    }
}


static void hex_to_uppercase(char *buffer, size_t length)
{
    size_t index;

    if ((buffer != NULL) && (length > 0))
    {
        for (index = 0; index < length; index++)
        {
            buffer[index] = (uint8_t)(toupper(buffer[index]));
        }
    }
}

/** \brief To reverse the input data.
 *  \param[in]    bin        Input data to reverse.
 *  \param[in]    bin_size   Size of data to reverse.
 *  \param[out]   dest       Buffer to store reversed binary data.
 *  \param[in]    dest_size  The size of the dest buffer.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_reversal(const uint8_t* bin, size_t bin_size, uint8_t* dest, size_t* dest_size)
{
    size_t last, i;

    // Verify the inputs
    if ((bin == NULL) || (dest == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    if (*dest_size < bin_size)
    {
        return ATCA_SMALL_BUFFER;
    }

    last = bin_size - 1;

    for (i = 0; i < bin_size; i++)
    {
        dest[i] = bin[last];
        last--;
    }
    *dest_size = bin_size;
    return ATCA_SUCCESS;
}


/** \brief Function that converts a binary buffer to a hex string suitable for
 *         easy reading.
 *  \param[in]    bin        Input data to convert.
 *  \param[in]    bin_size   Size of data to convert.
 *  \param[out]   hex        Buffer that receives hex string.
 *  \param[in,out] hex_size   As input, the size of the hex buffer.
 *                           As output, the size of the output hex.
 *  \param[in]    is_pretty  Indicates whether new lines should be
 *                           added for pretty printing.
 *  \param[in]	  is_space   Convert the output hex with space between it.
 *  \param[in]	  is_upper   Convert the output hex to upper case.

 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_bin2hex_(const uint8_t* bin, size_t bin_size, char* hex, size_t* hex_size, bool is_pretty, bool is_space, bool is_upper)
{
    size_t i;
    size_t cur_hex_size = 0;
    size_t max_hex_size;

    // Verify the inputs
    if (bin == NULL || hex == NULL || hex_size == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    max_hex_size = *hex_size;
    *hex_size = 0;

    // Convert one byte at a time
    for (i = 0; i < bin_size; i++)
    {
        if (cur_hex_size > max_hex_size)
        {
            break;
        }
        if (i != 0)
        {
            if (is_pretty && (i % 16 == 0))
            {
                if (cur_hex_size + 2 > max_hex_size)
                {
                    return ATCA_SMALL_BUFFER;
                }
                memcpy(&hex[cur_hex_size], "\r\n", 2);
                cur_hex_size += 2;
            }
            else
            {
                if (is_space)
                {
                    if (cur_hex_size + 1 > max_hex_size)
                    {
                        return ATCA_SMALL_BUFFER;
                    }
                    hex[cur_hex_size] = ' ';
                    cur_hex_size += 1;
                }
            }
        }

        if (cur_hex_size + 2 > max_hex_size)
        {
            return ATCA_SMALL_BUFFER;
        }
        uint8_to_hex(bin[i], &hex[cur_hex_size]);
        cur_hex_size += 2;
    }

    if (is_upper)
    {
        hex_to_uppercase(hex, cur_hex_size);
    }
    else
    {
        hex_to_lowercase(hex, cur_hex_size);
    }

    *hex_size = cur_hex_size;
    if (cur_hex_size < max_hex_size)
    {
        // Since we have room add NULL as a convenience, but don't add to the
        // output size.
        hex[cur_hex_size] = 0;
    }

    return ATCA_SUCCESS;
}

inline static uint8_t hex_digit_to_num(char c)
{
    if (c >= '0' && c <= '9')
    {
        return (uint8_t)(c - '0');
    }
    if (c >= 'a' && c <= 'f')
    {
        return (uint8_t)(c - 'a') + 10;
    }
    if (c >= 'A' && c <= 'F')
    {
        return (uint8_t)(c - 'A') + 10;
    }
    return 16;
}


ATCA_STATUS atcab_hex2bin_(const char* hex, size_t hex_size, uint8_t* bin, size_t* bin_size, bool is_space)
{
    size_t hex_index;
    size_t bin_index = 0;
    bool is_upper_nibble = true;

    for (hex_index = 0; hex_index < hex_size; hex_index++)
    {
        if (!isHexDigit(hex[hex_index]))
        {
            if (((hex_index + 1) % 3 == 0) && is_space)
            {
                if (hex[hex_index] != ' ')
                {
                    return ATCA_BAD_PARAM;
                }
            }

            continue; // Skip any non-hex character
        }
        if (bin_index >= *bin_size)
        {
            return ATCA_SMALL_BUFFER;
        }

        if (is_upper_nibble)
        {
            // Upper nibble
            bin[bin_index] = (uint8_t)(hex_digit_to_num(hex[hex_index]) << 4);
        }
        else
        {
            // Lower nibble
            bin[bin_index] += hex_digit_to_num(hex[hex_index]);
            bin_index++;
        }
        is_upper_nibble = !is_upper_nibble;
    }
    if (!is_upper_nibble)
    {
        // Didn't end with an even number of hex digits. Assume it was malformed.
        return ATCA_BAD_PARAM;
    }
    *bin_size = bin_index;

    return ATCA_SUCCESS;
}

/** \brief Function that converts a hex string to binary buffer
 *  \param[in]    hex       Input buffer to convert
 *  \param[in]    hex_size  Length of buffer to convert
 *  \param[out]   bin       Buffer that receives binary
 *  \param[in,out] bin_size  As input, the size of the bin buffer.
 *                          As output, the size of the bin data.
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_hex2bin(const char* hex, size_t hex_size, uint8_t* bin, size_t* bin_size)
{
    return atcab_hex2bin_(hex, hex_size, bin, bin_size, false);
}

/**
 * \brief Checks to see if a character is an ASCII representation of a digit ((c ge '0') and (c le '9'))
 * \param[in] c  character to check
 * \return True if the character is a digit
 */
bool isDigit(char c)
{
    return (c >= '0') && (c <= '9');
}

/**
 * \brief Checks to see if a character is blank space
 * \param[in] c  character to check
 * \return True if the character is blankspace
 */
bool isBlankSpace(char c)
{
    return (c == '\n') || (c == '\r') || (c == '\t') || (c == ' ');
}

/**
 * \brief Checks to see if a character is an ASCII representation of hex ((c >= 'A') and (c <= 'F')) || ((c >= 'a') and (c <= 'f'))
 * \param[in] c  character to check
 * \return True if the character is a hex
 */
bool isAlpha(char c)
{
    return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
}

/**
 * \brief Checks to see if a character is an ASCII representation of hex ((c >= 'A') and (c <= 'F')) || ((c >= 'a') and (c <= 'f'))
 * \param[in] c  character to check
 * \return True if the character is a hex
 */
bool isHexAlpha(char c)
{
    return ((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f'));
}

/**
 * \brief Returns true if this character is a valid hex character or if this is blankspace (The character can be
 *        included in a valid hexstring).
 * \param[in] c  character to check
 * \return True if the character can be included in a valid hexstring
 */
bool isHex(char c)
{
    return isHexDigit(c) || isBlankSpace(c);
}

/**
 * \brief Returns true if this character is a valid hex character.
 * \param[in] c  character to check
 * \return True if the character can be included in a valid hexstring
 */
bool isHexDigit(char c)
{
    return isDigit(c) || isHexAlpha(c);
}

/**
 * \brief Remove spaces from a ASCII hex string.
 * \param[in]    ascii_hex      Initial hex string to remove blankspace from
 * \param[in]    ascii_hex_len  Length of the initial hex string
 * \param[in]    packed_hex     Resulting hex string without blankspace
 * \param[in,out] packed_len     In: Size to packed_hex buffer
 *                              Out: Number of bytes in the packed hex string
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS packHex(const char* ascii_hex, size_t ascii_hex_len, char* packed_hex, size_t* packed_len)
{
    size_t i = 0;
    size_t j = 0;

    // Verify the inputs
    if ((ascii_hex == NULL) || (packed_hex == NULL) || (packed_len == NULL))
    {
        return ATCA_BAD_PARAM;
    }
    // Loop through each character and only add the hex characters
    for (i = 0; i < ascii_hex_len; i++)
    {
        if (isHexDigit(ascii_hex[i]))
        {
            if (j > *packed_len)
            {
                break;
            }
            packed_hex[j++] = ascii_hex[i];
        }
    }
    *packed_len = j;

    return ATCA_SUCCESS;
}

#ifdef ATCA_PRINTF
/** \brief Print each hex character in the binary buffer with spaces between bytes
 *  \param[in] label label to print
 *  \param[in] binary input buffer to print
 *  \param[in] bin_len length of buffer to print
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_printbin_label(const char* label, uint8_t* binary, size_t bin_len)
{
    printf("%s", label);
    return atcab_printbin(binary, bin_len, true);
}

/** \brief Print each hex character in the binary buffer with spaces between bytes
 *  \param[in] binary input buffer to print
 *  \param[in] bin_len length of buffer to print
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_printbin_sp(uint8_t* binary, size_t bin_len)
{
    return atcab_printbin(binary, bin_len, true);
}

/** \brief Print each hex character in the binary buffer
 *  \param[in] binary input buffer to print
 *  \param[in] bin_len length of buffer to print
 *  \param[in] add_space indicates whether spaces and returns should be added for pretty printing
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_printbin(uint8_t* binary, size_t bin_len, bool add_space)
{
    size_t i = 0;
    size_t line_len = 16;

    // Verify the inputs
    if (binary == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    // Set the line length
    line_len = add_space ? 16 : 32;

    // Print the bytes
    for (i = 0; i < bin_len; i++)
    {
        // Print the byte
        if (add_space)
        {
            printf("%02X ", binary[i]);
        }
        else
        {
            printf("%02X", binary[i]);
        }

        // Break at the line_len
        if ((i + 1) % line_len == 0)
        {
            printf("\r\n");
        }
    }
    // Print the last carriage return
    printf("\r\n");

    return ATCA_SUCCESS;
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Base 64 Encode/Decode

#define B64_IS_EQUAL   (uint8_t)64
#define B64_IS_INVALID (uint8_t)0xFF

/**
 * \brief Returns true if this character is a valid base 64 character or if this is space (A character can be
 *        included in a valid base 64 string).
 * \param[in] c      character to check
 * \param[in] rules  base64 ruleset to use
 * \return True if the character can be included in a valid base 64 string
 */
bool isBase64(char c, const uint8_t * rules)
{
    return isBase64Digit(c, rules) || isBlankSpace(c);
}

/**
 * \brief Returns true if this character is a valid base 64 character.
 * \param[in] c      character to check
 * \param[in] rules  base64 ruleset to use
 * \return True if the character can be included in a valid base 64 string
 */
bool isBase64Digit(char c, const uint8_t * rules)
{
    return isDigit(c) || isAlpha(c) || c == rules[0] || c == rules[1] || c == rules[2];
}

/**
 * \brief Returns the base 64 index of the given character.
 * \param[in] c      character to check
 * \param[in] rules  base64 ruleset to use
 * \return the base 64 index of the given character
 */
uint8_t base64Index(char c, const uint8_t * rules)
{
    if ((c >= 'A') && (c <= 'Z'))
    {
        return (uint8_t)(c - 'A');
    }
    if ((c >= 'a') && (c <= 'z'))
    {
        return (uint8_t)(26 + c - 'a');
    }
    if ((c >= '0') && (c <= '9'))
    {
        return (uint8_t)(52 + c - '0');
    }
    if (c == rules[0])
    {
        return (uint8_t)62;
    }
    if (c == rules[1])
    {
        return (uint8_t)63;
    }
    if (c == rules[2])
    {
        return B64_IS_EQUAL;
    }
    return B64_IS_INVALID;
}

/**
 * \brief Returns the base 64 character of the given index.
 * \param[in] id     index to check
 * \param[in] rules  base64 ruleset to use
 * \return the base 64 character of the given index
 */
char base64Char(uint8_t id, const uint8_t * rules)
{
    if (id < 26)
    {
        return (char)('A' + id);
    }
    if ((id >= 26) && (id < 52))
    {
        return (char)('a' + id - 26);
    }
    if ((id >= 52) && (id < 62))
    {
        return (char)('0' + id - 52);
    }
    if (id == 62)
    {
        return rules[0];
    }
    if (id == 63)
    {
        return rules[1];
    }

    if (id == B64_IS_EQUAL)
    {
        return rules[2];
    }
    return B64_IS_INVALID;
}

static ATCA_STATUS atcab_base64decode_block(const uint8_t id[4], uint8_t* data, size_t* data_size, size_t data_max_size)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    size_t new_bytes = 0;

    do
    {
        // Make sure padding characters can only be the last two
        if ((id[0] == B64_IS_EQUAL) ||
            (id[1] == B64_IS_EQUAL) ||
            (id[2] == B64_IS_EQUAL && id[3] != B64_IS_EQUAL))
        {
            status = ATCA_TRACE(ATCA_BAD_PARAM, "Base64 chars after end padding");
            break;
        }

        // Make sure output buffer has enough space
        if (id[2] == B64_IS_EQUAL)
        {
            new_bytes = 1;
        }
        else if (id[3] == B64_IS_EQUAL)
        {
            new_bytes = 2;
        }
        else
        {
            new_bytes = 3;
        }
        if ((*data_size) + new_bytes > data_max_size)
        {
            status = ATCA_TRACE(ATCA_BAD_PARAM, "decoded buffer too small");
            break;
        }

        // Decode into output buffer
        data[(*data_size)++] = (uint8_t)((id[0] << 2) | (id[1] >> 4));
        if (id[2] == B64_IS_EQUAL)
        {
            break;
        }
        data[(*data_size)++] = (uint8_t)((id[1] << 4) | (id[2] >> 2));
        if (id[3] == B64_IS_EQUAL)
        {
            break;
        }
        data[(*data_size)++] = (uint8_t)((id[2] << 6) | id[3]);
    }
    while (false);

    return status;
}

/**
 * \brief Decode base64 string to data with ruleset option.
 *
 * \param[in]    encoded       Base64 string to be decoded.
 * \param[in]    encoded_size  Size of the base64 string in bytes.
 * \param[out]   data          Decoded data will be returned here.
 * \param[in,out] data_size     As input, the size of the byte_array buffer.
 *                             As output, the length of the decoded data.
 * \param[in]    rules         base64 ruleset to use
 */
ATCA_STATUS atcab_base64decode_(const char* encoded, size_t encoded_size, uint8_t* data, size_t* data_size, const uint8_t * rules)
{
    ATCA_STATUS status = ATCA_SUCCESS;
    uint8_t id[4];
    int id_index = 0;
    size_t enc_index = 0;
    size_t data_max_size;
    bool is_done = false;

    do
    {
        // Check the input parameters
        if (encoded == NULL || data == NULL || data_size == NULL || rules == NULL)
        {
            status = ATCA_TRACE(ATCA_BAD_PARAM, "Null input parameter");
            break;
        }
        data_max_size = *data_size;
        *data_size = 0;

        // Start decoding the input data
        for (enc_index = 0; enc_index < encoded_size; enc_index++)
        {
            if (isBlankSpace(encoded[enc_index]))
            {
                continue; // Skip any empty characters
            }
            if (!isBase64Digit(encoded[enc_index], rules))
            {
                status = ATCA_TRACE(ATCA_BAD_PARAM, "Invalid base64 character");
                break;
            }
            if (is_done)
            {
                // We found valid base64 characters after end padding (equals)
                // characters
                status = ATCA_TRACE(ATCA_BAD_PARAM, "Base64 chars after end padding");
                break;
            }
            id[id_index++] = base64Index(encoded[enc_index], rules);
            // Process data 4 characters at a time
            if (id_index >= 4)
            {
                id_index = 0;
                status = atcab_base64decode_block(id, data, data_size, data_max_size);
                if (status != ATCA_SUCCESS)
                {
                    break;
                }

                is_done = (id[3] == B64_IS_EQUAL);
            }
        }
        if (status != ATCA_SUCCESS)
        {
            break;
        }
        if (id_index)
        {
            if (id_index < 2)
            {
                status = ATCA_TRACE(ATCA_BAD_PARAM, "Invalid number of base64 chars");
                break;
            }
            // End of base64 string, but no padding characters
            for (; id_index < 4; id_index++)
            {
                id[id_index] = B64_IS_EQUAL;
            }
            status = atcab_base64decode_block(id, data, data_size, data_max_size);
        }
    }
    while (false);

    return status;
}

/** \brief Encode data as base64 string with ruleset option. */
ATCA_STATUS atcab_base64encode_(
    const uint8_t*  data,         /**< [in] The input byte array that will be converted to base 64 encoded characters */
    size_t          data_size,    /**< [in] The length of the byte array */
    char*           encoded,      /**< [in] The output converted to base 64 encoded characters. */
    size_t*         encoded_size, /**< [inout] Input: The size of the encoded buffer, Output: The length of the encoded base 64 character string */
    const uint8_t * rules         /**< [in] ruleset to use during encoding */
    )
{
    ATCA_STATUS status = ATCA_SUCCESS;
    size_t data_idx = 0;
    size_t b64_idx = 0;
    size_t offset = 0;
    uint8_t id = 0;
    size_t b64_len;

    do
    {
        // Check the input parameters
        if (encoded == NULL || data == NULL || encoded_size == NULL || !rules)
        {
            status = ATCA_TRACE(ATCA_BAD_PARAM, "Null input parameter");
            break;
        }

        // Calculate output length for buffer size check
        b64_len = (data_size / 3 + (data_size % 3 != 0)) * 4; // ceil(size/3)*4
        if (rules[3])
        {
            // We add newlines to the output
            if (rules[3] % 4 != 0)
            {
                status = ATCA_TRACE(ATCA_BAD_PARAM, "newline rules[3] must be multiple of 4");
                break;
            }
            b64_len += (b64_len / rules[3]) * 2;
        }
        b64_len += 1; // terminating null
        if (*encoded_size < b64_len)
        {
            status = ATCA_TRACE(ATCA_SMALL_BUFFER, "Length of encoded buffer too small");
            break;
        }
        // Initialize the return length to 0
        *encoded_size = 0;

        // Loop through the byte array by 3 then map to 4 base 64 encoded characters
        for (data_idx = 0; data_idx < data_size; data_idx += 3)
        {
            // Add \r\n every n bytes if specified
            if (rules[3] && data_idx > 0 && (b64_idx - offset) % rules[3] == 0)
            {
                // as soon as we do this, we introduce an offset
                encoded[b64_idx++] = '\r';
                encoded[b64_idx++] = '\n';
                offset += 2;
            }

            id = (data[data_idx] & 0xFC) >> 2;
            encoded[b64_idx++] = base64Char(id, rules);
            id = (uint8_t)((data[data_idx] & 0x03) << 4);
            if (data_idx + 1 < data_size)
            {
                id |= (data[data_idx + 1] & 0xF0) >> 4;
                encoded[b64_idx++] = base64Char(id, rules);
                id = (uint8_t)((data[data_idx + 1] & 0x0F) << 2);
                if (data_idx + 2 < data_size)
                {
                    id |= (data[data_idx + 2] & 0xC0) >> 6;
                    encoded[b64_idx++] = base64Char(id, rules);
                    id = data[data_idx + 2] & 0x3F;
                    encoded[b64_idx++] = base64Char(id, rules);
                }
                else
                {
                    encoded[b64_idx++] = base64Char(id, rules);
                    encoded[b64_idx++] = base64Char(B64_IS_EQUAL, rules);
                }
            }
            else
            {
                encoded[b64_idx++] = base64Char(id, rules);
                encoded[b64_idx++] = base64Char(B64_IS_EQUAL, rules);
                encoded[b64_idx++] = base64Char(B64_IS_EQUAL, rules);
            }
        }

        // Strip any trailing nulls
        while (b64_idx > 1 && encoded[b64_idx - 1] == 0)
        {
            b64_idx--;
        }

        // Null terminate end
        encoded[b64_idx++] = 0;

        // Set the final encoded length (excluding terminating null)
        *encoded_size = b64_idx - 1;
    }
    while (false);
    return status;
}


/**
 * \brief Encode data as base64 string
 *
 * \param[in]    byte_array   Data to be encode in base64.
 * \param[in]    array_len    Size of byte_array in bytes.
 * \param[in]    encoded      Base64 output is returned here.
 * \param[in,out] encoded_len  As input, the size of the encoded buffer.
 *                            As output, the length of the encoded base64
 *                            character string.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_base64encode(const uint8_t* byte_array, size_t array_len, char* encoded, size_t* encoded_len)
{
    return atcab_base64encode_(byte_array, array_len, encoded, encoded_len, atcab_b64rules_default);
}

/**
 * \brief Decode base64 string to data
 *
 * \param[in]    encoded     Base64 string to be decoded.
 * \param[in]    encoded_len  Size of the base64 string in bytes.
 * \param[out]   byte_array   Decoded data will be returned here.
 * \param[in,out] array_len    As input, the size of the byte_array buffer.
 *                            As output, the length of the decoded data.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS atcab_base64decode(const char* encoded, size_t encoded_len, uint8_t* byte_array, size_t* array_len)
{
    return atcab_base64decode_(encoded, encoded_len, byte_array, array_len, atcab_b64rules_default);
}

#if !defined(ATCA_PLATFORM_MEMSET_S) && !defined(memset_s)
/**
 * \brief Guaranteed to perform memory writes regardless of optimization level. Matches memset_s signature
 */
int atcab_memset_s(void* dest, size_t destsz, int ch, size_t count)
{
    if (dest == NULL)
    {
        return -1;
    }
    if (destsz > SIZE_MAX)
    {
        return -1;
    }
    if (count > destsz)
    {
        return -1;
    }

    volatile unsigned char* p = dest;
    while (destsz-- && count--)
    {
        *p++ = (uint8_t)ch;
    }

    return 0;
}
#endif

#if !defined(ATCA_PLATFORM_STRCASESTR) && !defined(strcasecstr)
/**
 * \brief Search for a substring in a case insenstive format
 */
char * lib_strcasestr(const char *haystack, const char *needle)
{
    const char * h = haystack;
    const char * n = needle;
    const char * m = NULL;

    if (!h || !n)
    {
        return (char*)h;
    }

    while(*h && *n)
    {
        if (*h != *n && *h != toupper(*n))
        {
            if(m)
            {
                /* Restart Matching */
                m = NULL;
                n = needle;
            }
            else 
            {
                /* Continue stepping through the haystack */
                h++;
            }
        }
        else
        {
            if (!m)
            {
                /* Save the start of the match */
                m = h;
            }
            n++;
            h++;
        }
    }

    /* if we reached the end of the needle then it was found */
    return (char*)((!*n) ? m : NULL);
}
#endif
