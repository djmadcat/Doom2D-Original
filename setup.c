#include <stdio.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <fsound.h>
#include <snddrv.h>
#include <keyb.h>
#include <vga.h>
#include <averr.h>

enum {
    END,
    BYTE,
    WORD,
    DWORD,
    SWITCH
};

enum {
    NONE,
    MAIN,
    SOUND,
    SCARD,
    SPORT,
    SDMA,
    SIRQ,
    SFREQ,
    RKPL,
    RKEYS,
    TESTYN,
    SNDYN,
    TESTSND,
    MAX_MNU
};

int mnu = NONE;
int swmnu = MAIN;
int need_exit = 0;
int redraw = 0;
int def_k = 0;
int cur[MAX_MNU];
int total[MAX_MNU] = { 0, 3, 6, SDRV__END, 0, 0, 0, 10, 2, 9, 2, 2, 0 };

extern vgapal doompal;
extern vgaimg cursor;
extern int doom_sfnt, doom_bfnt;
extern short *snd_par;
vgaimg *back = NULL;
snd *snd1, *snd2;

unsigned char plk[2][9] = {
    { 75, 77, 72, 80, 0x9D, 0xB8, 73, 71, 54 },
    { 31, 33, 18, 32, 16, 30, 19, 17, 15 }
};

char newscr[64008];

char cfile[128] = "DEFAULT.CFG";

int snd_card = 0;

typedef struct {
    char *n;
    char t;
    void *p;
} cfg_t;

cfg_t cfg[] = {
    { "sound_card", DWORD, &snd_card },
    { "sound_rate", WORD, &sfreq },
    { "sound_port", WORD, &snd_port },
    { "sound_dma", WORD, &snd_dma },
    { "sound_irq", WORD, &snd_irq },
    { "pl1_left", BYTE, &plk[0][0] },
    { "pl1_right", BYTE, &plk[0][1] },
    { "pl1_up", BYTE, &plk[0][2] },
    { "pl1_down", BYTE, &plk[0][3] },
    { "pl1_jump", BYTE, &plk[0][4] },
    { "pl1_fire", BYTE, &plk[0][5] },
    { "pl1_next", BYTE, &plk[0][6] },
    { "pl1_prev", BYTE, &plk[0][7] },
    { "pl1_use", BYTE, &plk[0][8] },
    { "pl2_left", BYTE, &plk[1][0] },
    { "pl2_right", BYTE, &plk[1][1] },
    { "pl2_up", BYTE, &plk[1][2] },
    { "pl2_down", BYTE, &plk[1][3] },
    { "pl2_jump", BYTE, &plk[1][4] },
    { "pl2_fire", BYTE, &plk[1][5] },
    { "pl2_next", BYTE, &plk[1][6] },
    { "pl2_prev", BYTE, &plk[1][7] },
    { "pl2_use", BYTE, &plk[1][8] },
    { NULL, END, NULL }
};

void error(int z, int t, int n, char *s1, char *s2) {
    char *m;

    V_done();
    K_done();
    T_done();
    printf(av_ez_msg[z], s1, s2);
    if (t == ET_STD) {
        m = strerror(n);
    } else {
        m = av_err_msg[n];
    }
    printf(":\n  %s\n", m);
    exit(1);
}

static char prbuf[500];

void V_printf(char *s, ...) {
    char *p;
    va_list ap;

    va_start(ap, s);
    vsprintf(prbuf, s, ap);
    va_end(ap);
    for (p = prbuf; *p; ++p) {
        V_putch(*p);
    }
}

void V_prhdr(int y, char *s) {
    vf_font = &doom_bfnt;
    vf_x = 160 - V_strlen(s) / 2;
    vf_y = y;
    V_puts(s);
    vf_font = &doom_sfnt;
}

#define gotoxy(x, y) vf_x=(x);vf_y=(y);

