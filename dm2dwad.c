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

typedef struct{
  char r,g,b;
}rgb_t;

typedef struct{
  long o,l;
  char n[8];
}wad_t;

typedef struct{
  word w,h;
  int sx,sy;
}vgaimg;

char path[MAXPATH],drive[MAXDRIVE],dir[MAXDIR],file[MAXFILE],ext[MAXEXT];

int wad_h,fh;
byte far buf[64000];
wad_t far wad[4000];
long wadn=0,wado=12L;

byte huge mixmap[256][256];

rgb_t pal[256],tmppal[256];
byte cmap[256];

/*
int getbestcolor(int r,int g,int b) {
  int i,best,m,j,rt,gt,bt;

  if(r<0) r=0; else if(r>63) r=63;
  if(g<0) g=0; else if(g>63) g=63;
  if(b<0) b=0; else if(b>63) b=63;
  rt=(r>=g && r>=b)?2:1;
  gt=(g>=r && g>=b)?2:1;
  bt=(b>=r && b>=g)?2:1;
  best=1;
  m=abs((int)pal[1].r-r)*rt+abs((int)pal[1].g-g)*gt+abs((int)pal[1].b-b)*bt;
  for(i=2;i<256;++i)
    if(m>(j=abs((int)pal[i].r-r)*rt+abs((int)pal[i].g-g)*gt+abs((int)pal[i].b-b)*bt)) {
      best=i;m=j;
    }
  return best;
}
*/

static int sqr(int a) {return a*a;}

int getbestcolor(int r,int g,int b) {
  int i,best,m,j;

  if(r<0) r=0; else if(r>63) r=63;
  if(g<0) g=0; else if(g>63) g=63;
  if(b<0) b=0; else if(b>63) b=63;
  best=1;
  m=sqr((int)pal[1].r-r)+sqr((int)pal[1].g-g)+sqr((int)pal[1].b-b);
  for(i=2;i<256;++i)
	if(m>(j=sqr((int)pal[i].r-r)+sqr((int)pal[i].g-g)+sqr((int)pal[i].b-b))) {
      best=i;m=j;
    }
  return best;
}

void readstrz(int h,char *s,int m) {
  int i;
  auto char c;

  for(i=0;;++i) {
    c=0;
    read(h,&c,1);
    if(i<m) s[i]=c;
    if(c==0) break;
  }
  s[i]=0;
}

void add(char *n,long l) {
  char s[8];

  memset(s,0,8);
  strncpy(s,n,8);
  memcpy(wad[wadn].n,s,8);
  wad[wadn].o=wado;
  wad[wadn].l=l;
  wado+=l;++wadn;
}

void add_lump(char *fn) {
  int h;
  long l,n;

  if((h=_open(fn,O_BINARY|O_RDONLY))==-1) {
    perror("Cannot open file");exit(1);
  }
  l=filelength(h);
  fnsplit(fn,drive,dir,file,ext);
  strupr(file);
  printf("adding %s\n",file);
  add(file,l);
  for(;l;) {
    n=min(l,32000L);
    read(h,buf,n);
    write(wad_h,buf,n);
    l-=n;
  }
  close(h);
}

void add_snd(void) {
  struct ffblk ff;
  int i;
  char s[MAXPATH];

  add("D_START",0L);
  for(i=findfirst("snd\\*.dmi",&ff,0);!i;i=findnext(&ff)) {
    strcat(strcpy(s,"snd\\"),ff.ff_name);
    add_lump(s);
  }
  for(i=findfirst("snd\\_*.snd",&ff,0);!i;i=findnext(&ff)) {
    strcat(strcpy(s,"snd\\"),ff.ff_name);
    add_lump(s);
  }
  add("D_END",0L);
}

