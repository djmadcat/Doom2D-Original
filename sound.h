/*
  ��ࠢ����� ��㪮� � ��몮� ��� DOS4GW
  ����� 1.0
  (C) ����ᥩ ����᪮�, 1996
*/

#pragma library("sound.lib")

#ifdef __cplusplus
extern "C" {
#endif

// ���� ⠩���
#define TIMER_FREQ 1193180L

// 1/f ���� ᥪ㭤� � ������� ⠩��� (�. timer)
// ���ਬ��: for(timer=0;timer<tlength(25);); // ��㧠 �� 1/25 ᥪ㭤�
#define tlength(f) (TIMER_FREQ/f)

// ⨯ �ࠩ���
enum {
    ST_NONE = -1,       // ��� �����
    ST_DAC,             // �����⮢� ���
    ST_DMA              // DMA-����
};

// ��������� �����㬥�� (DMI)
typedef struct {
    unsigned int len,       // ����� � �����
        rate,               // ���� � ��.
        lstart,             // ��砫� ����� � ����� �� ��砫� ������
        llen;               // ����� ����� � �����
} snd_t;

// ��⠭����� �ࠩ��� n
// (�. snddrv.h)
void set_snd_drv(short n);

// �஢���� ����稥 ��㪮��� �����
// �����頥� 0, �᫨ ����� ���
short S_detect(void);

// ������� �ࠩ���
void S_init(void);

// �몫���� �ࠩ���
void S_done(void);

// �ந���� ��� s �� ������ c (1-8), ���� r � �஬���� v (0-255)
// �����頥� ����� ������, �� ���஬ ��ࠥ��� ���
// �᫨ c==0, � ��� ������� � �� ᢮����� �����
// r - �� �⭮�⥫쭠� ���� (���筮 1024)
short S_play(snd_t *s, short c, unsigned r, short v);

// ��⠭����� ��� �� ������ c (1-8)
void S_stop(short c);

// ��⠭����� ����� r � ��㪠 �� ������ c
void S_setrate(short c, unsigned r);

// ��⠭����� �஬����� v (0-255) � ��㪠 �� ������ c
void S_setvolume(short c, int v);

// ����� ����
void S_startmusic(void);

// ��⠭����� ����
void S_stopmusic(void);

// ������� �ࠩ��� ⠩���
// ���筮 ����砥��� �� �맮�� S_init()
void T_init(void);

// �몫���� �ࠩ��� ⠩���
// ���筮 �몫�砥��� �� �맮�� S_done()
void T_done(void);

// �஬����� ��㪠 � ��모 (0-128)
extern short snd_vol, mus_vol;

extern char s_interp;

// ����, DMA, IRQ
extern unsigned short snd_port, snd_dma, snd_irq;

// ⨯ ����� (�. ���)
extern short snd_type;

// �������� �����
extern char *snd_name;

// ���� ᬥ蠭���� ��㪠 � �����
extern unsigned short sfreq;

// ⠡��� �⭮�⥫��� ���� ���
extern unsigned short note_tab[96];

// ⠩���
extern volatile int timer;

#ifdef __cplusplus
}
#endif