void load_cfg(void) {
    FILE *h;
    char s[505], *p, *p2;
    cfg_t *c;

    printf("Loading %s\n", cfile);
    if (!(h = fopen(cfile, "rt"))) {
        perror("Cannot open file");
        return;
    }
    while (fgets(s, 500, h)) {
        if (!(p = strtok(s, " \r\n\t="))) {
            continue;
        }
        if (*p == ';') {
            continue;
        }
        if (!(p2 = strtok(NULL, " \r\n\t;"))) {
            continue;
        }
        for (c = cfg; c->t; ++c) {
            if (stricmp(p, c->n) == 0) {
                switch (c->t) {
                    case BYTE:
                    case SWITCH:
                        *(char *) c->p = strtol(p2, NULL, 0);
                        if (stricmp(p2, "ON") == 0 || stricmp(p2, "YES") == 0) {
                            *(char *) c->p = 1;
                        } else if (stricmp(p2, "OFF") == 0 || stricmp(p2, "NO") == 0) {
                            *(char *) c->p = 0;
                        }
                        break;
                    case WORD:
                        *(short *) c->p = strtol(p2, NULL, 0);
                        break;
                    case DWORD:
                        *(int *) c->p = strtol(p2, NULL, 0);
                        break;
                }
            }
        }
    }
    fclose(h);
}

void save_cfg(void) {
    FILE *h, *oh;
    char str[505], s[505], *p, *p2;
    cfg_t *c;

    remove("CONFIG.ZZZ");
    rename(cfile, "CONFIG.ZZZ");
    h = fopen("CONFIG.ZZZ", "rt");
    if (!(oh = fopen(cfile, "wt"))) {
        return;
    }
    if (!h) {
        fprintf(oh, ";” ©« ª®­ä¨£ãà æ¨¨\n\n");
        fprintf(oh, "gamma=1\n");
        fprintf(oh, "sound_volume=128\n");
        fprintf(oh, "music_volume=128\n");
        fprintf(oh, "sound_interp=off\n");
    } else {
        while (fgets(str, 500, h)) {
            strcpy(s, str);
            if (!(p = strtok(s, " \r\n\t="))) {
                goto copy;
            }
            if (*p == ';') {
                goto copy;
            }
            if (!(p2 = strtok(NULL, " \r\n\t;"))) {
                goto copy;
            }
            for (c = cfg; c->t; ++c) {
                if (stricmp(p, c->n) == 0) {
                    break;
                }
            }
            if (!c->t) {
                copy:
                fprintf(oh, "%s", str);
            }
        }
    }
    for (c = cfg; c->t; ++c) {
        switch (c->t) {
            case BYTE:
                fprintf(oh, "%s=%u\n", c->n, *(unsigned char *) c->p);
                break;
            case SWITCH:
                fprintf(oh, "%s=%s\n", c->n, (*(char *) c->p) ? "on" : "off");
                break;
            case WORD:
                fprintf(oh, "%s=%u\n", c->n, *(unsigned short *) c->p);
                break;
            case DWORD:
                fprintf(oh, "%s=%u\n", c->n, *(unsigned *) c->p);
                break;
        }
    }
    if (h) {
        fclose(h);
    }
    fclose(oh);
    remove("CONFIG.ZZZ");
    puts("\nŠ®­ä¨£ãà æ¨ï á®åà ­¥­ .");
}

void load_back(void) {
    FILE *h;
    int n, o;
    struct {
        int o, l;
        char n[8];
    } w;

    if (!(h = fopen("CMRTKA.WAD", "rb"))) {
        V_done();
        K_done();
        T_done();
        puts("File CMRTKA.WAD not found");
        exit(1);
    }
    fseek(h, 4, SEEK_SET);
    fread(&n, 4, 1, h);
    fread(&o, 4, 1, h);
    fseek(h, o, SEEK_SET);
    for (; n; --n) {
        fread(&w, 16, 1, h);
        o += 16;
        if (memicmp(w.n, "INTERPIC", 8) == 0) {
            if (!(back = malloc(64008))) {
                break;
            }
            fseek(h, w.o, SEEK_SET);
            fread(back, 64008, 1, h);
            fseek(h, o, SEEK_SET);
        } else if (memicmp(w.n, "DSTEST1", 8) == 0) {
            if (!(snd1 = malloc(w.l + sizeof(*snd1)))) {
                break;
            }
            fseek(h, w.o, SEEK_SET);
            snd1->len = snd1->rate = snd1->lstart = snd1->llen = 0;
            fread(&snd1->len, 2, 1, h);
            fread(&snd1->rate, 2, 1, h);
            fread(&snd1->lstart, 2, 1, h);
            fread(&snd1->llen, 2, 1, h);
            fread(snd1 + 1, w.l, 1, h);
            fseek(h, o, SEEK_SET);
        } else if (memicmp(w.n, "DSTSTEND", 8) == 0) {
            if (!(snd2 = malloc(w.l + sizeof(*snd2)))) {
                break;
            }
            fseek(h, w.o, SEEK_SET);
            snd2->len = snd2->rate = snd2->lstart = snd2->llen = 0;
            fread(&snd2->len, 2, 1, h);
            fread(&snd2->rate, 2, 1, h);
            fread(&snd2->lstart, 2, 1, h);
            fread(&snd2->llen, 2, 1, h);
            fread(snd2 + 1, w.l, 1, h);
            fseek(h, o, SEEK_SET);
        }
    }
    fclose(h);
}

