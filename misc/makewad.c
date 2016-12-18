#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;

int wh;
long wn;
long wo;

typedef struct {
    long o;
    long l;
    char n[8];
} wad_t;

byte far buf[64000];
wad_t far wad[4000];

void add(char *n, long l) {
    char s[9];

    memset(s, 0, 9);
    strncpy(s, n, 8);
    strupr(s);
    memcpy(wad[wn].n, s, 8);
    wad[wn].o = wo;
    wad[wn].l = l;
    wo += l;
    ++wn;
}

void add_lump(char *fn, char *nm) {
    int h;
    long l, n;

    if ((h = _open(fn, O_BINARY | O_RDONLY)) == -1) {
        perror("Cannot open file");
        exit(1);
    }
    l = filelength(h);
    printf("adding %s\n", nm);
    add(nm, l);
    for (; l;) {
        n = min(l, 32000L);
        read(h, buf, n);
        write(wh, buf, n);
        l -= n;
    }
    close(h);
}

int main(int argc, char *argv[]) {
    int i;

    puts("\nWAD Maker V1.0\n(C) Aleksey Volynskov, 1996\n");
    if (argc < 4) {
        puts("Usage: MAKEWAD filename.wad filename.lmp wadname [filename wadname ...]");
        return 1;
    }
    printf("Making %s\n", argv[1]);
    if ((wh = _creat(argv[1], 0x20)) == -1) {
        perror(argv[1]);
        return 1;
    }
    _write(wh, "PWAD", 12);
    wn = 0;
    wo = 12;
    for (i = 2; i + 1 < argc; i += 2) {
        add_lump(argv[i], argv[i + 1]);
    }
    _write(wh, wad, wn * 16);
    lseek(wh, 4, SEEK_SET);
    _write(wh, &wn, 4);
    _write(wh, &wo, 4);
    _close(wh);
    return 0;
}
