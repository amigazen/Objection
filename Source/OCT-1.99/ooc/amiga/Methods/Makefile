############################
#	:bk=0
#	
#	Makefile for class files in the Amiga Specials.
#		make  --  produce classes and substrate.
#		make clean  -- delete all intermediary files.
#		make lib    -- produce class library.
#		make tex	-- create .tex document (process with LaTeX).
#
#   Use LDFLAGS for linking, not LFLAGS (that's for LEX)
#       June 92,        bjw
#
# -------------------------------------

CC = cc
LN = ln
DELETE = delete >nil: quiet
MODEL =

OOCINC = ooc:include/
OOCBIN = sys:bins/
OOCLIB = LIB:

#	Some actual include files:
TREEINC = sys:usr/include/tree.h
RELEASE = $(OOCINC)objc/release.h

# -------------------------------------

AMILIB   = $(OOCLIB)ocexec.lib
HEADERS  = $(OOCINC)objc/objc.h
OCT = oct
OCFLAG = -w2
#

#	Store the .tex output somewhere:
TEXFILE = all.tex

OBJSIZE = osize.o

EXEC  = MsgPort.o
CEXEC = MsgPort.c
MEXEC = MsgPort.m

#	Substrate functions written in "C":
#		The order here is VERY important!
# SUBS = subs.o subio.o submem.o subbind.o submsg.o
# CSUBS = subio.c main.c
# MSUBS = subio.m main.m
SUBS  = EList.o ENode.o
CSUBS = EList.c ENode.c
MSUBS = EList.m ENode.m


#  All named methods:
ALL_LIB_METHODS = $(MEXEC)
ALL_METHODS = main.m $(MEXEC) $(MSUBS)


#	All modules in class library:
ALL_LIB_OBJ = $(EXEC) $(SUBS)


CFLAGS = $(MODEL) -n -DNO_PROTOTYPE
LDFLAGS = -g
CLIB   = -lc
LIBS = -locbase -locsubs


# ----------------------------------
#	Rules from the Roundtable:
#
.m.c :
	$(OCT) $(OCFLAG) $*.m


# ----------------------------------
#	The beginning has order:
#
all :	main16

install :	main16 lib

lib :	$(AMILIB)

tex :	$(TEXFILE)

clean :
	-$(DELETE) main16 main.c
	-$(DELETE) $(CSUBS) $(CEXEC)
	-$(DELETE) #?.tex #?.o #?.bak #?.sym #?.lst

help :
	@echo "Makefile for OCT substrate and foundation classes."
	@echo " "
	@echo "make all    -- Build test of library."
	@echo "make zoo    -- Build library and .zoo backup file."
	@echo "make lib    -- Build and install substrate library."
	@echo "make clean  -- Erase all re-generatable files."


# ----------------------------------
#	Create the foundation library:
#	The Object library is ordered, the substrate library is
#	already ordered correctly.
#
$(AMILIB) :	amiObj.ord $(ALL_LIB_OBJ) 
	-@type $(OBJSIZE)
	lb $(AMILIB) $(ALL_LIB_OBJ)

amiObj.ord :	makefile
	ls >t:uu $(ALL_LIB_OBJ)
	ord t:uu amiObj.ord

subs :	$(ALL_LIB_OBJ) $(SUBS)


# ----------------------------------
#	The workers of reality:
#
main16 :	$(SUBS) $(ALL_LIB_OBJ) main.o ww.o
	$(LN) $(LDFLAGS) -o main16 main.o ww.o $(ALL_LIB_OBJ) $(LIBS) $(CLIB)
	echo >$(OBJSIZE) "16 bit ints"

ww.o :	ww.c
	$(CC) $(CFLAGS) ww.c

ww.c :	makefile
	prelink ww.c $(ALL_METHODS)

main.o :	main.c
main.c :	main.m


# ----------------------------------
#	Statement of Relations for Class Files:
#
MsgPort.o :	MsgPort.c

EList.o :	EList.c
ENode.o :	ENode.c


# ----------------------------------
#	Super Dependencies:
#
MsgPort.c : 	MsgPort.m $(OOCINC)objc/objc-runtime.h


# ----------------------------------
#	TeX document:
#
$(TEXFILE) :
	/makedoc/makedoc -C classfile -o $(TEXFILE) $(ALL_LIB_METHODS)
	@eho "Type  TeX $(TEXFILE)  to format this stuff! (use LaTeX)"


