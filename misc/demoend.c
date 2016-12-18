#include <conio.h>
#include <io.h>
#include <fcntl.h>

char scr[4000];

int hexdigit(char c) {
  if(c>='0' && c<='9') return c-'0';
  if(c>='A' && c<='F') return c-'A'+10;
  if(c>='a' && c<='f') return c-'a'+10;
  return -1;
}

void charstr(char c,int n) {
  int i;

  for(i=0;i<n;++i) putch(c);
}

void putstr(int y,char *s) {
  int i,l;

  for(i=l=0;s[i];++i) if(s[i]=='@') switch(s[++i]) {
    case 'b':
    case 't': ++i;break;
    case '@': ++l;break;
    case 0: --i;break;
  }else ++l;
  gotoxy((66-l)/2+1,y);
  for(i=0;s[i];++i) if(s[i]=='@') switch(s[++i]) {
    case 't': textcolor(hexdigit(s[++i]));break;
    case 'b': textbackground(hexdigit(s[++i]));break;
    case '@': putch('@');break;
    case 0: --i;break;
  }else putch(s[i]);
}

void make_scr(void) {
  textmode(C80);textattr(7);clrscr();
  window(8,2,73,23);textattr(0x4E);clrscr();
  window(8,2,74,23);textattr(4);
  gotoxy(1,1);charstr('▄',66);
//  gotoxy(1,22);charstr('▀',66);
  textattr(0x4E);
  gotoxy(2,3);charstr('─',64);
  putstr(2,"@tfDoom2D@te");
  putstr(4,"сделано @tbPrikol Software@te в 1996 на базе @tfDOOM'а II");
  putstr(5,"@teв городе Ивантеевка Московской области");
  putstr(7,"@teВы играли в неполную версию @tfDoom'а 2D@te. В ней только");
  putstr(8,"один уровень, в deathmatch можно играть не более 10 минут,");
  putstr(9,"нельзя добавлять свои WAD'ы (уровни и графику).");
  putstr(11,"В полной версии вас ждет более 30 уровней, новые монстры,");
  putstr(12,"новое оружие и много приколов. Вы сможете создавать свои");
  putstr(13,"уровни и играть в deathmatch сколько угодно.");
  putstr(15,"Полная версия появится в Москве в начале августа 1996 года.");
  putstr(16,"Возможно, вы сможете найти ее на Митинском рынке.");
  putstr(18,"@tbПрограмма@te: Алексей Волынсков");
  putstr(19,"@tbГрафика@te: Адриан Кармак, Кевин Клауд, Евгений Ковтунов");
  putstr(20,"@tbЗвук@te: Джей Уилбур, Роберт Принс");
  putstr(21,"@tbМузыка@te: Владимир Калинин");
  window(1,1,80,25);gotoxy(1,24);
  if(!getch()) getch();
}

int main() {
  int h;

  make_scr();
  window(1,1,80,25);gotoxy(1,25);
  gettext(1,1,80,25,scr);
  h=_creat("endoom.lmp",0x20);
  write(h,scr,4000);
  close(h);
  return 0;
}
