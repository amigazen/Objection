#############################################
#		:bk=0 :ts=8
#	Makefile for OO "C" translaor
#	To install locally, set LEX to you lexer program (lex for Unix).
#	Set CC and LN to start just a compile and to just link a program.
#	TEMP is just a temp file somewhere.  OOCINC specifies tree where
#	ooc include files are located.  OOCBIN where executable files
#	will be stored.
#
#	This version is responsible for the translator.
#		sept 89,		bjw
#	Prelinker added again.  Creates file "prelink" in $(PLN).
#		Sept 89,		bjw
#	Use more symbols, less hardcoded dependencies
#		Oct 89, 		bjw
#	Renamed from "OOC" to "OCT" (Objective C Translator).
#		Nov 89,  		bjw
#	Added class.c (class output routines)
#		March 90,		bjw
#   Changed "files.h" into "config.h" file; included by oct.h header.
#       May 28, 1991,   bjw
#   Added mystrs.c for total ANSI string function compatibility.
#       May 30, 1991,   bjw
#

## vvvvvvvvvvvvvvvvvvv  User Servicable Parts Follow  vvvvvvvvvvvvvvvvvv */

# ---- AMIGA  -------------------------------------

CC = cc
LN = ln
LEX = flex
LDFLAGS = -g
LIBS = tree.o -lc
DELETE = delete quiet
CLONE  = copy clone
RENAME = rename

#  Filename Extension (Object file and Executable):
OE = .o
EE =

WILD_ALL = #?
WILD_BAK = $(WILD_ALL).bak
WILD_OBJ = $(WILD_ALL)$(OE)
TMPDIR = t:
OOCINC = ooc:include/
OOCBIN = Haven:bins/bin/
OOCLIB = ooc:include/

# -------------------------------------

#	Some actual include files:
TREEINC = dh4:usr/include/tree.h
RELEASE = $(OOCINC)objc/release.h
TEMP = $(TMPDIR)cpp-temp.c

#	The programs we create:
OCT = oct
PLN = prelink
OCT_CPP = oct-cpp
TEST_FILE = /test/t6.m

#	non-generated OOC files; string ops BUILTIN, sdb stuff, pack strs.
CFLAGS = +x5 -n -DNO_PROTOTYPE +Q
CF_STACK = +m

#	for Generated "C" files (flex): string ops BUILTIN, lots optimize,
#	"-Y140" increase switch symbol table
GCFLAGS = +x5,3 -Y140 -DNO_PROTOTYPE

#  Flex flags:
FFLAGS =


# ---- UNIX  -------------------------------------

#U CC = cc -c
#U LN = cc
#U LEX = lex
#U MODEL = -g
#U LDFLAGS = $(MODEL)
#U LIBS = tree.o -ll
#U DELETE = rm -f
#U CLONE = cp
#U RENAME = mv
#U 
#U #  Filename Extension (Object file and Executable):
#U OE = .o
#U EE =
#U 
#U WILD_ALL = *
#U WILD_BAK = $(WILD_ALL).bak $(WILD_ALL)~
#U WILD_OBJ = $(WILD_ALL)$(OE)
#U TMPDIR = /usr/tmp/
#U OOCINC = $(HOME)/myoct/include/
#U OOCBIN = $(HOME)/bin/
#U OOCLIB = $(HOME)/myoct/lib/
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
#U OCT_CPP = oct-cpp
#U TEST_FILE = ../collect/Set.m
#U 
#U #	non-generated OOC files; string ops BUILTIN, sdb stuff, pack strs.
#U CFLAGS = $(MODEL) -DNO_PROTOTYPE -I../usr/include -I$(OOCINC)
#U CF_STACK =
#U 
#U #	for Generated "C" files (flex): string ops BUILTIN, lots optimize,
#U GCFLAGS = $(CFLAGS)
#U 
#U #  Flex flags:
#U FFLAGS =
#U 

##
## ^^^^^^^^^^^^^^^^  No User Servicable Parts Beyound  ^^^^^^^^^^^^^^^^^ */
##


# -------------------------------------
#
#	--- Reference Symbols ---
#

ALLSRC = gram0.c main.c symbol.c file.c strs.c select.c gram.c expr.c mdef.c\
		class.c error.c info.c hate.c tree.c mystrs.c
