####################################################
#	:bk=0
#
#	Makefile for Amiga Views, yet another windowing interface.
#
#	This makefile takes .m files and moves directly into .o files.
#
#	This program may be debugged under MANX's sdb product like:
#		sdb -s/methods!/maker main16 depends
#	where -s specifies source directories.  Do a "G app_main",
#	start stepping message expressions!  To abort, use "r pc=app_exit"
#	to properly cleanup the system.
#
#	20-mar-90	bjw 	Creation...
#	12-apr-90	bjw 	Added full featured FullView.m
#	9-sept-90	bjw 	.m files dependent on .h files.
#

# -------------------------------------

CC = cc
LN = ln
OCT = oct

TEMPDIR = t:
# OOCLIB = ooc:include/
OOCLIB = LIB:


# -------------------------------------

ALL_METHODS = REM.m Responder.m Window.m OuterView.m View.m Pick.m \
	ScrollBar.m
OBJS 	    = REM.o Responder.o Window.o OuterView.o View.o Pick.o \
	ScrollBar.o

# LIBS 	= $(OOCLIB)ocbase.lib $(OOCLIB)ocsubs.lib
LIBS 	= -locbase -locsubs

OCTFLAG = -W2
MODEL  =
CFLAGS = $(MODEL) -n -DNO_PROTOTYPE
LFLAGS = -g
CLIB   = -lc


# -------------------------------------
#
#	--- New rules ---
#
.m.o :
	$(OCT) $(OCTFLAG) -o $(TEMPDIR)XX.c $*.m
	$(CC) $(CFLAGS) -o $*.o $(TEMPDIR)XX.c


# -------------------------------------
#
#	--- Top Level Controls ---
#
all :	main16

clean :
	-delete #?.bak #?.o main16 main16.dbg

setup :
	xcopy LIB:oc????.lib ram:bin

zoo :	zoofile


# -------------------------------------
#
#	--- project: MAIN ---
#
main16 :	$(OBJS) main.o ww.o
	$(LN) $(LFLAGS) -o main16 $(OBJS) main.o ww.o $(LIBS) $(CLIB)

ww.o : 	ww.c
	$(CC) $(MODEL) ww.c

ww.c :	makefile
	prelink ww.c main.m $(ALL_METHODS)


main.o :	main.m gui.h

REM.o : 	REM.m REM.h
Responder.o:	Responder.m Responder.h
View.o :	View.m View.h
Window.o :	Window.m Window.h
OuterView.o :	OuterView.m OuterView.h
Pick.o :	Pick.m Pick.h
ScrollBar.o :	ScrollBar.m ScrollBar.h


# -------------------------------------
#
#	--- project: Zoo file ---
#
zoofile :
	@-delete quiet #?.bak
	zoo -add T:V *.m *.h Makefile *.mac
	@echo "Zoo file in T:V"


