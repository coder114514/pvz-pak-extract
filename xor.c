#include <stdio.h>
#include <limits.h>

typedef unsigned char byte;

#define XOR_MASK 0xF7

#define PAK_PATH "main.pak"
#define OUTPUT_PATH "decoded_main.pak"

int main(void)
{
    FILE *r = fopen(PAK_PATH, "rb");
    FILE *w = fopen(OUTPUT_PATH, "wb");

    if (!r || !w)
    {
        fprintf(stderr, "failed to open files\n");
        return 1;
    }

    byte tmp = 0;
    while (feof(r) == 0)
    {
        fread(&tmp, 1, 1, r);
        tmp ^= XOR_MASK;
        fwrite(&tmp, 1, 1, w);
    }
    fclose(r), fclose(w);
    printf("done\n");
    return 0;
}
