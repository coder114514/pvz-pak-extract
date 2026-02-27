#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

typedef unsigned char byte;

#define DECODED_PAK_PATH "decoded_main.pak"
#define OUTPUT_DIR "/"

typedef struct File File;
struct File
{
    char *path;
    unsigned int length;
    File *nxt;
};

unsigned int write_file(File *, FILE *);
void prepare_path(char *);

int main(void)
{
    FILE *f = fopen(DECODED_PAK_PATH, "rb");
    if (!f)
    {
        fprintf(stderr, "failed to open file\n");
        return 1;
    }

    printf("magic number: ");
    for (int i = 0; i < 9; ++i)
    {
        byte b;
        fread(&b, 1, 1, f);
        printf(" %.2X", b);
    }
    puts("");

    puts("scanning file info");
    File *head = NULL, *tail = NULL;
    unsigned int file_count = 0;
    for (;;)
    {
        File *file = (File *)malloc(sizeof(*file));

        // read name
        byte len;
        fread(&len, 1, 1, f);
        file->path = (char *)malloc(len + 1);
        fread(file->path, 1, len, f);
        file->path[len] = 0;
        printf("file #%u: %s", ++file_count, file->path);

        // read length
        fread(&(file->length), 4, 1, f);

        //// skip trash data (8 bytes)
        //fseek(f, 8, SEEK_CUR);

        // read trash data
        printf("\t\t\t\ttrash data: ");
        for (int i = 0; i < 8; i++)
        {
            byte b;
            fread(&b, 1, 1, f);
            if (i > 0)
                putchar(' ');
            printf("%.2x", b);
        }

        // read flag
        byte flag;
        fread(&flag, 1, 1, f);
        printf("\t\tflag: %.2x\n", flag);

        // append file to the list
        file->nxt = NULL;
        if (!head)
            head = tail = file;
        else
            tail = tail->nxt = file;

        if (flag == 0x80)
            break;
    }

    int status;

    puts("extracting");
    for (File *file = head; file; file = file->nxt)
    {
        status = write_file(file, f);
    }

    puts("cleaning up");
    while (head)
    {
        File *nxt = head->nxt;
        free(head);
        head = nxt;
    }
    return status;
}

unsigned int write_file(File *file, FILE *f)
{
    char filename[PATH_MAX] = OUTPUT_DIR;
    strcat(filename, file->path);
    prepare_path(filename);

    FILE *w = fopen(filename, "wb");
    if (!w)
    {
        fprintf(stderr, "failed to open file %s\n", filename);
        return 1;
    }

    byte *buf = (byte *)malloc(file->length);
    fread(buf, 1, file->length, f);
    fwrite(buf, 1, file->length, w);

    free(buf);
    fclose(w);
    printf("%s extracted\n", file->path);
    return 0;
}

void prepare_path(char *path)
{
    char *dir_end = path;
    while (*dir_end) ++dir_end;
    while (*dir_end != '\\') --dir_end;

    char *p = path;
    while (p <= dir_end)
    {
        if (*p == '\\')
        {
            *p = '\0';
            mkdir(path, 0777);
            *p = '/';
        }
        ++p;
    }
}
