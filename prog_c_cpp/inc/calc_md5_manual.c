#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//part of sine wave
const uint32_t md5_constants[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

//rotate left
#define ROTL(x, n)              (((x) << (n)) | ((x) >> (32 - (n))))

//convert to big-endian
static void to_be(uint32_t value, uint8_t *out)
{
    out[0] = (uint8_t)(value & 0xff);
    out[1] = (uint8_t)((value >> 8) & 0xff);
    out[2] = (uint8_t)((value >> 16) & 0xff);
    out[3] = (uint8_t)((value >> 24) & 0xff);
}

static void process_message_block(const uint8_t *block, uint32_t *state)
{
    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];
    uint32_t m[16];

    //split the message block into 16 words and reorder to bytes
    for (int i = 0; i < 16; i++) {
        m[i] = (((uint32_t)block[i * 4 + 0]) << 0)
                | (((uint32_t)block[i * 4 + 1]) << 8)
                | (((uint32_t)block[i * 4 + 2]) << 16)
                | (((uint32_t)block[i * 4 + 3]) << 24);
    }

    //MD5 calculation rounds
    for (int i = 0; i < 64; i++) {
        uint32_t f, g;

        if (i < 16) {
            f = (b & c) | ((~b) & d);
            g = i;
        } else if (i < 32) {
            f = (d & b) | ((~d) & c);
            g = (5 * i + 1) % 16;
        } else if (i < 48) {
            f = b ^ c ^ d;
            g = (3 * i + 5) % 16;
        } else {
            f = c ^ (b | (~d));
            g = (7 * i) % 16;
        }

        uint32_t temp = d;
        d = c;
        c = b;
        b = b + ROTL((a + f + md5_constants[i] + m[g]), 7);
        a = temp;
    }

    //update states
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

static void calc_md5(const uint8_t *message, size_t len, uint8_t *digest)
{
    //Initialize variables
    uint32_t state[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    size_t padded_len = ((len + 8) / 64 + 1) * 64;
    uint8_t *padded_message = (uint8_t *)calloc(padded_len, 1);
    memcpy(padded_message, message, len);
    //Pre-processing: adding a single 1 bit
    padded_message[len] = 0x80;
    //append length of bits
    to_be((uint32_t)(len * 8), padded_message + padded_len - 8);

    //Process the message in successive 512-bit chunks
    for (size_t i = 0; i < padded_len; i += 64) {
        process_message_block(padded_message + i, state);
    }

    //generate the digest
    for (int i = 0; i < 4; i++) {
        to_be(state[i], digest + i * 4);
    }
    
    free(padded_message);
}

void show_md5(const uint8_t* digest)
{
    printf("MD5: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x ", digest[i]);
    }
    printf("\n");
}

int main(void)
{
    const char* message = "This is a book";
    size_t len = strlen(message);
    uint8_t digest[16];

    calc_md5((const uint8_t *)message, len, digest);
    show_md5(digest);

    return 0;
}