MOBJ   = gram0.o main.o file.o strs.o symbol.o select.o gram.o expr.o mdef.o\
		class.o error.o hate.o info.o  mystrs.o
GOBJ   = scan.o
CSRC    = scan.c

ALLHEADR = oct.h $(OOCINC)objc/private/config.h 

# -------------------------------------
#
#	--- Rules ---
#

.l.o:
	$(LEX) $(FFLAGS) $*.l
	@echo "done $(LEX)ing..."
	@-$(DELETE) $*.c
	@$(RENAME) lex.yy.c $*.c
	$(CC) $(GCFLAGS) $*.c


# -------------------------------------
#
#	--- The controllers ---
#
all :	$(OCT) $(PLN)

help :
	@echo "Makefile for Object C translator"
	@echo "make install  -- Ensure /bin version is up to date"
	@echo "make tags     -- Update vi C-tags file"
	@echo "make clean    -- Delete all object files, etc"
	@echo "make zoofile  -- Package up source and Makefile"
	@echo "make test     -- Test our xlator on one module"

install :	$(OCT) $(PLN)
	$(CLONE) $(OCT)  $(OOCBIN)$(OCT)
	$(CLONE) $(PLN)  $(OOCBIN)$(PLN)
	$(CLONE) ccpp/cpp $(OOCBIN)$(OCT_CPP)


tags :	$(ALLHEADR) error.h Makefile
	ctags $(ALLSRC)


clean :
	-$(DELETE) $(WILD_OBJ) $(WILD_BAK) $(TMPDIR)ot$(WILD_ALL) testout.i
	-$(DELETE) $(CSRC) $(WILD_ALL).lst
	-$(DELETE) $(OCT) $(PLN)

test :	$(OCT)
	-$(DELETE) $(TMPDIR)ot$(WILD_ALL)
	$(OCT) -l -w2 -zt -zk -zd $(TEST_FILE)

#U 	$(RENAME) /usr/tmp/ot* testout.i


# -------------------------------------
#
#	---   Projects  ---
#

$(OCT) :	$(GOBJ) $(MOBJ) tree.o
	$(LN) $(LDFLAGS) -o $(OCT) $(MOBJ) $(GOBJ) $(LIBS)


# -------------------------------------
#
#	--- Underworld relations ---
#
# scan.o :	$(ALLHEADR) scan.c 
# scan.c :	$(ALLHEADR) scan.l
scan.l :	$(ALLHEADR) gram.h


main.o : $(ALLHEADR) $(RELEASE) error.h $(OOCINC)objc/objc.h $(OOCINC)octhead.h

class.o :	$(ALLHEADR) gram.h error.h $(TREEINC) $(OOCINC)objc/objc.h
hate.o :	$(ALLHEADR) gram.h error.h $(TREEINC)
strs.o :	$(ALLHEADR)
symbol.o :	$(ALLHEADR) $(TREEINC) gram.h $(OOCINC)objc/objc-class.h error.h
select.o :	$(ALLHEADR) error.h
file.o : 	$(ALLHEADR)
info.o :	$(ALLHEADR) error.h
mdef.o :	$(ALLHEADR) gram.h error.h
gram0.o :	$(ALLHEADR) gram.h


# ----------------------------------
#	"+m"	stack overflow checking.
expr.o :	$(ALLHEADR) error.h gram.h
	$(CC) $(CFLAGS) $(CF_STACK) expr.c

gram.o : 	$(ALLHEADR) gram.h error.h
	$(CC) $(CFLAGS) $(CF_STACK) gram.c


# -------------------------------------
#
#	---   Prelinker  ---
#
$(PLN) :	pln.o mystrs.o
	$(LN) -o $(PLN) pln.o mystrs.o $(LIBS)

pln.o : 	$(RELEASE) $(OOCINC)octhead.h
mystrs.o : 	$(ALLHEADR)


# -------------------------------------
#
#	---   Zoo file  ---
#
zoofile :
	$(CLONE) Makefile mk-amiga
	zoo -add ram:X scan.l *.c *.h mk-amiga
	zoo -delete ram:X scan.c
	@$(DELETE) mk-amiga ram:X.bak

