.AUTODEPEND

#		*Translator Definitions*
CC = d:\watcom\binb\wcc386 -d1 -s -olrt -3s -j -zq -i=d:\watcom\prog\libs
PP = d:\watcom\binb\wpp386 -d1 -s -olrt -3s -j -zq -i=d:\watcom\prog\libs
TASM = d:\asm\tasm /ml/z/q/t/m5
LINK = d:\watcom\bin\wlink system dos4g_avl op stack=16384 name editor

VGA2OBJ = d:\watcom\bin\vga2obj3

#		*Implicit Rules*
.c.obj:
  $(CC) $<

.cpp.obj:
  $(PP) $<

.asm.obj:
  $(TASM) $<


#		*List Macros*


EXE_dependencies =  \
  main.obj \
  edit.obj \
  view.obj \
  thing.obj \
  switch.obj \
  files.obj \
  config.obj \
  error.obj \
  memory.obj \
  misca.obj \
  vga2.obj \
  editor.obj \
  miscc.obj

#		*Explicit Rules*
editor.exe: $(EXE_dependencies)
  $(LINK) @&&|
file main,
edit,
view,
thing,
switch,
files,
config,
error,
memory,
miscc,
misca,
vga2,
editor
|


#		*Individual File Dependencies*

editor.obj: editor.vga 
	$(VGA2OBJ) editor.vga editor.obj

