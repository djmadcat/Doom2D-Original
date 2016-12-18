#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#include <dir.h>
#include <fcntl.h>

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;

typedef struct {
    char r;
    char g;
    char b;
} rgb_t;

typedef struct {
    long o;
    long l;
    char n[8];
} wad_t;

typedef struct {
    word w;
    word h;
    int sx;
    int sy;
} vgaimg;

char path[MAXPATH];
char drive[MAXDRIVE];
char dir[MAXDIR];
char file[MAXFILE];
char ext[MAXEXT];

int wad_h;
int fh;
byte far buf[64000];
wad_t far wad[4000];
long wadn = 0;
long wado = 12L;

rgb_t pal[256];
rgb_t tmppal[256];
byte cmap[256];

int getbestcolor(int r, int g, int b) {
    int i, best, m, j, rt, gt, bt;

    if (r < 0) {
        r = 0;
    } else if (r > 63) {
        r = 63;
    }
    if (g < 0) {
        g = 0;
    } else if (g > 63) {
        g = 63;
    }
    if (b < 0) {
        b = 0;
    } else if (b > 63) {
        b = 63;
    }
    rt = (r >= g && r >= b) ? 2 : 1;
    gt = (g >= r && g >= b) ? 2 : 1;
    bt = (b >= r && b >= g) ? 2 : 1;
    best = 1;
    m = abs((int) pal[1].r - r) * rt + abs((int) pal[1].g - g) * gt + abs((int) pal[1].b - b) * bt;
    for (i = 2; i < 256; ++i) {
        if (m > (j = abs((int) pal[i].r - r) * rt + abs((int) pal[i].g - g) * gt + abs((int) pal[i].b - b) * bt)) {
            best = i;
            m = j;
        }
    }
    return best;
}

void readstrz(int h, char *s, int m) {
    int i;
    auto char c;

    for (i = 0;; ++i) {
        c = 0;
        read(h, &c, 1);
        if (i < m) {
            s[i] = c;
        }
        if (c == 0) {
            break;
        }
    }
    s[i] = 0;
}

void add(char *n, long l) {
    char s[8];

    memset(s, 0, 8);
    strncpy(s, n, 8);
    memcpy(wad[wadn].n, s, 8);
    wad[wadn].o = wado;
    wad[wadn].l = l;
    wado += l;
    ++wadn;
}

void add_spr(char *fn) {
    int h;
    char s[41];
    vgaimg v;

    printf("Adding graphics from %s\n", fn);
    if ((h = _open(fn, O_BINARY | O_RDONLY)) == -1) {
        perror("Cannot open file");
        close(wad_h);
        exit(1);
    }
    lseek(h, 0x307L, SEEK_SET);
    for (;;) {
        readstrz(h, s, 40);
        if (*s == 0) {
            break;
        }
        read(h, &v, 8);
        printf("пишем %s\n", s);
        add(s, (long) v.w * v.h + 8);
        read(h, buf, v.w * v.h);
        write(wad_h, &v, 8);
        write(wad_h, buf, v.w * v.h);
    }
    close(h);
}

int main(int argc, char *argv[]) {
    int i, j;

    if (argc < 3) {
        puts("Надо: VGA2WAD файл.wad графика.vga");
        return 1;
    }
    printf("Строим %s\n", argv[1]);
    if ((wad_h = _creat(argv[1], 0x20)) == -1) {
        perror("Cannot create file");
        return 1;
    }
    write(wad_h, "PWAD", 4);
    write(wad_h, &wadn, 4);
    write(wad_h, &wadn, 4);
    if ((fh = _open(argv[2], O_BINARY | O_RDONLY)) == -1) {
        perror("Cannot open file");
        close(wad_h);
        return 1;
    }
    lseek(fh, 7, SEEK_SET);
    read(fh, pal, 768);
    close(fh);

    add("W_START", 0);
    add_spr(argv[2]);
    add("W_END", 0);
    puts("Это все.");
    write(wad_h, wad, wadn * 16);
    lseek(wad_h, 4L, SEEK_SET);
    write(wad_h, &wadn, 4);
    write(wad_h, &wado, 4);
    close(wad_h);
    return 0;
}
