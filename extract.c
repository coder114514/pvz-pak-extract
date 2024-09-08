#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct File
{
    char *name;
    int length;
    struct File *nxt;
} File;

void structwrite(File *, FILE *);
void preppath(char *);

int main(void)
{
    FILE *f = fopen("decoded_main.pak", "rb");
    if (!f)
    {
        fprintf(stderr, "failed to open file\n");
        return 1;
    }

    printf("magic number:\n");
    for (int i = 0; i < 9; i++)
    {
        int b = 0;
        fread(&b, 1, 1, f);
        if (i) putchar(' ');
        printf("%.2x", b);
    }
    puts("");

    puts("scanning file info");
    File *head = NULL, *tail = NULL;
    int flag, len;
    for (int i = 0; ; i++)
    {
        flag = 0, len = 0;
        File *file = (File *)malloc(sizeof(File));

        // read name
        fread(&len, 1, 1, f);
        file->name = (char *)malloc(len + 1);
        fread(file->name, 1, len, f);
        file->name[len] = 0;
        printf("file %d: %s", i, file->name);

        // read length
        fread(&(file->length), 1, 4, f);

        // skip trash data (8 bytes)
        //fseek(f, 8, SEEK_CUR);
        printf("\t\t\t\ttrash data: ");
        for (int i = 0; i < 8; i++)
        {
            int b = 0;
            fread(&b, 1, 1, f);
            printf((i ? " %.2x": "%.2x"), b);
        }
        puts("");

        // read flag
        fread(&flag, 1, 1, f);

        // append file to list
        file->nxt = NULL;
        if (!head)
            head = tail = file;
        else
            tail = tail->nxt = file;

        if (flag == 0x80) break;
    }

    puts("extracting");
    for (File *file = head; file; file = file->nxt)
    {
        structwrite(file, f);
    }

    puts("cleaning up");
    while (head)
    {
        File *nxt = head->nxt;
        free(head);
        head = nxt;
    }
    return 0;
}

void structwrite(File *file, FILE *f)
{
    char filename[200] = "";
    strcat(filename, file->name);
    preppath(filename);
    FILE *w = fopen(filename, "wb");
    unsigned char *buf = (unsigned char *)malloc(file->length);
    fread(buf, 1, file->length, f);
    fwrite(buf, 1, file->length, w);
    free(buf);
    fclose(w);
    printf("%s written\n", filename);
}

void preppath(char *path)
{
    char *dir_end = path;
    while (*dir_end) dir_end++;
    while (*dir_end != '\\') dir_end--;

    char *p = path;
    while (p <= dir_end)
    {
        if (*p == '\\')
        {
            *p = '\0';
            struct stat sb;
            if (stat(path, &sb) || !S_ISDIR(sb.st_mode)) mkdir(path, S_IRWXU);
            *p = '/';
        }
        p++;
    }
}