void add_wad(char *fn) {
  int h;
  long o,n,sz;
  wad_t w;

  printf("Adding data from %s\n",fn);
  if((h=_open(fn,O_BINARY|O_RDONLY))==-1) {
    perror("Cannot open file");exit(1);
  }
  read(h,&n,4);
  if(memcmp(&n,"IWAD",4)!=0 && memcmp(&n,"PWAD",4)!=0) {
	close(h);puts("No IWAD or PWAD id");exit(1);
  }
  read(h,&n,4);read(h,&o,4);
  for(;n;--n,o+=16) {
	lseek(h,o,SEEK_SET);read(h,&w,16);
	printf("  adding %.8s\n",w.n);
	add(w.n,w.l);
	lseek(h,w.o,SEEK_SET);
	for(;w.l;) {
	  sz=min(w.l,32000L);
	  read(h,buf,sz);
	  write(wad_h,buf,sz);
	  w.l-=sz;
	}
  }
  close(h);
}

void add_spr(char *fn) {
  int h;
  char s[41];
  vgaimg v;

  printf("Adding graphics from %s\n",fn);
  if((h=_open(fn,O_BINARY|O_RDONLY))==-1)
    {perror("Cannot open file");close(wad_h);exit(1);}
  lseek(h,0x307L,SEEK_SET);
  for(;;) {
    readstrz(h,s,40);
    if(*s==0) break;
    read(h,&v,8);
    printf("adding %s\n",s);
    add(s,(long)v.w*v.h+8);
    read(h,buf,v.w*v.h);
    write(wad_h,&v,8);
    write(wad_h,buf,v.w*v.h);
  }
  close(h);
}

void add_vga(char *fn) {
  int h,x,y;
  word i;
  char s[41];
  vgaimg v;
  char map[256];

  printf("Adding non-DOOM graphics from %s\n",fn);
  if((h=_open(fn,O_BINARY|O_RDONLY))==-1)
    {perror("Cannot open file");close(wad_h);exit(1);}
  lseek(h,7L,SEEK_SET);
  read(h,tmppal,768);
  for(i=0;i<256;++i)
    map[i]=getbestcolor(tmppal[i].r,tmppal[i].g,tmppal[i].b);
  for(;;) {
    readstrz(h,s,40);
    if(*s==0) break;
    read(h,&v,8);
    printf("loading %s",s);
    add(s,(long)v.w*v.h+8);
    read(h,buf,v.w*v.h);
    printf(" - converting:\n");
    for(y=0;y<v.h;++y) {
      printf("\r  %3d%%",y*100L/v.h);
      for(x=0;x<v.w;++x)
	buf[i]=map[buf[i=(long)y*v.w+x]];
    }
    printf("\r  done - saving             \n");
    write(wad_h,&v,8);
    write(wad_h,buf,v.w*v.h);
  }
  close(h);
}

void add_scr(char *fn) {
  int h,x,y;
  word i;
  char s[41];
  vgaimg v;
  char map[256];

  printf("Adding screen from %s\n",fn);
  if((h=_open(fn,O_BINARY|O_RDONLY))==-1)
    {perror("Cannot open file");close(wad_h);exit(1);}
  lseek(h,7L,SEEK_SET);
  read(h,tmppal,768);
  for(;;) {
    readstrz(h,s,40);
    if(*s==0) break;
    read(h,&v,8);
    printf("loading %s",s);
	add(s,(long)v.w*v.h+8+768);
    read(h,buf,v.w*v.h);
	write(wad_h,tmppal,768);
	write(wad_h,&v,8);
    write(wad_h,buf,v.w*v.h);
  }
  close(h);
}

void make_mixmap(void) {
  int h,i,j;

  if((h=_open("mixmap.dat",O_BINARY|O_RDONLY))!=-1) {
	close(h);return;
  }
  puts("Making MIXMAP");
  for(i=0;i<256;++i) {
	printf("\r%3d%%",i*100/255);
	mixmap[i][i]=i;
	for(j=i+1;j<256;++j)
	  mixmap[i][j]=mixmap[j][i]=getbestcolor(
		(pal[i].r+pal[j].r)/2,(pal[i].g+pal[j].g)/2,(pal[i].b+pal[j].b)/2
	  );
  }
  puts("");
  if((h=_creat("mixmap.dat",0x20))==-1) {
	perror("Cannot create file MIXMAP.DAT");return;
  }
  for(i=0;i<256;++i)
	write(h,mixmap+i,256);
  close(h);
}

