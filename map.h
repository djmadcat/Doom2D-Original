/**************************************************************************\
*                                                                          *
*  ��ଠ�� ���� Doom'� 2D                                       ����� 2   *
*                                                                          *
*  Prikol Software                                           10.VII.1996   *
*                                                                          *
*  ����蠥��� ᢮����� �����࠭��� ��� 䠩�, �� �᫮��� ��࠭����   *
*  ��뫪� �� Prikol Software, ���ᨨ � ���� ���᪠ 䠩��.                 *
*                                                                          *
*  ��� include-䠩� ���⠭ �� WATCOM C 10.0 � DOS4GW                    *
*  �� ����� ��।����� ��� �� �� ��㣮� ��/���������, �� ��࠭��  *
*  ��� 䠩� (�᫨ ᮡ�ࠥ��� ��� �����࠭���), � � ᢮�� 㪠���,     *
*  �� �� ������� (��।����).                                             *
*                                                                          *
\**************************************************************************/

#pragma pack(1)

/* ���� �ଠ� - ����� 1.04 alpha (� ࠭��)

  �������� ⥪���� (old_wall_t)
    ���砥��� ���⮩ ��ப�� (old_wall_t.n[0]==0)

  䮭           - ���ᨢ 100x100 ���� - ����� ⥪����

  ⨯ �⥭��    - ���ᨢ 100x100 ����:
    0 - ����
    1 - �⥭�
    2 - ������� �����
    3 - ������ �����
    4 - ��㯥�쪠

  ��।��� ���� - ���ᨢ 100x100 ���� - ����� ⥪����

  ���, ������� � ��. (old_thing_t)
    ���砥��� �㫥�� ⨯�� (old_thing_t.t==0)

  ��४���⥫� (old_switch_t)
    ���砥��� �㫥�� ⨯�� (old_switch_t.t==0)

*/

typedef struct {
    char n[8];              // �������� ⥪�����
    char t;                 // ⨯: 0-ᯫ�譠� 1-"���⪠"
} old_wall_t;

typedef struct {
    short x, y;             // ���न����
    short t;                // ⨯
    unsigned short f;       // 䫠��
} old_thing_t;

typedef struct {
    unsigned char x, y;     // ���न����/8
    unsigned char t;        // ⨯
    unsigned char tm;       // ������ ���� 0
    unsigned char a, b;     // ���筮 - ���न����/8 ����
    unsigned short c;       // �� �ᯮ������ (�த� ��)
} old_switch_t;

/* ���� �ଠ� - ��稭�� � ���ᨨ 1.05 alpha

  ��������� ����� (map_header_t)

  ����� (map_block_t)
    ���砥��� ������ MB_END (map_block_t.t==MB_END)

*/

typedef struct {
    char id[8];             // "�������" - "Doom2D\x1A"
    short ver;              // ����� �����
} map_header_t;

typedef struct {
    short t;                // ⨯ �����
    short st;               // ���⨯ (��⮤ 㯠�����, ���ਬ��)
    // �᫨ �� �ᯮ������, � ������ ���� 0
    // (��� ���饩 ᮢ���⨬���)
    int sz;                 // ࠧ��� (᪮�쪮 ���� ��᫥ �⮩ ��������)
} map_block_t;

enum {
    MB_COMMENT = -1,
    MB_END = 0,
    MB_WALLNAMES,
    MB_BACK,
    MB_WTYPE,
    MB_FRONT,
    MB_THING,
    MB_SWITCH,
    MB_MUSIC,
    MB_SKY,
    MB_SWITCH2,
    MB__UNKNOWN
};

/* ����� 0  (Doom2D ���ᨨ 1.05 alpha)

  MB_COMMENT - �������਩

  MB_WALLNAMES - �������� ⥪���� (�. ����� �����)
    ������⢮ - �� ࠧ���� �����

  MB_BACK,MB_WTYPE,MB_FRONT - 䮭,⨯,��।��� ���� (�. ����� �����)
    ���⨯ 0 - ��� 㯠����� (��� � ��ன ���ᨨ)

  MB_THING - ���,������� � ��. (�. ����� �����)
    ������⢮ - �� ࠧ���� �����

  MB_SWITCH - ��४���⥫� (�. ����� �����)
    ������⢮ - �� ࠧ���� �����

*/

/* ����� 1  (Doom2D ���ᨨ 1.06 alpha)

  MB_WALLNAMES
    ��������� �ᥢ��-⥪����� _WATER_*
      ��� * �� 0=����,1=��᫮�,2=�஢�

  MB_WTYPE
    �������� ���� ⨯ 5 - ����

  MB_MUSIC - ���� ���� - �������� ��모 (8 ����)

  MB_SKY - ���� ���� - ⨯ ���� (2 ���� - short)
    1 = ������
    2 = ��த
    3 = ��

*/

#define SW_PL_PRESS     1
#define SW_MN_PRESS     2
#define SW_PL_NEAR      4
#define SW_MN_NEAR      8
#define SW_KEY_R        16
#define SW_KEY_G        32
#define SW_KEY_B        64

typedef struct {
    unsigned char x, y;     // ���न����/8
    unsigned char t;        // ⨯
    unsigned char tm;       // ������ ���� 0
    unsigned char a, b;     // ���筮 - ���न����/8 ����
    unsigned short c;       // �� �ᯮ������ (�த� ��)
    unsigned char f;        // 䫠��
} switch2_t;

/* ����� 2  (Doom2D ���ᨨ 1.17 alpha)

  ���� MB_SWITCH ������� �� MB_SWITCH2 (�. switch2_t)

*/

#define LAST_MAP_VER 2    // ����� ��᫥���� ����� �����

#pragma pack()

/*  �����  */
