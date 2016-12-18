#include <conio.h>
#include <io.h>
#include <fcntl.h>

char scr[4000];

int hexdigit(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return -1;
}

void charstr(char c, int n) {
    int i;

    for (i = 0; i < n; ++i) {
        putch(c);
    }
}

void putstr(int y, char *s) {
    int i, l;

    for (i = l = 0; s[i]; ++i) {
        if (s[i] == '@') {
            switch (s[++i]) {
                case 'b':
                case 't':
                    ++i;
                    break;
                case '@':
                    ++l;
                    break;
                case 0:
                    --i;
                    break;
            }
        } else {
            ++l;
        }
    }
    gotoxy((66 - l) / 2 + 1, y);
    for (i = 0; s[i]; ++i) {
        if (s[i] == '@') {
            switch (s[++i]) {
                case 't':
                    textcolor(hexdigit(s[++i]));
                    break;
                case 'b':
                    textbackground(hexdigit(s[++i]));
                    break;
                case '@':
                    putch('@');
                    break;
                case 0:
                    --i;
                    break;
            }
        } else {
            putch(s[i]);
        }
    }
}

void make_scr(void) {
    textmode(C80);
    textattr(7);
    clrscr();
    window(8, 2, 73, 23);
    textattr(0x4E);
    clrscr();
    window(8, 2, 74, 23);
    textattr(4);
    gotoxy(1, 1);
    charstr('Ü', 66);
//    gotoxy(1, 22);
//    charstr('ß', 66);
    textattr(0x4E);
    gotoxy(2, 3);
    charstr('Ä', 64);
    putstr(2, "@tfDoom2D@te");
    putstr(4, "made by @tbPrikol Software@te in 1996 on the base of @tfDOOM II");
    putstr(5, "@tein Ivanteevka, Russia");
    putstr(7, "@teThank you for playing @tfDoom2D@te. We hope you have as");
    putstr(8, "much fun playing it as we had making it. If you don't, then");
    putstr(9, "try playing @tfDOOM II@te. If you don't like that great game,");
    putstr(10, "play your favorite @tfTetris@te!");
    putstr(17, "@tbProgramming@te: Aleksey Volynskov");
    putstr(18, "@tbGraphics@te: Adrian Carmack, Kevin Cloud, Eugeny Kovtunov");
    putstr(19, "@tbSound@te: Jay Wilbur, Robert Prince");
    putstr(20, "@tbMusic@te: Vladimir Kalinin");
    putstr(21, "@tbLevel design@te: Vladimir Kalinin, Aleksey Volynskov");
    window(1, 1, 80, 25);
    gotoxy(1, 24);
    if (!getch()) {
        getch();
    }
}

int main() {
    int h;

    make_scr();
    window(1, 1, 80, 25);
    gotoxy(1, 25);
    gettext(1, 1, 80, 25, scr);
    h = _creat("endoom.lmp", 0x20);
    write(h, scr, 4000);
    close(h);
    return 0;
}