void draw_back(void) {
    if (back) {
        V_pic(0, 0, back);
    } else {
        V_clr(0, 320, 0, 200, 0x97);
    }
}

void prmenu(int x, int y, int c, ...) {
    char **ap;

    if (c >= 0) {
        V_spr(x - 10, y + c * 10, &cursor);
    }
    vf_font = (void *) &doom_sfnt;
    ap = (char **) (&c + 1);
    while (*ap) {
        vf_x = x;
        vf_y = y;
        V_puts(*ap);
        ++ap;
        y += 10;
    }
}

char *keyname(short k) {
    static char *nm[83] = {
        "ESC",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "0",
        "-",
        "=",
        "BACKSPACE",
        "TAB",
        "Q",
        "W",
        "E",
        "R",
        "T",
        "Y",
        "U",
        "I",
        "O",
        "P",
        "[",
        "]",
        "ENTER",
        "‹…‚›‰ CONTROL",
        "A",
        "S",
        "D",
        "F",
        "G",
        "H",
        "J",
        "K",
        "L",
        ";",
        "\"",
        "\'",
        "‹…‚›‰ SHIFT",
        "\\",
        "Z",
        "X",
        "C",
        "V",
        "B",
        "N",
        "M",
        "<",
        ">",
        "/",
        "€‚›‰ SHIFT",
        "*",
        "‹…‚›‰ ALT",
        "Ž…‹",
        "CAPS LOCK",
        "F1",
        "F2",
        "F3",
        "F4",
        "F5",
        "F6",
        "F7",
        "F8",
        "F9",
        "F10",
        "NUM LOCK",
        "SCROLL LOCK",
        "[7]",
        "[8]",
        "[9]",
        "‘…›‰ -",
        "[4]",
        "[5]",
        "[6]",
        "‘…›‰ +",
        "[1]",
        "[2]",
        "[3]",
        "[0]",
        "[.]"
    };
    static char s[20];

    if (k >= 1 && k <= 0x53) {
        return nm[k - 1];
    }
    if (k == 0) {
        return "???";
    }
    if (k == 0x57) {
        return "F11";
    }
    if (k == 0x58) {
        return "F12";
    }
    switch (k) {
        case 0x9C:
            return "‘…›‰ ENTER";
        case 0x9D:
            return "€‚›‰ CONTROL";
        case 0xB8:
            return "€‚›‰ ALT";
        case 0xC7:
            return "HOME";
        case 0xC8:
            return "‚‚…•";
        case 0xC9:
            return "PAGE UP";
        case 0xCB:
            return "‚‹…‚Ž";
        case 0xCD:
            return "‚€‚Ž";
        case 0xCF:
            return "END";
        case 0xD0:
            return "‚ˆ‡";
        case 0xD1:
            return "PAGE DOWN";
        case 0xD2:
            return "INSERT";
        case 0xD3:
            return "DELETE";
    }
    sprintf(s, "Š‹€‚ˆ˜€ #%X", k);
    return s;
}

