#include <stdio.h>
#include <limits.h>

typedef unsigned char byte;

#define BASE_DIR ""

int main(void)
{
    FILE *r = fopen(BASE_DIR"main.pak", "rb");
    FILE *w = fopen("decoded_main.pak", "wb");

    if (!r || !w)
    {
        fprintf(stderr, "failed to open files\n");
        return 1;
    }

    byte tmp = 0;
    while (feof(r) == 0)
    {
        fread(&tmp, 1, 1, r);
        tmp ^= 0xF7;
        fwrite(&tmp, 1, 1, w);
    }
    fclose(r), fclose(w);
    printf("done\n");
    return 0;
}
