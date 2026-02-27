.key startplace
;	build release diskette for OOC system (complete disk) at "OCTDISK:"
;	HOLDS XLATOR SOURCE, METHODS, TOOLS
.bra {
.ket }

;  Also fill the volume "OCTDISK:"
FAILAT 40
assign OCTDISK: exists
if warn
   date >t:disk0-undo-assign
   assign OCTDISK: df1:
   echo using df1 as default.
endif
FAILAT 6

if "{startplace}" EQ "methods"
   echo "Skipping to methods..."
   skip  do_methods
endif

if "{startplace}" EQ "xlat"
   echo "Skipping to xlat..."
   skip  do_xlat
endif

if "{startplace}" EQ "tools"
	echo "Skipping to tools.."
	skip  do_tools
endif

if "{startplace}" EQ "cpp"
	echo "Skipping to cpp.."
	skip  do_cpp
endif

if "{startplace}" EQ "bins"
	echo "Skipping to bins.."
	skip  do_bins
endif


FAILAT 21
echo  "Complete Disk: Xlator, Methods, Tools"
makedir OCTDISK:bin
makedir OCTDISK:lib

makedir OCTDISK:ooc
makedir OCTDISK:ooc/methods
makedir OCTDISK:ooc/collect
makedir OCTDISK:ooc/amiga

makedir OCTDISK:include
makedir OCTDISK:include/objc
makedir OCTDISK:include/apps
makedir OCTDISK:include/exec
makedir OCTDISK:include/collect

makedir OCTDISK:src
makedir OCTDISK:src/cpp
makedir OCTDISK:src/tools
makedir OCTDISK:src/xlator
makedir OCTDISK:src/unix

makedir OCTDISK:T

;	Create a list of all the directories:
find >OCTDISK:lib/dirlist OCTDISK: -type d -print

date >OCTDISK:ooc/zoodummy
date >OCTDISK:include/zoodummy
date >OCTDISK:src/zoodummy


;	First, label the diskette
copy ooc:include/objc/RELEASE.h to OCTDISK:RELEASE
FAILAT 1

echo  "Force the Update."
FAILAT 30		; DBG DBG DBG


; ------------------------------------------------------
;	:OOC/INCLUDE
echo "..Include.."

cd ooc:include/objc
zoo -add OCTDISK:include/objc/Zobjc.zoo *.h README
 ;-  copy #?.h       to OCTDISK:include/objc dates

cd ooc:include/collect
zoo -add OCTDISK:include/collect/Zcollect.zoo *.h README
 ;-  copy #?.h       to OCTDISK:include/collect dates

cd ooc:include/apps
zoo -add OCTDISK:include/apps/Zapps.zoo *.h README
 ;-  copy #?.h       to OCTDISK:include/apps dates

cd ooc:include
zoo -add OCTDISK:include/Zinclude.zoo *.h mustinit.ooc sample.m README
 ;-  copy sample.m   to OCTDISK:include      dates

;	Now add our own includes	;
cd  DH4:usr/include
copy stdlib.h T:stdlib.h-a
copy string.h T:string.h-a
copy tree.h   T:tree.h
copy ooc:S/README.inc T:README.inc
cd T:
zoo -add OCTDISK:include/Zusr-inc.zoo stdlib.h-a string.h-a tree.h README.inc
 ;- copy stdlib.h   to OCTDISK:include dates
 ;- copy string.h   to OCTDISK:include dates
 ;- copy tree.h     to OCTDISK:include dates
cd ooc:S


; ------------------------------------------------------
;	:SOURCE/TOOLS

LAB  do_tools
echo "..Tools.."

cd ooc:tools
delete quiet #?.bak
make install

 ;- copy mkif.c     to OCTDISK:src/tools dates
 ;- copy Makefile   to OCTDISK:src/tools dates
zoo -add OCTDISK:src/tools/ZTools.zoo *.c *.h Makefile README *.mac