void drawmenu(int m) {
    short *p;
    int i;
    static char *kn[9] = {
        "‚‹…‚Ž", "‚€‚Ž", "‚‚…•", "‚ˆ‡", "›†ŽŠ", "ŽƒŽœ",
        "‘‹…„. Ž“†ˆ…", "…„. Ž“†ˆ…", "Ž’Š›‚€’œ"
    };

    V_setrect(0, 320, 0, 200);
    draw_back();
    switch (m) {
        case MAIN:
            V_prhdr(35, "SETUP");
            prmenu(100, 80, cur[m], "€‘’Ž‰Š€ ‡‚“Š€", "‚›Ž Š‹€‚ˆ˜", "‚›•Ž„ ‚ DOS", NULL);
            break;
        case SOUND:
            V_prhdr(35, "€‘’Ž‰Š€ ‡‚“Š€");
            gotoxy(100, 80);
            V_printf("Š€’€ %s", strupr(snd_name));
            p = snd_par;
            gotoxy(100, 90);
            V_puts("Ž’ ");
            if (*p) {
                V_printf("%X", snd_port);
            } else {
                V_puts("… “†…");
            }
            p += *p + 1;
            gotoxy(100, 100);
            V_puts("DMA ");
            if (*p) {
                V_printf("%u", snd_dma);
            } else {
                V_puts("… “†Ž");
            }
            p += *p + 1;
            gotoxy(100, 110);
            V_puts("IRQ ");
            if (*p) {
                V_printf("%u", snd_irq);
            } else {
                V_puts("… “†Ž");
            }
            gotoxy(100, 120);
            V_printf("—€‘’Ž’€ %u ƒ–", sfreq);
            gotoxy(100, 130);
            V_puts("Ž‚…Š€");
            V_spr(100 - 10, 80 + cur[m] * 10, &cursor);
            break;
        case TESTSND:
            V_prhdr(90, "Ž‚…Š€ ‡‚“Š€");
            break;
        case TESTYN:
            V_prhdr(35, "…’ ‡‚“ŠŽ‚Ž‰ Š€’›");
            gotoxy(90, 60);
            V_puts("Ž„Ž‹†ˆ’œ Ž‚…Š“ ?");
            gotoxy(10, 80);
            V_puts("(’Ž ŒŽ†…’ ˆ‚…‘’ˆ Š ‡€‚ˆ‘€ˆž ŠŽŒœž’…€)");
            gotoxy(150, 100);
            V_puts("…’");
            gotoxy(150, 110);
            V_puts("„€");
            V_spr(150 - 10, 100 + cur[m] * 10, &cursor);
            break;
        case SNDYN:
        gotoxy(100, 60);
            V_puts("‚› ‘‹›˜€‹ˆ ‡‚“Š ?");
            gotoxy(150, 100);
            V_puts("„€");
            gotoxy(150, 110);
            V_puts("…’");
            V_spr(150 - 10, 100 + cur[m] * 10, &cursor);
            break;
        case SCARD:
            V_prhdr(35, "‚›…ˆ ‡‚“ŠŽ‚“ž Š€’“");
            for (i = 0; i < SDRV__END; ++i) {
                set_snd_drv(i);
                gotoxy(100, 80 + i * 10);
                V_puts(strupr(snd_name));
            }
            V_spr(100 - 10, 80 + cur[m] * 10, &cursor);
            set_snd_drv(snd_card);
            break;
        case SPORT:
            V_prhdr(35, "‚›…ˆ Ž’");
            p = snd_par + 1;
            for (i = 0; i < total[m]; ++i, ++p) {
                gotoxy(130, 80 + i * 10);
                V_printf("Ž’ %X", *p);
            }
            V_spr(130 - 10, 80 + cur[m] * 10, &cursor);
            break;
        case SDMA:
            V_prhdr(35, "‚›…ˆ DMA");
            p = snd_par;
            p += *p + 2;
            for (i = 0; i < total[m]; ++i, ++p) {
                gotoxy(140, 80 + i * 10);
                V_printf("DMA %u", *p);
            }
            V_spr(140 - 10, 80 + cur[m] * 10, &cursor);
            break;
        case SIRQ:
            V_prhdr(35, "‚›…ˆ IRQ");
            p = snd_par;
            p += *p + 1;
            p += *p + 2;
            for (i = 0; i < total[m]; ++i, ++p) {
                gotoxy(140, 80 + i * 10);
                V_printf("IRQ %u", *p);
            }
            V_spr(140 - 10, 80 + cur[m] * 10, &cursor);
            break;
        case SFREQ:
            V_prhdr(35, "‚›…ˆ Š€—…‘’‚Ž ‡‚“Š€");
            prmenu(130, 80, cur[m],
                "5000 ƒ–", "6000 ƒ–", "7000 ƒ–", "8000 ƒ–", "9000 ƒ–",
                "10000 ƒ–", "11000 ƒ–", "16000 ƒ–", "22000 ƒ–", "44000 ƒ–", NULL);
            break;
        case RKPL:
            V_prhdr(35, "‚›Ž Š‹€‚ˆ˜");
            prmenu(100, 80, cur[m], "…‚›‰ ˆƒŽŠ", "‚’ŽŽ‰ ˆƒŽŠ", NULL);
            break;
        case RKEYS:
            V_prhdr(35, "‚›…ˆ Š‹€‚ˆ˜ˆ");
            for (i = 0; i < 9; ++i) {
                gotoxy(90, 80 + i * 10);
                V_puts(kn[i]);
                gotoxy(180, 80 + i * 10);
                V_puts(keyname(plk[cur[RKPL]][i]));
            }
            V_spr(90 - 10, 80 + cur[m] * 10, &cursor);
            break;
    }
}

