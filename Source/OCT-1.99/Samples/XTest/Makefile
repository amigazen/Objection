####################################################
#	:bk=0
#
#	Makefile for test suite.
#
#	These programs may be debugged under MANX's sdb product like:
#		sdb -s/methods!/maker main16 depends
#	where -s specifies source directories.  Do a "G app_main",
#	start stepping message expressions!
#
#	26-oct-89	bjw  	Initial comments.
#	8-nov-90	bjw 	t4.m now compiles.  No more zoo method. use -zt.
#	1-july-22	bjw 	t8.m tests array typedfs and bitfields.
#

## vvvvvvvvvvvvvvvvvvv  User Servicable Parts Follow  vvvvvvvvvvvvvvvvvv */

# ---- AMIGA  -------------------------------------

CC = cc
LN = ln
OCT = /xlator/oct
LEX = flex
DELETE = delete quiet
CLONE  = copy clone
RENAME = rename

WILD_ALL = #?
WILD_BAK = $(WILD_ALL).bak
OOCINC = ooc:include/
OOCBIN = Haven:bins/
OOCLIB = LIB:

#  Your local include directory:
LOCALINCL = dh4:usr/include/

# -------------------------------------

#	Some actual include files:
RELEASE = $(OOCINC)objc/release.h

CFLAGS = -DNO_PROTOTYPE -n

LDFLAGS = -g
CLIBS = tree.o -lc
LIBS = $(OOCLIB)ocbase.lib $(OOCLIB)ocsubs.lib $(CLIBS)

# ---- UNIX  -------------------------------------
#U 
#U .SUFFIXES: .o .c .f .h. .y .l .m .s
#U 
#U CC = cc -c
#U LN = cc
#U LEX = lex
#U OCT = ../xlator/oct
#U DELETE = rm -f
#U CLONE = cp
#U RENAME = mv
#U
#U WILD_ALL = *
#U WILD_BAK = *.bak *~
#U OOCINC = $(HOME)/myoct/include/
#U OOCBIN = $(HOME)/bin/
#U OOCLIB = $(HOME)/myoct/lib/
#U 
#U #  Your local include directory:
#U LOCALINCL = /usr/local/include/

#U 
#U # -------------------------------------
#U 
#U #	Some actual include files:
#U RELEASE = $(OOCINC)objc/release.h
#U 
#U LIBS  = -locbase -locsubs
#U MODEL = -g
#U LDFLAGS = $(MODEL) -L $(OOCLIB)
#U 
#U CFLAGS = -DNO_PROTOTYPE -I ../usr/include -I $(OOCINC) $(MODEL)
#U 

## ^^^^^^^^^^^^^^^^  No User Servicable Parts Beyound  ^^^^^^^^^^^^^^^^^ */

OCTFLAGS = -Wall -zt -zd -zk 


# -------------------------------------

OBJS = test.o t7.o main.o
COBJS = test.c t7.c main.c

ALL_METHODS = test.m

OSUITE = t1.o t2.o t3.o t4.o t5.o t6.o t7.o t8.o

#----------------------------------------------------------
#
#	--- New rules ---
#
.m.c :
	$(OCT) $(OCTFLAGS) $*.m


#   Ignore errors from C compiler:
#A .c.o :
#A	-$(CC) -o $*.o $(CFLAGS) $*.c
.c.o :
	-$(CC) $(CFLAGS) $*.c


#----------------------------------------------------------
#
#	--- Top Level Controls ---
#
all :	suite

app :	main16

clean :
	-$(DELETE) $(WILD_BAK) $(WILD_ALL).o $(WILD_ALL).lst  main16 main16.dbg
	-$(DELETE) $(COBJS)
	-$(DELETE) t$(WILD_ALL).c t$(WILD_ALL).o


suite : $(OSUITE)


help :
	@echo "app   -- generates test application"
	@echo "clean -- cleans up from last test"
	@echo "suite -- translator scrutiny [default]"

#
#	--- project: MAIN ---
#
main16 :	$(OBJS) ww.o
	$(LN) $(LDFLAGS) -o main16 ww.o $(OBJS) $(LIBS)

ww.o : 	ww.c
	$(CC) $(CFLAGS) ww.c

ww.c :
	prelink ww.c main.m $(ALL_METHODS)

test.o :  	test.c
main.o :	main.c
t7.o 	:	t7.c

test.c :  	test.m $(OCT) test.h
main.c :	main.m $(OCT) t7.h test.h
t7.c 	:	t7.m $(OCT) t7.h

#
#	--- project: Test Suites ---
#
t8.o :	t8.c
t6.o :	t6.c
t5.o :	t5.c
t3.o :	t3.c
t2.o :	t2.c
t1.o :	t1.c
t4.o :	t4.c


t8.c :	t8.m $(OCT)
t6.c :	t6.m $(OCT)
t5.c :	t5.m $(OCT)
t4.c :	t4.m $(OCT)
t3.c :	t3.m $(OCT)
t2.c :	t2.m $(OCT)
t1.c :	t1.m $(OCT)