int main() {
  int i,j;

  puts("Making DOOM2D.WAD");
  if((wad_h=_creat("CMRTKA.wad",0x20))==-1) {
    perror("Cannot create file");return 1;
  }
  write(wad_h,"IWAD",4);
  write(wad_h,&wadn,4);
  write(wad_h,&wadn,4);
  add_lump("misc\\endoom.lmp");
//  add_lump("vga\\endanim.anm");
//  add_lump("vga\\end2anim.anm");
  add_wad("levels\\maps.wad");
//  add_wad("levels\\demo.wad");
  if((fh=_open("vga\\spr.vga",O_BINARY|O_RDONLY))==-1) {
    perror("Cannot open file");close(wad_h);return 1;
  }
  lseek(fh,7,SEEK_SET);
  read(fh,pal,768);
  add("PLAYPAL",768);
  write(wad_h,pal,768);

  printf("adding COLORMAP ");
//  add("COLORMAP",256*12+132*3+260);
  add("COLORMAP",256*12);

// 6 уровней красного

  for(i=0;i<12;i+=2) {
    for(j=0;j<256;++j)
      cmap[j]=getbestcolor((63-(int)pal[j].r)*i/11+pal[j].r,
	(0-(int)pal[j].g)*i/11+pal[j].g,(0-(int)pal[j].b)*i/11+pal[j].b);
	write(wad_h,cmap,256);putch('.');
  }

// инверсное моно для эффекта неуязвимости

  for(i=0;i<256;++i) {
	j=63-(pal[i].r+pal[i].g+pal[i].b)/3;
	cmap[i]=getbestcolor(j,j,j);
  }
  write(wad_h,cmap,256);putch('.');

// затемнение для эффекта невидимости

  for(i=0;i<256;++i)
	cmap[i]=getbestcolor(pal[i].r*3/4,pal[i].g*3/4,pal[i].b*3/4);
  write(wad_h,cmap,256);putch('.');

// синее моно - вода

  for(i=0;i<256;++i) {
	j=(pal[i].r+pal[i].g+pal[i].b)/2;
	cmap[i]=getbestcolor(j-63,j-63,j);
  }
  write(wad_h,cmap,256);putch('.');

// зеленое моно - кислота

  for(i=0;i<256;++i) {
	j=(pal[i].r+pal[i].g+pal[i].b)/3;
	cmap[i]=getbestcolor(0,j,0);
  }
  write(wad_h,cmap,256);putch('.');

// красное моно - кровь

  for(i=0;i<256;++i) {
	j=(pal[i].r+pal[i].g+pal[i].b)/3;
	cmap[i]=getbestcolor(j,0,0);
  }
  write(wad_h,cmap,256);putch('.');

// молния

  for(i=0;i<256;++i) {
	cmap[i]=getbestcolor(min(pal[i].r+31,63),min(pal[i].g+31,63),min(pal[i].b+31,63));
  }
  write(wad_h,cmap,256);putch('.');

/*
  memset(cmap,0,256);
  write(wad_h,cmap,132);
  write(wad_h,cmap,132);
  write(wad_h,cmap,132);
  write(wad_h,cmap,4);
  write(wad_h,cmap,256);
*/

  close(fh);puts("");

  make_mixmap();
  add_lump("mixmap.dat");

  add_vga("vga\\title.vga");
  add_vga("vga\\interpic.vga");
  add_scr("vga\\cd1.vga");
  add_spr("vga\\endpic.vga");
  add_spr("font.vga");
  add_spr("bigfont.vga");
  add("S_START",0);
  add_spr("vga\\spr.vga");
  add_spr("vga\\player.vga");
  add_spr("spr2.vga");
  add_spr("spr3.vga");
  add_spr("smoke.vga");
  add_spr("vga\\robo.vga");
  add_spr("vga\\sky2.vga");
  add("S_END",0);
  add_snd();
  add_wad("dmm\\music.wad");
  add("W_START",0);
  add_spr("wall.vga");
  add_spr("wall2.vga");
  add_spr("kwalls.vga");
  add_spr("vga\\wall01.vga");
  add("W_END",0);
  puts("Done.");
  write(wad_h,wad,wadn*16);
  lseek(wad_h,4L,SEEK_SET);
  write(wad_h,&wadn,4);
  write(wad_h,&wado,4);
  close(wad_h);
  return 0;
}
