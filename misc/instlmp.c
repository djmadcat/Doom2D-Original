#include <stdio.h>
#include <dir.h>
#include <string.h>
#include <stdlib.h>

#define BUFSZ 65000L

char far buf[BUFSZ];

struct {
    long o;
    long l;
    char n[8];
} *wad;

int main(int ac, char *av[]) {
    FILE *h;
    static long lmpl;
    static long wn;
    static long wo;
    static char lmpn[40];
    int i;

    if (ac < 3) {
        puts("Usage: INSTLMP file.lmp file.wad");
        return 1;
    }
    fnsplit(av[1], NULL, NULL, lmpn, NULL);
    if (!(h = fopen(av[1], "rb"))) {
        perror(av[1]);
        return 1;
    }
    lmpl = fread(buf, 1, BUFSZ, h);
    fclose(h);
    printf("Loaded %s (%ld bytes)\n", av[1], lmpl);
    if (!(h = fopen(av[2], "rb+"))) {
        perror(av[2]);
        return 1;
    }
    printf("Checking %s\n", av[2]);
    fread(&wn, 4, 1, h);
    if (memcmp(&wn, "IWAD", 4) != 0 && memcmp(&wn, "PWAD", 4) != 0) {
        fclose(h);
        puts("No IWAD or PWAD id");
        return 1;
    }
    fread(&wn, 4, 1, h);
    fread(&wo, 4, 1, h);
    if (!(wad = calloc(wn, sizeof(wad[0])))) {
        fclose(h);
        puts("Not enough memory");
        return 1;
    }
    fseek(h, wo, SEEK_SET);
    fread(wad, wn * sizeof(wad[0]), 1, h);
    for (i = 0; i < wn; ++i) {
        if (strnicmp(wad[i].n, lmpn, 8) == 0) {
            if (wad[i].l != lmpl) {
                fclose(h);
                puts("LUMP size don't match WAD element size!");
                return 1;
            }
            fseek(h, wad[i].o, SEEK_SET);
            fwrite(buf, lmpl, 1, h);
            break;
        }
    }
    fclose(h);
    puts("Done.");
    return 0;
}
