#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;

int wh, mh;
long wn, wo;

typedef struct {
    long o;
    long l;
    char n[8];
} wad_t;

byte far buf[64000];
wad_t far wad[4000];
byte used[4000];

int find(char *s) {
    int i;

    for (i = 0; i < wn; ++i) {
        if (strnicmp(s, wad[i].n, 8) == 0) {
            return i;
        }
    }

    return -1;
}

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

int main() {
    int i;
    FILE *h;
    char s[20];
    long n, l;
    unsigned sz;

    if ((mh = _open("main.wad", O_BINARY | O_RDONLY)) == -1) {
        perror("open");
        return 1;
    }
    lseek(mh, 4, SEEK_SET);
    _read(mh, &wn, 4);
    _read(mh, &wo, 4);
    lseek(mh, wo, SEEK_SET);
    _read(mh, wad, wn * 16);
    memset(used, 0, sizeof(used));
    for (i = 0; i < wn; ++i) {
        if (!wad[i].l) {
            used[i] = 1;
        }
    }
    if (!(h = fopen("demo.txt", "rt"))) {
        perror("text");
        return 1;
    }
    while (fgets(s, 15, h)) {
        if (strlen(s) >= 1) {
            if (s[strlen(s) - 1] == '\n') {
                s[strlen(s) - 1] = 0;
            }
        }
        printf("use %s ", s);
        if ((i = find(s)) == -1) {
            puts("- not found");
            continue;
        }
        used[i] = 1;
        puts("- ok");
    }
    fclose(h);
    if ((wh = _creat("doom2d.wad", 0x20)) == -1) {
        perror("create");
        return 1;
    }
    _write(wh, "IWAD", 12);
    wo = 12;
    for (i = 0, n = 0; i < wn; ++i) {
        if (used[i]) {
            printf("adding %.8s\n", wad[i].n);
            lseek(mh, wad[i].o, SEEK_SET);
            wad[i].o = wo;
            for (l = wad[i].l; l; l -= sz) {
                sz = min(l, 64000);
                _read(mh, buf, sz);
                _write(wh, buf, sz);
                wo += sz;
            }
            ++n;
        }
    }
    for (i = 0; i < wn; ++i) {
        if (used[i]) {
            _write(wh, &wad[i], 16);
        }
    }
    lseek(wh, 4, SEEK_SET);
    _write(wh, &n, 4);
    _write(wh, &wo, 4);
    _close(wh);
    return 0;
}
