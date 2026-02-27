# $Header: /usr/sirius/greg/src/makedoc/RCS/Makefile,v 3.2 89/10/10 11:40:49 greg Exp Locker: greg $
#
# $Log:	Makefile,v $
# Revision 3.2  89/10/10  11:40:49  greg
# > Output now goes to .tex file.
# -o and -c ala cc now added.
# -c becomes -C.
# 
# -m becomes -M.
# 
# Revision 3.1  89/10/04  10:15:55  greg
# Print out method index.  This scheme doesn't work.
# 
# Revision 1.1  89/09/29  08:41:12  greg
# Initial revision
# 
# Initial revision
# 
#
#
# Variable definitions:
CC=
LN=ln
LINT=		/usr/bin/lint
LPRDEST=	
CATMANDIR=	/tmp
BINDIR=		sys:bins/bin
MANDIR	=	sys:usr/man

# Compile time options.  Use USG for Suns.
#S OPTIONS=  -DUSG
OPTIONS= -DUSG +CDL +Q

# CFLAGS, use optimizer + compile time options
#S CFLAGS= -O $(OPTIONS)
CFLAGS= $(OPTIONS) -n +m

# Link flags shareable text and strip image
#S LDFLAGS= -s
LDFLAGS= -g
LIBS= -lgnul32 -lcl32

# Lint flags
LINT_FLAGS= -Dlint

# CFLAGS in debug mode, tell program we are in debug mode,
#  and use compile time options
DEBUG_CFLAGS= -DDEBUG $(OPTIONS) -g

# LDFLAGS in debug mode, do not strip image
DEBUG_LDFLAGS= -g

SRCS=		makedoc.c parse.c printclass.c printmethod.c
OBJS=		makedoc.o parse.o printclass.o printmethod.o
DOCS=		makedoc.1
MAN=		makedoc.man

PACK=		makedoc.1.z

# ----------------------------------------------

.SUFFIXES:	.1.z .man .1

.1.man:
		nroff -man $*.1 > $*.man

.man.1.z:
		cp $*.man $*XXX
		pack -f $*XXX
		mv $*XXX.z $*.1.z

# ----------------------------------------------

#S all:		makedoc pack
all:		makedoc

# ----------------------------------------------

makedoc:	$(OBJS) Makefile
		$(LN) $(OBJS) $(LDFLAGS) -o makedoc $(LIBS)

makedoc.o:	makedoc.c makedoc.h

printclass.o:	printclass.c makedoc.h

printmethod.o:	printmethod.c makedoc.h

debug:		Makefile
		exec make "CFLAGS=$(DEBUG_CFLAGS)" "LDFLAGS=$(DEBUG_LDFLAGS)" makedoc

lint:
		$(LINT) $(LINT_FLAGS) $(OPTIONS) $(SRCS)

clean:
		rm -f $(OBJS) makedoc $(MAN) $(PACK) print

archive:
		shar Makefile $(SRCS) > makedoc.src

man:		$(MAN)

print:		$(MAN)
		lpr $(LPRDEST) $?
		touch print

pack:		man $(PACK)

install:	all
		cpset makedoc $(BINDIR) 755 bin bin
		cpset makedoc.1.z $(CATMANDIR)/man1 444 bin bin
		cpset makedoc.1 $(MANDIR)/man1 444 bin bin