;	-- UN*X things..
copy ooc:unix/#?.zoo  to OCTDISK:src/unix  clone
copy ooc:unix/README  to OCTDISK:src/unix  clone


; ------------------------------------------------------
;	:SOURCE/METHODS
;		Always get the latest version!

LAB do_methods
echo "..Methods.."
FAILAT 5
cd ooc:methods
make lib
echo "If the library isn't up to data, restart as 'methods'"

 ;- copy #?.m       to OCTDISK:ooc/methods dates
 ;- copy sub#?.c    to OCTDISK:ooc/methods dates
 ;- copy make#?     to OCTDISK:ooc/methods dates
 ;- copy main16     to OCTDISK:ooc/methods dates
 ;- copy main32     to OCTDISK:ooc/methods dates
zoo -add OCTDISK:ooc/methods/ZMethods *.m sub*.c make* main16 main32 README *.mac

cd ooc:amiga
make -n lib
zoo -add OCTDISK:ooc/amiga/ZMethods *.m make* main16 main32 README *.mac

cd ooc:collect
make -n lib
zoo -add OCTDISK:ooc/collect/ZMethods *.m make* main16 main32 README *.mac

cd LIB:
 ;- copy ocbas#?.lib   to OCTDISK:include   dates
 ;- copy ocsub#?.lib   to OCTDISK:include   dates
 ;- copy ocexe#?.lib   to OCTDISK:include   dates
 ;- copy ocClt#?.lib   to OCTDISK:include   dates
zoo -add OCTDISK:lib/ZLib ocbas*.lib ocsub*.lib ocexe*.lib ocCltn*.lib



; ------------------------------------------------------
;	:SOURCE/CPP
;	(takes too much space if unpacks!)

LAB  do_cpp

cd ooc:xlator/ccpp
zoo -add OCTDISK:src/cpp/Zcpp.zoo *.c *.h *.mem *.rno Make* Changes Contents readme.txt
copy clone README to OCTDISK:src/cpp

; ------------------------------------------------------
;	:SOURCE/TRANSLATOR
;	(Trying to cut down on space)

LAB  do_xlat
echo "..translator.."

cd ooc:xlator
delete #?.bak
make install

;**  Zoo compresses about 51%
zoo -add t:xlat.zoo  *.c scan.l *.h Makefile oct.mac README
copy t:xlat.zoo  to OCTDISK:src/xlator/Zxlator.zoo dates
delete t:xlat.zoo

 ;- copy  #?.(C|L|H) to  OCTDISK:source/translator dates
 ;- copy  Makefile   to  OCTDISK:source/translator dates
 ;- copy  #?.mac     to  OCTDISK:source/translator dates

wait 2
; ------------------------------------------------------
;	:BIN

LAB  do_bins
echo "Bin directory.."
cd ooc:xlator
make install

cd sys:bins/bin

copy  oct       to  OCTDISK:bin  clone
copy  prelink   to  OCTDISK:bin  clone
copy  mkif      to  OCTDISK:bin  clone
copy  deroff    to  OCTDISK:bin  clone

copy  cpp       to  OCTDISK:bin  clone
copy  flex      to  OCTDISK:bin  clone


; ------------------------------------------------------
;	:	(root)
;

cd OCTDISK:
zoo -extract ooc:S/at-root

cd ooc:S
copy clone ReadMe        OCTDISK:ReadMe 

zoo -add OCTDISK:stuffs.zoo *.sh changes


; ------------------------------------------------------
;  cleanup the temp assign only if we made it:
if EXISTS t:disk0-undo-assign
   delete T:disk0-undo-assign
   assign OCTDISK:
endif


;	build first release diskette for OOC system (Sept 20, 1990)
;	Part 2 holds documentation and samples.
;	HOLDS XLATOR SRC, METHODS, TOOLS (disk #0)  (Oct 9, 1993)