void setmnu(int m) {
    int y;

    V_setscr(newscr + 8);
    drawmenu(m);
    V_setscr(NULL);
    V_setrect(0, 320, 0, 200);
    for (y = 200; y > 0; y >>= 1) {
        timer = 0;
        V_clr(0, 320, y - 1, 1, 0);
        V_pic(0, y, (vgaimg *) newscr);
        while (timer < 0xFFFF) {
        }
    }
    memcpy(scra, newscr + 8, 64000);
    mnu = m;
}

void menu_kp(short k) {
    short *p;
    static unsigned short ftab[10] = {
        5000, 6000, 7000, 8000, 9000, 10000, 11000, 16000, 22000, 44000
    };

    switch (k) {
        case 1:
            switch (mnu) {
                case MAIN:
                    need_exit = 1;
                    break;
                case SOUND:
                    swmnu = MAIN;
                    break;
                case SCARD:
                    swmnu = SOUND;
                    break;
                case SPORT:
                    swmnu = SOUND;
                    break;
                case SDMA:
                    swmnu = SOUND;
                    break;
                case SIRQ:
                    swmnu = SOUND;
                    break;
                case SFREQ:
                    swmnu = SOUND;
                    break;
                case RKPL:
                    swmnu = MAIN;
                    break;
                case RKEYS:
                    swmnu = RKPL;
                    break;
            }
            break;
        case 0x48:
        case 0xC8:
            if (--cur[mnu] < 0) {
                cur[mnu] = total[mnu] - 1;
            }
            redraw = 1;
            break;
        case 0x50:
        case 0xD0:
        case 0x4C:
            if (++cur[mnu] >= total[mnu]) {
                cur[mnu] = 0;
            }
            redraw = 1;
            break;
        case 0x1C:
        case 0x39:
        case 0x9C:
            switch (mnu) {
                case MAIN:
                    switch (cur[mnu]) {
                        case 0:
                            swmnu = SOUND;
                            break;
                        case 1:
                            swmnu = RKPL;
                            break;
                        case 2:
                            need_exit = 1;
                            break;
                    }
                    break;
                case SOUND:
                    switch (cur[mnu]) {
                        case 0:
                            swmnu = SCARD;
                            break;
                        case 1:
                            if (total[SPORT] > 1) {
                                swmnu = SPORT;
                            }
                            break;
                        case 2:
                            if (total[SDMA] > 1) {
                                swmnu = SDMA;
                            }
                            break;
                        case 3:
                            if (total[SIRQ] > 1) {
                                swmnu = SIRQ;
                            }
                            break;
                        case 4:
                            swmnu = SFREQ;
                            break;
                        case 5:
                            if (!S_detect()) {
                                swmnu = TESTYN;
                                cur[TESTYN] = 0;
                                break;
                            }
                            swmnu = TESTSND;
                            break;
                    }
                    break;
                case TESTYN:
                    swmnu = (cur[mnu]) ? TESTSND : SOUND;
                    break;
                case SNDYN:
                    swmnu = SOUND;
                    break;
                case SCARD:
                    set_snd_drv(snd_card = cur[mnu]);
                    p = snd_par;
                    if (total[SPORT] = *p) {
                        snd_port = p[1];
                    }
                    p += *p + 1;
                    if (total[SDMA] = *p) {
                        snd_dma = p[1];
                    }
                    p += *p + 1;
                    if (total[SIRQ] = *p) {
                        snd_irq = p[1];
                    }
                    cur[SPORT] = cur[SDMA] = cur[SIRQ] = 0;
                    swmnu = SOUND;
                    break;
                case SPORT:
                    p = snd_par;
                    snd_port = p[cur[mnu] + 1];
                    swmnu = SOUND;
                    break;
                case SDMA:
                    p = snd_par;
                    p += *p + 1;
                    snd_dma = p[cur[mnu] + 1];
                    swmnu = SOUND;
                    break;
                case SIRQ:
                    p = snd_par;
                    p += *p + 1;
                    p += *p + 1;
                    snd_irq = p[cur[mnu] + 1];
                    swmnu = SOUND;
                    break;
                case SFREQ:
                    sfreq = ftab[cur[mnu]];
                    swmnu = SOUND;
                    break;
                case RKPL:
                    swmnu = RKEYS;
                    break;
                case RKEYS:
                    plk[cur[RKPL]][cur[RKEYS]] = 0;
                    def_k = 1;
                    V_setscr(scrbuf);
                    drawmenu(mnu);
                    V_setscr(NULL);
                    memcpy(scra, scrbuf, 64000);
                    break;
            }
            break;
    }
}

