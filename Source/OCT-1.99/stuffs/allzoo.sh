.key startplace
;	Creates two big zoo files!
;	HOLDS XLATOR SOURCE, METHODS, TOOLS
.bra {
.ket }
echo "xxx"

;  Fill the volume "OCTDISK:"
FAILAT 40
delete quiet T:whole.zo? T:disk?-undo-assign T:amibin.zo?

assign OCTDISK: exists
if warn
   assign OCTDISK: df1:
   assign OCTDISK: exists   	; Let user see what we did.
   date >t:allzoo-undo-assign
endif



FAILAT 21
echo  "Complete Disk: Xlator, Methods, Tools"
makedir OCTDISK:unix


;	First, label the diskette
copy ooc:include/objc/RELEASE.h to OCTDISK:RELEASE
FAILAT 1

echo  "Force the Update."
FAILAT 30		; DBG DBG DBG


; ------------------------------------------------------
;	:OOC/DOCS
;	(COPY A .ZOO FILE)

cd ooc:docs
delete #?.bak
delete others/#?.bak

cd ooc:
zoo a T:whole docs/*.ro docs/others/* docs/README
;; copy docs/misc/zoos OCTDISK:docs/misc  clone  ; add any already-zoo files


; ------------------------------------------------------
;	:OOC/INCLUDE
echo "..Include.."
cd ooc:
delete include/objc/#?.bak include/objc/private/#?.bak

zoo a T:whole include/objc/*.h include/objc/README include/objc/private/*
zoo a T:whole include/collect/*.h include/collect/README
zoo a T:whole include/apps/*.h include/apps/README
zoo a T:whole include/*.h include/*.m include/mustinit.ooc include/sample.m include/README

;	Now add our own includes	;
makedir T:usr
makedir T:usr/include

cd  DH4:usr/include
copy stdlib.h T:usr/include/stdlib.h-a   clone
copy string.h T:usr/include/string.h-a   clone
copy tree.h   T:usr/include/tree.h       clone
copy ooc:S/README.inc T:usr/include/README   clone
copy s:flex#?  t:usr/include             clone

cd T:
zoo a T:whole usr/include/* usr/include/README
delete T:usr all quiet


; ------------------------------------------------------
;	:SOURCE/TOOLS

LAB  do_tools
echo "..Tools.."

cd ooc:tools
delete quiet #?.bak
make install

cd ooc:
zoo a T:whole tools/*.c tools/*.h tools/Makefile tools/README

;	-- UN*X things..
;	(COPY A .ZOO FILE)
copy ooc:unix/sunmake.tar.Z  OCTDISK:unix  clone
copy ooc:unix/Umakes.zoo    OCTDISK:unix  clone
copy ooc:unix/gears.zoo     OCTDISK:unix  clone
copy ooc:unix/README        OCTDISK:unix  clone


; ------------------------------------------------------
;	:SOURCE/METHODS
;		Always get the latest version!

LAB do_methods
echo "..Methods.."
FAILAT 5
cd ooc:methods
make lib
echo "If the library isn't up to data, restart as 'methods'"

cd ooc:
zoo a T:whole methods/*.m methods/sub*.c methods/Make* methods/README methods/*.h
zoo a T:amibin methods/main16 methods/main32 methods/*.mac

cd ooc:amiga
make -n lib
cd ooc:
zoo a T:whole amiga/*.m amiga/Make* amiga/README
zoo a T:amibin amiga/main16 amiga/main32 amiga/*.mac

cd ooc:collect
make -n lib
cd ooc:
zoo a T:whole collect/*.m collect/Make* collect/README
zoo a T:amibin collect/main16 collect/main32 collect/*.mac

cd LIB:
cd /
;; zoo a T:whole lib/ocbas*.lib lib/ocsub*.lib lib/ocexe*.lib lib/ocCltn*.lib
;  -- just store 16-bit int versions..
zoo a T:amibin lib/ocbase.lib lib/ocsubs.lib lib/ocexec.lib lib/ocCltn.lib



; ------------------------------------------------------
;	:SOURCE/TRANSLATOR
;	(Trying to cut down on space)

LAB  do_xlat
echo "..translator.."

cd ooc:xlator
delete #?.bak
make install

;**  Zoo compresses about 51%
cd ooc:
zoo a T:whole  xlator/*.c xlator/scan.l xlator/*.h xlator/Makefile xlator/oct.mac xlator/README

; ------------------------------------------------------
;	:SOURCE/CPP
;	(They whole cpp program is zoo'ed)

LAB  do_cpp

cd ooc:
zoo a T:whole xlator/ccpp/cpp*.c xlator/ccpp/*h xlator/ccpp/Make*
zoo a T:whole xlator/ccpp/Contents xlator/ccpp/README xlator/ccpp/Changes
zoo a T:whole xlator/ccpp/cpp.rno xlator/ccpp/cpp.mem xlator/ccpp/readme.txt


; ------------------------------------------------------
;	:SAMPLES/VIEW

LAB do_samples
echo "..Samples.."

;--* cd ooc:
;--* delete view/#?.bak
;--* zoo a T:whole view/*.m view/*.h view/Makefile view/BMFile view/ReadMe
;--* zoo a T:amibin view/main16


; ------------------------------------------------------
;	:SAMPLES/MAKER

cd ooc:maker
delete #?.bak
echo "Please make sure the 'maker' is up to date"

cd ooc:
zoo a T:whole  maker/*.m maker/*.h maker/Make* maker/README maker/depends
zoo a T:amibin maker/*16 make/*32 make/*.mac

; ------------------------------------------------------
;	:SAMPLES/TESTOUT

cd ooc:test
delete #?.bak #?.lst #?.c #?.o

cd ooc:
zoo a T:whole  test/*.m test/*.h test/Makefile test/README test/*.mac


; ------------------------------------------------------
;	:BIN

LAB  do_bins
echo "Bin directory.."
cd ooc:xlator
make install

cd sys:bins

; 	These executables can be remade by the user and deleted if
;	user needs more space.
zoo a T:amibin  bin/oct     bin/prelink bin/mkif
zoo a T:amibin  bin/oct-cpp bin/flex   	; special programs
;* zoo a T:whole   bin/oct-cpp bin/flex   	; special programs

; ------------------------------------------------------
;	:	(root)
;

makedir T:attop
makedir T:attop/s
cd T:attop
zoo -extract ooc:S/at-root
copy ooc:S/ReadMe.1st    T:attop    clone
copy ooc:S/Overview.txt  T:attop    clone
copy ooc:S/changes       T:attop	clone
copy ooc:Make.unix       T:attop    clone
copy ooc:S/#?.sh         T:attop/s  clone
copy s:Flex#?            T:attop/s  clone

zoo a T:whole * s/*

cd ooc:
delete all quiet T:attop


; ------------------------------------------------------
;	Update destination with the zoo file!

;  cleanup the temp assign only if we made it:
if EXISTS t:allzoo-undo-assign
   copy T:whole.zoo to OCTDISK: clone
   copy T:amibin.zoo to OCTDISK: clone
   delete T:allzoo-undo-assign
   assign OCTDISK:
else
   list t:(whole|amibin).zoo
   echo "Zoo files left in T:"
endif

; =================================================================
;	build entire release diskette for OCT system (July 3, 1992)
;	HOLDS XLATOR SRC, METHODS, TOOLS (disk #0)
;   Moves across flex include files.
;	Copys objc/private and methods/*.h
;
;	To create only in RAM:, set octdisk: to t:
;
