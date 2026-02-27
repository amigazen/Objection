############################
#	:bk=0
#
#		Makefile for OO Translator's Tools
#
#

OOCBIN = sys:bins/bin/
OOCINC = ooc:include/
DELETE = rm
LINKER = ln
CLONE  = copy clone 

CFLAGS = -n +x3,5
CLIBS  = -lc

OE = .o
EE = 

WILD_ALL = #?
WILD_BAK = $(WILD_ALL).bak
WILD_OBJ = $(WILD_ALL)$(OE)

#
# ----------------------
#

# MAINS = mkif buildmf
MAINS = mkif
MIOBJ = mkif.o
BMFOBJ = buildmf.o


# ----------------------
#
all :	$(MAINS)

install :	$(MAINS)
	$(CLONE) mkif      $(OOCBIN)mkif
	$(CLONE) buildmf   $(OOCBIN)buildmf

clean :
	-$(DELETE) $(MAINS) $(WILD_OBJ) $(WILD_BAK)

#
# ----------------------
#
mkif :	$(MIOBJ)
	$(LINKER) -o mkif $(MIOBJ) $(CLIBS)

mkif.o :	$(OOCINC)objc/release.h



# ----------------------
#
buildmf :	$(BMFOBJ)
	$(LINKER) -o buildmf $(BMFOBJ) $(CLIBS)

buildmf.o :	$(OOCINC)objc/release.h