volatile unsigned char defk_key;

void defk_kp(short k) {
    defk_key = k;
}

int main(int ac, char *av[]) {
    short *p;
    unsigned short f;

    if (ac >= 2)
        strcpy(cfile, av[1]);
    load_cfg();
    load_back();
    K_init();
    ((short *) newscr)[0] = 320;
    ((short *) newscr)[1] = 200;
    memset(newscr + 4, 0, 4);
    memset(cur, 0, sizeof(cur));
    set_snd_drv(snd_card);
    p = snd_par;
    p += (total[SPORT] = *p) + 1;
    p += (total[SDMA] = *p) + 1;
    total[SIRQ] = *p;
    V_init();
    VP_setall(doompal);
    vf_font = (void *) &doom_sfnt;
    vf_color = 0;
    vf_step = -1;
    K_setkeyproc(menu_kp);
    T_init();
    while (!need_exit) {
        if (redraw) {
            V_setscr(scrbuf);
            drawmenu(mnu);
            V_setscr(NULL);
            memcpy(scra, scrbuf, 64000);
            redraw = 0;
        }
        if (def_k) {
            plk[cur[RKPL]][cur[RKEYS]] = 0;
            defk_key = 0;
            K_setkeyproc(defk_kp);
            while (!defk_key) {
            }
            K_setkeyproc(menu_kp);
            plk[cur[RKPL]][cur[RKEYS]] = defk_key;
            def_k = 0;
            redraw = 1;
        }
        if (swmnu) {
            if (mnu == SNDYN && cur[SNDYN] == 0) {
                f = sfreq;
                sfreq = 11025;
                T_done();
                S_init();
                if (snd2 && snd_card) {
                    S_play(snd2);
                }
                while (S_issound()) {
                }
                S_done();
                T_init();
                sfreq = f;
                swmnu = MAIN;
            }
            setmnu(swmnu);
            if (swmnu == TESTSND) {
                f = sfreq;
                sfreq = 11025;
                T_done();
                S_init();
                if (snd1 && snd_card) {
                    S_play(snd1);
                }
                while (S_issound()) {
                }
                S_done();
                T_init();
                sfreq = f;
                swmnu = SNDYN;
                cur[SNDYN] = 1;
            } else {
                swmnu = 0;
            }
        }
    }
    V_done();
    K_done();
    T_done();
    save_cfg();
//    puts("€ â¥¯¥àì, ¯®¤ã¬ ©â¥ å®à®è¥­ìª®!");
    return 0;
}
