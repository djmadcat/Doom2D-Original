.AUTODEPEND

#		*Translator Definitions*
CC = d:\watcom\binb\wcc386 -zq -w3 -d1 -s -olrt -3s -j -i=d:\watcom\prog\libs
TASM = d:\asm\tasm /ml/z/q/t/m5
LINK = d:\watcom\bin\wlink op stack=16K

VGA2OBJ = d:\watcom\bin\vga2obj3
FNT2OBJ = d:\watcom\bin\fnt2obj3

#		*Implicit Rules*
.c.obj:
  $(CC) $<

.asm.obj:
  $(TASM) $<


#		*List Macros*


EXE_dependencies =  \
  main.obj \
  player.obj \
  dots.obj \
  smoke.obj \
  weapons.obj \
  items.obj \
  monster.obj \
  switch.obj \
  fx.obj \
  bmap.obj \
  view.obj \
  menu.obj \
  game.obj \
  files.obj \
  config.obj \
  error.obj \
  memory.obj \
  vga2.obj \
  a8.obj \
  misca.obj \
  miscc.obj

#		*Explicit Rules*

all: start.exe setup.exe install.exe

start.exe: $(EXE_dependencies)
  $(LINK) debug all system dos4g_avl name start @&&|
file main,
player,
dots,
smoke,
weapons,
items,
monster,
switch,
fx,
bmap,
view,
menu,
game,
files,
config,
error,
memory,
vga2,
a8,
misca,
miscc
|

setup.exe: setup.obj doomfnt1.obj doomfnt2.obj setup_v.obj
  $(LINK) system dos4g_avl name setup file setup,setup_v,doomfnt1,doomfnt2

install.exe: install.obj doomfnt1.obj doomfnt2.obj inst_v.obj flame.obj flamef.obj
  $(LINK) system dos4g_avl name install file install,inst_v,doomfnt1,doomfnt2,flame,flamef


#		*Individual File Dependencies*

doomfnt1.obj: vga\doomfnt1.vga
  $(FNT2OBJ) vga\doomfnt1.vga doomfnt1.obj doom_sfnt

doomfnt2.obj: vga\doomfnt2.vga
  $(FNT2OBJ) vga\doomfnt2.vga doomfnt2.obj doom_bfnt

setup_v.obj: vga\setup.vga
  $(VGA2OBJ) vga\setup.vga setup_v.obj doompal

inst_v.obj: vga\inst.vga
  $(VGA2OBJ) vga\inst.vga inst_v.obj doompal

flame.obj: vga\flame.vga
  $(VGA2OBJ) vga\flame.vga flame.obj

main.obj: main.c 

player.obj: player.c 

dots.obj: dots.c 

weapons.obj: weapons.c 

items.obj: items.c 

monster.obj: monster.c 

switch.obj: switch.c 

bmap.obj: bmap.c 

view.obj: view.c 

menu.obj: menu.c 

game.obj: game.c 

files.obj: files.c 

config.obj: config.c 

error.obj: error.c 

memory.obj: memory.c 

a8.obj: a8.c 
  d:\watcom\binb\wcc386 -zq -w3 -d1 -s -olrte -4s -j -i=d:\watcom\prog\libs a8.c

fx.obj: fx.c 
  d:\watcom\binb\wcc386 -zq -w3 -d1 -s -olrte -4s -j -i=d:\watcom\prog\libs fx.c

vga2.obj: vga2.asm 

misca.obj: misca.asm 

miscc.obj: miscc.c 

