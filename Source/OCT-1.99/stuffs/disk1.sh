.key startplace
;	build second release diskette for OOC system at "OCTDISK:"
;	HOLDS SAMPLES AND DOCS (Part 2)
.bra {
.ket }

;  Also fill the volume "OCTDISK:"
FAILAT 40
assign OCTDISK: exists
if warn
   date >t:disk1-undo-assign
   assign OCTDISK: df1:
   echo using df1 as default.
endif
FAILAT 6


if "{startplace}" EQ "samples"
   echo "Skipping to samples..."
   skip  do_samples
endif

FAILAT 21
echo  "Part 2 -- Samples and Docs"
makedir OCTDISK:samples
makedir OCTDISK:samples/view
makedir OCTDISK:samples/maker
makedir OCTDISK:samples/xtest
makedir OCTDISK:ooc/docs
makedir OCTDISK:ooc/docs/misc

date >OCTDISK:samples/zoodummy


;	First, label the diskette
copy ooc:include/objc/RELEASE.h to OCTDISK:RELEASE
FAILAT 1

echo  "Force the Update."
FAILAT 30		; DBG DBG DBG


; ------------------------------------------------------
;	:OOC/DOCS

cd ooc:docs
delete #?.bak
delete others/#?.bak
zoo -add OCTDISK:ooc/docs/ZDocs *.ri *.ro README
cd misc
zoo -add OCTDISK:ooc/docs/misc/ZMisc * netnotes/*
cd /others
zoo -add OCTDISK:ooc/docs/misc/ZMisc *

copy zoos OCTDISK:ooc/docs clone 	 ; add any already-zoo files


; ------------------------------------------------------
;	:SAMPLES/VIEW

LAB do_samples
echo "..Samples.."

cd ooc:view
delete #?.bak
;* copy #?.m       to OCTDISK:samples/view dates
;* copy #?.h       to OCTDISK:samples/view dates
;* copy Makefile   to OCTDISK:samples/view dates
zoo -add OCTDISK:samples/view/Zview.zoo *.m *.h Makefile BMFile ReadMe


; ------------------------------------------------------
;	:SAMPLES/MAKER

cd ooc:maker
delete #?.bak
echo "Please make sure the 'maker' is up to date"

 ;- copy #?.m       to OCTDISK:samples/maker dates
 ;- copy #?.h       to OCTDISK:samples/maker dates
 ;- copy main32     to OCTDISK:samples/maker dates
 ;- copy main16     to OCTDISK:samples/maker dates
 ;- copy hw16       to OCTDISK:samples/maker dates
 ;- copy hw32       to OCTDISK:samples/maker dates
 ;- copy make#?     to OCTDISK:samples/maker dates
 ;- copy depends    to OCTDISK:samples/maker dates
zoo -add OCTDISK:samples/maker/Zsamples.zoo *.m *.h *16 *32 make* README *.mac depends *.speed

; ------------------------------------------------------
;	:SAMPLES/TESTOUT

cd ooc:test
delete #?.bak #?.lst #?.c

 ;- copy #?.m               OCTDISK:samples/xtest  dates
 ;- copy #?.h               OCTDISK:samples/xtest  dates
 ;- copy Makefile           OCTDISK:samples/xtest  dates
zoo -add OCTDISK:samples/xtest/ZTest.zoo *.m *.h Makefile README *.mac



; ------------------------------------------------------
;  cleanup the temp assign only if we made it:
if EXISTS t:disk1-undo-assign
   delete T:disk1-undo-assign
   assign OCTDISK:
endif


;	builds just documentation and samples for OCT system (Oct 93)
;	HOLDS DOCS AND SAMPLES.

