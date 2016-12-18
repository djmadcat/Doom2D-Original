#include <stdio.h>
#include <stdlib.h>
#include <dir.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>

#define MAXDMI 1000
#define MAXWAD 2000

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;

typedef struct{
  long len;
  int w;
}dmi_data;

typedef struct{
  long o,l;
  char n[8];
}wad_t;

dmi_data far dd[MAXDMI];
wad_t far wad[MAXWAD];
byte far buf[64000];

int dmi[255];

char str[255],musname[9];

char path[MAXPATH],drive[MAXDRIVE],dir[MAXDIR],file[MAXFILE],ext[MAXEXT];

int dn=0,wadh,fdmi=1;
long wadn=0,wado=12;
FILE *lh;

void addwad(char *n,long l) {
  char s[8];

  memset(s,0,8);
  strncpy(s,n,8);
  memcpy(wad[wadn].n,s,8);
  wad[wadn].o=wado;
  wad[wadn].l=l;
  wado+=l;++wadn;
}

void add_lump(char *fn,char *nm) {
  int h;
  long l,n;

  printf("adding %s as %s\n",fn,nm);
  if((h=_open(fn,O_BINARY|O_RDONLY))==-1) {
	perror("Cannot open lump file");exit(1);
  }
  l=filelength(h);
  addwad(nm,l);
  for(;l;) {
    n=min(l,32000L);
    read(h,buf,n);
	write(wadh,buf,n);
    l-=n;
  }
  close(h);
}

void add_dmm(long o,int in) {
  int h,i;
  long l,n;

  printf("adding %s as %s\n",path,musname);
  if((h=_open(path,O_BINARY|O_RDONLY))==-1) {
	perror("Cannot open DMM file");exit(1);
  }
  l=filelength(h);
  addwad(musname,l);
  for(;o;) {
	n=min(o,32000L);
    read(h,buf,n);
	write(wadh,buf,n);
	o-=n;
  }
  for(i=0;i<in;++i) {
	read(h,str,16);
	memset(str+1,0,13);
	if(dmi[i]!=-1) sprintf(str+1,"DMI%04u",dmi[i]);
	write(wadh,str,16);
  }
  close(h);
}

int add_dmi(void) {
  int h,i,e;
  long len,l,n;

  printf("  checking %s ",path);
  if((h=_open(path,O_BINARY|O_RDONLY))==-1) {
	perror("\nCannot open DMI file");exit(1);
  }
  len=filelength(h);
  for(i=e=0;i<dn;++i) if(dd[i].len==len) {
	lseek(wadh,wad[dd[i].w].o,SEEK_SET);
	lseek(h,0,SEEK_SET);
	for(l=len,e=0;l;) {
	  n=min(l,32000L);
	  read(h,buf,n);
	  if(read(wadh,buf+32000,n)!=n) puts("!!! read error");
	  l-=n;
	  if(memcmp(buf,buf+32000,n)!=0) {e=0;break;} else e=1;
	}
	lseek(wadh,0,SEEK_END);
	if(e) break;
  }
  if(e) {
	printf("= DMI%04u\n",i+fdmi);
	close(h);
	return i+fdmi;
  }
  sprintf(str,"DMI%04u",dn+fdmi);
  printf("- adding as %s\n",str);
  dd[dn].w=wadn;dd[dn].len=len;
  addwad(str,l=len);
  lseek(h,0,SEEK_SET);
  for(;l;) {
    n=min(l,32000L);
    read(h,buf,n);
	write(wadh,buf,n);
    l-=n;
  }
  ++dn;
  close(h);
  return dn+fdmi-1;
}

int getline(void) {
  int i;

  for(;;) {
	if(!fgets(str,250,lh)) return 0;
	i=strlen(str);if(str[i-1]=='\n') str[i-1]=0;
	if(!str[0] || str[0]==';') continue;
	return 1;
  }
}

int main(int argc,char *argv[]) {
  long psz,o;
  FILE *h;
  int i;

  puts("DMM to WAD packer V1.0\n(C) Aleksey Volynskov, 1996\n");
  if(argc<3)
	{puts("Usage: DMM2WAD listfile.lst wadfile.wad [start_number]");return 1;}
  if(argc>=4) fdmi=atoi(argv[3]);
  if(!(lh=fopen(argv[1],"rt"))) {
	perror("Cannot open list file");return 1;
  }
  printf("Making %s\n",argv[2]);
  if((wadh=_creat(argv[2],0x20))==-1) {
	perror("Cannot create WAD file");
	fclose(lh);
	return 1;
  }
  write(wadh,"PWAD",4);
  write(wadh,&wadn,4);
  write(wadh,&wadn,4);
  addwad("M_START",0);
  while(getline()) {
	memset(musname,0,9);
	strncpy(musname,str,8);
	if(!getline()) {
	  puts("Error in list file!");
	  fclose(lh);close(wadh);
	  return 1;
	}
	fnsplit(str,drive,dir,file,ext);
	if(!(h=fopen(str,"rb"))) {
	  perror(str);
	  fclose(lh);close(wadh);
	  return 1;
	}
	fread(str,6,1,h);
	if(memcmp(str,"DMM",4)!=0) {
	  puts("It is not a DMM file!");
	  fclose(h);continue;
	}
	psz=0;fread(&psz,2,1,h);psz<<=2;
	fseek(h,psz,SEEK_CUR);
	psz=0;fread(&psz,1,1,h);
	fseek(h,psz,SEEK_CUR);
	psz=0;fread(&psz,1,1,h);
	o=ftell(h);
	for(i=0;psz;--psz,++i) {
	  fseek(h,1,SEEK_CUR);
	  fread(str,13,1,h);
	  fnmerge(path,drive,dir,"","");
	  if(str[0]) {
		strcat(path,str);
		dmi[i]=add_dmi();
	  }else dmi[i]=-1;
	  fseek(h,2,SEEK_CUR);
	}
	fclose(h);
	fnmerge(path,drive,dir,file,ext);
	add_dmm(o,i);
  }
  addwad("M_END",0);
  puts("Done.");
  write(wadh,wad,wadn*16);
  lseek(wadh,4L,SEEK_SET);
  write(wadh,&wadn,4);
  write(wadh,&wado,4);
  close(wadh);fclose(lh);
  return 0;
}
