####################################################
#	:bk=0
#
#	Makefile for dependency graph example from Ch. 9,
#	Dr. Cox's OOP bbok [1986]. Defines the classes
#	Graph and Node.
#
#	This makefile takes .m files and moves directly into .o files.
#
#	This program may be debugged under MANX's sdb product like:
#		sdb -s/methods!/maker main16 depends
#	where -s specifies source directories.  Do a "G app_main",
#	start stepping message expressions!
#
#	Since the class libraries are updated periodically, you might
#	want a make rule to delete the executables.  Sometimes I copy
#	the class libraries to RAM: for faster access.
#
#	28-oct-89	bjw 	Adapted makefile.
#	12-jan-90	bjw 	Added Tree.m series.
#	16-april-90	bjw 	Not dependent on oc-lib.
#	19-feb-91   bjw     UNIX verison of making.
#    21-april-91  bjw   Cleanup/ Better Amiga defines.
#   23-nov-91   bjw     Added class-of-four speed testing.
#   Use LDFLAGS for linking, not LFLAGS (that's for LEX)
#       June 92,        bjw
#   20-nov-92   bjw     Added "norun" method.
#

## vvvvvvvvvvvvvvvvvvv  User Servicable Parts Follow  vvvvvvvvvvvvvvvvvv */

# ---- AMIGA  -------------------------------------

CC = cc
LN = ln
LEX = flex
LDFLAGS = -g
DELETE = delete quiet
CLONE  = copy clone
RENAME = rename

WILD_ALL = #?
WILD_BAK = $(WILD_ALL).bak
TMPDIR = t:
OOCINC = ooc:include/
OOCBIN = Haven:bins/
# OOCLIB = ooc:include/
OOCLIB = LIB:

# -------------------------------------

#	Some actual include files:
TREEINC = dh4:usr/include/tree.h
RELEASE = $(OOCINC)objc/release.h
TEMP = $(TMPDIR)cpp-temp.c

#	The programs we create:
OCT = oct
PLN = prelink

MODEL =
LIBS  = -locbase -locsubs 
COLLECT_LIB = -locCltn
CLIB  = -lc

CFLAGS = $(MODEL) -n -DNO_PROTOTYPE


# ---- UNIX  -------------------------------------
#U 
#U .SUFFIXES: .o .c .h .m
#U 
#U CC = cc -c
#U LN = cc
#U LEX = lex
#U DELETE = rm -f
#U CLONE = cp
#U RENAME = mv
#U 
#U WILD_ALL = *
#U WILD_BAK = $(WILD_ALL).bak $(WILD_ALL)~
#U TMPDIR = /usr/tmp/
#U OOCINC = $(HOME)/myoct/include/
#U OOCBIN = $(HOME)/bin/
#U OOCLIB = $(HOME)/myoct/lib/
#U 
#U MODEL = 
#U LIBS = -locbase -locsubs 
#U COLLECT_LIB = -locCltn
#U CLIB   =
#U 
#U # -------------------------------------
#U 
#U #	Some actual include files:
#U TREEINC = $(OOCINC)tree.h
#U RELEASE = $(OOCINC)objc/release.h
#U TEMP = $(TMPDIR)cpp-temp.c
#U 
#U #	The programs we create:
#U OCT = oct
#U PLN = prelink
#U 
#U CFLAGS = $(MODEL) -DNO_PROTOTYPE -I../usr/include -I$(OOCINC)
#U LDFLAGS = $(MODEL) -L $(OOCLIB)
#U 

##
## ^^^^^^^^^^^^^^^^  No User Servicable Parts Beyound  ^^^^^^^^^^^^^^^^^ */
##


# -------------------------------------

GRAPH_OBJS = Graph.o Node.o Tree.o
GRAPH_METHODS = Graph.m Node.m Tree.m

SPED_OBJS = classa.o classb.o classc.o classd.o
SPED_METHODS = classa.m classb.m classc.m classd.m


# -------------------------------------
#
#	--- New rules ---
#
#     (Seems SysVR2 won't place .o file where I want it, use rename.)
#U .m.o :
#U 	$(OCT) -o $(TMPDIR)XX.c $*.m
#U 	$(CC) $(CFLAGS) $(TMPDIR)XX.c
#U 	$(RENAME) XX.o $*.o

.m.o :
	$(OCT) -o $(TMPDIR)XX.c $*.m
	$(CC) $(CFLAGS) -o $*.o $(TMPDIR)XX.c

# -------------------------------------
#
#	--- Top Level Controls ---
#
all :	main16 hw16 speed16

clean :
	-$(DELETE) $(WILD_BAK) $(WILD_ALL).o main16 main16.dbg
	-$(DELETE) hw16 hw16.dbg

zoo :	zoofile

noruns :
	$(DELETE) main16 hw16 speed16

help :
	@echo "all  -- build dependency and hello-world examples."
	@echo "zoo  -- package source into .zoo file."
	@echo "clean -- delete backup and object files."
	@echo "noruns - delete produced executables only."

# -------------------------------------
#
#	--- project: MAIN ---
#
main16 :	$(GRAPH_OBJS) main.o ww.o
	$(LN) $(LDFLAGS) -o main16 $(GRAPH_OBJS) main.o ww.o $(COLLECT_LIB) $(LIBS) $(CLIB)

ww.o : 	ww.c
	$(CC) $(MODEL) ww.c

ww.c :	Makefile
	prelink ww.c main.m $(GRAPH_METHODS)

Graph.o :	Graph.m Graph.h
Node.o :	Node.m Node.h
Tree.o :	Tree.m
main.o :	main.m


# -------------------------------------
#
#	--- project: HELLO WORLD ---
#
hw16  :	newbyte.o hw.o ww_hw.o $(GRAPH_OBJS)
	$(LN) $(LDFLAGS) -o hw16 newbyte.o hw.o ww_hw.o $(GRAPH_OBJS) $(COLLECT_LIB) $(LIBS) $(CLIB)

ww_hw.o :	ww_hw.c
	$(CC) $(MODEL) ww_hw.c

ww_hw.c :	Makefile
	prelink ww_hw.c hw.m Tree.m newbyte.m


newbyte.o :	newbyte.m
hw.o :	hw.m


# -------------------------------------
#
#	--- project: SPEED TEST ---
#
speed16  :	newbyte.o speed.o $(SPED_OBJS) ww_speed.o 
	$(LN) $(LDFLAGS) -o speed16 speed.o ww_speed.o $(SPED_OBJS) $(LIBS) $(CLIB)

ww_speed.o :	ww_speed.c
	$(CC) $(MODEL) ww_speed.c

ww_speed.c :	Makefile
	prelink ww_speed.c speed.m classa.m classb.m classc.m classd.m

speed.o :	speed.m classa.h classb.h classc.h classd.h
classa.o :	classa.m classa.h
classb.o :	classb.m classb.h
classc.o :	classc.m classc.h
classd.o :	classd.m classd.h


# -------------------------------------
#
#	--- project: Zoo file ---
#
zoofile :
	$(CLONE) Makefile mk-amiga
	zoo -add ram:D *.m *.h mk-amiga
	@$(DELETE) mk-amiga

