############################
#	:bk=0
#	ooc:collect/Makefile
#	
#	Makefile for class files in the foundation.
#    	make  --  produce classes and substrate.
#    	make clean  -- delete all intermediary files.
#    	make lib    -- produce classes and substrate libraries.
#            		when adding new classes, delete clsCtln.ord.
#    	make tex	-- create .tex document (process with LaTeX).
#
#	Libraries are stored in OOCLIB, currently "LIB:".
#
# -------------------------------------


## vvvvvvvvvvvvvvvvvvv  User Servicable Parts Follow  vvvvvvvvvvvvvvvvvv */

# ---- AMIGA  -------------------------------------

CC = cc
LN = ln
LEX = flex
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

CLASSLIB = $(OOCLIB)ocCltn.lib

LIBS  = -locBase -locsubs
CLIB  = -lc
MODEL =

CFLAGS   = $(MODEL) -n -DNO_PROTOTYPE
CF_STACK = +m
#  CFlags just for substrate files:
CS_FLAGS = $(CFLAGS)

LDFLAGS   = -g


# ---- UNIX  -------------------------------------
#U 
#U .SUFFIXES: .o .c .m .h
#U 
#U CC = cc -c
#U LN = cc
#U LEX = lex
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
#U 
#U CLASSLIB = $(OOCLIB)libocCltn.a
#U
#U # MODEL = -g
#U MODEL = 
#U LDFLAGS = $(MODEL) -g
#U LIBS  = -locCltn -locsubs
#U CLIB  =
#U 
#U CFLAGS = $(MODEL) -DNO_PROTOTYPE -I ../usr/include -I $(OOCINC)
#U CF_STACK =
#U #  CFlags just for substrate files:
#U CS_FLAGS = -g $(CFLAGS) -DANSI_PRINTF=0
#U 

##
## ^^^^^^^^^^^^^^^^  No User Servicable Parts Beyound  ^^^^^^^^^^^^^^^^^ */
##


# -------------------------------------

HEADERS  = $(OOCINC)objc/objc.h

OCFLAG = -w2 -p -DANSI_PRINTF=0

# -------------------------------------

HEADERS  = $(OOCINC)objc/objc.h
OCFLAG = -w2
#

#	Store the .tex output somewhere:
TEXFILE = cltn.tex

OBJSIZE = osize$(OE)

COLS = Collection$(OE) Sequence$(OE) Set$(OE) OrdCltn$(OE)
CCOLS = Collection.c Sequence.c Set.c OrdCltn.c
MCOLS = Collection.m Sequence.m Set.m OrdCltn.m



#  All named methods:
ALL_LIB_METHODS = $(MCOLS)
ALL_METHODS = $(ALL_LIB_METHODS)

#	All modules in class library:
ALL_LIB_OBJ = $(COLS)

# ----------------------------------
#	Rules from the Roundtable:
#
.m.c :
    $(OCT) $(OCFLAG) $*.m


# ----------------------------------
#	The beginning has order:
#
all :	main16

zoo :	lib zoofile

install :	main16 lib

lib :	$(CLASSLIB)

tex :	$(TEXFILE)

clean :
    -$(DELETE) main16
    -$(DELETE) $(CCOLS) $(WILD_OBJ) user.c
    -$(DELETE) $(WILD_ALL).tex $(WILD_BAK) $(WILD_ALL).sym $(WILD_ALL).lst

noruns :
	-$(DELETE) main16

help :
	@echo "Makefile for OCT Collection classes."
	@echo " "
	@echo "make all    -- build test of library."
	@echo "make zoo    -- Build library and .zoo backup file."
	@echo "make lib    -- Build and install substrate library."
	@echo "make clean  -- Erase all re-generatable files."
	@echo "make noruns -- Delete produced executables."

# ----------------------------------
#	Create the Collection library:
#	The Collection library is ordered.
#
# vvvvvvvvvvvvvvvvv  Amiga  vvvvvvvvvvvvvvvvvvvv

$(CLASSLIB) :	$(ALL_LIB_OBJ) clsCtln.ord
    -@type $(OBJSIZE)
    lb $(CLASSLIB) -f clsCtln.ord

clsCtln.ord :	Makefile
    ls >t:uu $(ALL_LIB_OBJ)
    ord t:uu clsCtln.ord


# vvvvvvvvvvvvvvvv  UN*X  vvvvvvvvvvvvvvvvvvvvv
#U
#U $(CLASSLIB) :	$(ALL_LIB_OBJ) clsCtln.ord
#U 	ar -cr $(CLASSLIB) `cat clsCtln.ord`
#U 
#U clsCtln.ord :	Makefile
#U 	lorder $(ALL_LIB_OBJ) | tsort >clsCtln.ord
#U 
 
# -----------------  common code  ------------------

subs :	$(ALL_LIB_OBJ)


# ----------------------------------
#	The workers of reality:
#
main16 :	$(ALL_LIB_OBJ) user$(OE) ww$(OE)
    $(LN) $(LDFLAGS) -o main16 user$(OE) ww$(OE) $(ALL_LIB_OBJ) $(LIBS) $(CLIB)
    echo >$(OBJSIZE) "16 bit ints"

ww$(OE) :	ww.c
    $(CC) $(CFLAGS) ww.c

ww.c :	Makefile
    prelink ww.c $(ALL_METHODS) user.m

user$(OE) :	user.c
user.c :	user.m


# ----------------------------------
#	Statement of Relations for Class Files:
#
Array$(OE) :	Array.c
Collection$(OE) :	Collection.c
IdArray$(OE) : 	IdArray.c
Object$(OE) : 	Object.c
OrdCltn$(OE) :	OrdCltn.c
List$(OE) :	List.c
Sequence$(OE) :	Sequence.c
Set$(OE) : 	Set.c



# ----------------------------------
#	Super Dependencies:
#
OrdCltn.c : 	OrdCltn.m $(OOCINC)objc/objc-runtime.h
Array.c :	Array.m $(OOCINC)objc/objc-runtime.h
Set.c	:	Set.m $(OOCINC)objc/objc-runtime.h

Object.c : 	Object.m $(HEADERS) $(OOCINC)objc/release.h \
        	 $(OOCINC)objc/substrate.h


# ----------------------------------
#	TeX document:
#
$(TEXFILE) :
    /makedoc/makedoc -C classfile -o $(TEXFILE) $(ALL_LIB_METHODS)
    @echo "Type  TeX $(TEXFILE)  to format this stuff! (use LaTeX)"


