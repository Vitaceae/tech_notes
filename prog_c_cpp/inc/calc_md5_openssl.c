#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>

#define MD5_LEN         64
#define FILE_TO_BE      64

void calc_md5(const unsigned char *data, unsigned long len, unsigned char *md5)
{
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, data, len);
    MD5_Final(md5, &ctx);
}

void calc_file_md5(const char *filename, unsigned char *md5)
{
    FILE *f = fopen(filename, "rb");

    if (f == NULL) {
        printf("failed to open %s\n", filename);
        return;
    }

    MD5_CTX ctx;
    MD5_Init(&ctx);

    unsigned char buf[1024];
    size_t rd;
    while ((rd = fread(buf, 1, sizeof(buf), f)) != 0) {
        MD5_Update(&ctx, buf, rd);
    }

    fclose(f);

    MD5_Final(md5, &ctx);
}

void show_md5(const unsigned char *md5, size_t len)
{
    printf("Calculated MD5:");
    for (int i = 0; i < len; i++) {
        if (i % 8 == 0) {
            printf("\n");
        }
        printf("%02x ", md5[i]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    unsigned char md5[MD5_LEN] = {0};

    if (argc == 2) {
        calc_file_md5(argv[1], md5);
        show_md5(md5, 16);
        return 0;
    }

    const unsigned char data[] = "MD5 Test";
    //exclude the tail null character
    unsigned long len = (unsigned long)sizeof(data) - 1;
    calc_md5(data, len, md5);
    show_md5(md5, MD5_LEN);

    return 0;
}
