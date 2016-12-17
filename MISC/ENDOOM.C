#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>

char scr[4000];

char pfn[256]="endoom.p";
char lfn[256]="endoom.lmp";

int cline,winw;

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

//  printf("putstr %d,\"%s\"\n",y,s);
  for(i=l=0;s[i];++i) if(s[i]=='@') switch(s[++i]) {
	case 'b':
	case 't': ++i;break;
	case '@': ++l;break;
    case 0: --i;break;
  }else ++l;
  gotoxy((winw-l)/2+1,y);
  for(i=0;s[i];++i) if(s[i]=='@') switch(s[++i]) {
	case 't': textcolor(hexdigit(s[++i]));break;
    case 'b': textbackground(hexdigit(s[++i]));break;
    case '@': putch('@');break;
    case 0: --i;break;
  }else putch(s[i]);
}

void progerr(char *s,...) {
  va_list ap;

  textmode(C80);window(1,1,80,25);textattr(7);clrscr();gotoxy(1,1);
  printf("\nОшибка в строке %d файла %s\n  ",cline,pfn);
  va_start(ap,s);
  vprintf(s,ap);
  va_end(ap);
  puts("");
  exit(1);
}

void make_scr(void) {
  FILE *h;
  char *p,*p2;
  static char str[504],buf[500];
  static int a,b,c,d;

  if(!(h=fopen(pfn,"rt"))) {perror(pfn);exit(1);}
  cline=0;
  while(fgets(str,500,h)) {
	++cline;
	for(p=str;*p;++p) {
	  if(*p=='#') {*p=0;break;}
	  if(*p=='\'')
		for(++p;*p && *p!='\'';++p)
		  if(*p=='\\') if(*(++p)==0) --p;
	  if(*p=='\"')
		for(++p;*p && *p!='\"';++p)
		  if(*p=='\\') if(*(++p)==0) --p;
	  if(*p==0) break;
	}
	if(!(p=strtok(str," \t\r\n"))) continue;
	if(stricmp(p,"window")==0) {
	  if(!(p=strtok(NULL," \t\r\n,;"))) progerr("Недостаточно параметров");
	  a=strtol(p,NULL,0);
	  if(!(p=strtok(NULL," \t\r\n,;"))) progerr("Недостаточно параметров");
	  b=strtol(p,NULL,0);
	  if(!(p=strtok(NULL," \t\r\n,;"))) progerr("Недостаточно параметров");
	  c=strtol(p,NULL,0);
	  if(!(p=strtok(NULL," \t\r\n,;."))) progerr("Недостаточно параметров");
	  d=strtol(p,NULL,0);
	  window(a,b,c,d);
	  winw=c-a;
	}else if(stricmp(p,"textattr")==0) {
	  if(!(p=strtok(NULL," \t\r\n,;."))) progerr("Недостаточно параметров");
	  a=strtol(p,NULL,0);
	  textattr(a);
	}else if(stricmp(p,"textback")==0) {
	  if(!(p=strtok(NULL," \t\r\n,;."))) progerr("Недостаточно параметров");
	  a=strtol(p,NULL,0);
	  textbackground(a);
	}else if(stricmp(p,"textcolor")==0) {
	  if(!(p=strtok(NULL," \t\r\n,;."))) progerr("Недостаточно параметров");
	  a=strtol(p,NULL,0);
	  textcolor(a);
	}else if(stricmp(p,"gotoxy")==0) {
	  if(!(p=strtok(NULL," \t\r\n,;"))) progerr("Недостаточно параметров");
	  a=strtol(p,NULL,0);
	  if(!(p=strtok(NULL," \t\r\n,;."))) progerr("Недостаточно параметров");
	  b=strtol(p,NULL,0);
	  gotoxy(a,b);
	}else if(stricmp(p,"charstr")==0) {
	  for(p=p+strlen(p)+1;*p && *p!='\'';++p)
		if(!strchr(" \t\r\n",*p)) progerr("Вместо ' нашли %c",*p);
	  if(!*p) progerr("Недостаточно параметров");
	  ++p;if(*p=='\\') ++p;
	  if(!*p) progerr("Ошибочный параметр");
	  a=(unsigned char)*p++;
	  if(*p!='\'') progerr("Вместо ' нашли %c",*p);
	  if(!(p=strtok(p+1," \t\r\n,;."))) progerr("Недостаточно параметров");
	  b=strtol(p,NULL,0);
	  charstr(a,b);
	}else if(stricmp(p,"midstr")==0) {
	  if(!(p=strtok(NULL," \t\r\n,;"))) progerr("Недостаточно параметров");
	  a=strtol(p,NULL,0);
	  for(p=p+strlen(p)+1;*p && *p!='\"';++p)
		if(!strchr(" \t\r\n,;",*p)) progerr("Вместо \" нашли %c",*p);
	  if(!*p) progerr("Недостаточно параметров");
	  for(p2=buf;;) {
		++p;if(*p=='\\') {
		  ++p;if(!*p) progerr("Ошибочный параметр");
		  *p2++=*p;continue;
		}
		if(!*p) progerr("Ошибочный параметр");
		if(*p=='\"') {*p2=0;break;}
		*p2++=*p;
	  }
	  putstr(a,buf);
	}else if(stricmp(p,"clrscr")==0) {
	  clrscr();
	}else{
	  progerr("Неизвестная команда '%s'",p);
	}
  }
  fclose(h);
  window(1,1,80,25);
  gotoxy(1,25);
  if(!getch()) getch();
  textmode(C80);
}

int main(int ac,char *av[]) {
  int h;

  if(ac>=2) strcpy(pfn,av[1]);
  if(ac>=3) strcpy(lfn,av[2]);
  make_scr();
  window(1,1,80,25);gotoxy(1,25);
  gettext(1,1,80,25,scr);
  h=_creat(lfn,0x20);
  write(h,scr,4000);
  close(h);
  return 0;
}
