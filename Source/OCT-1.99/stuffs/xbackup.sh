;  backup file for OOC system and projects  (July 89)
;	(0.06, 30-sept-89 bjw)
;	(0.07, 25-jan-90 bjw)	Runs under Shell or CLI.
;	(0.08, 16-jan-91 bjw)
;	(0.09, 21-april-91 bjw)  Cleanup..
echo "(Backup 0.09)"
FAILAT 21

cd ooc:xlator

delete >nil: #?.bak quiet
sys:bins/bin/xcopy -u *.l   df1:ooc/xlator
sys:bins/bin/xcopy -u *.c   df1:ooc/xlator
sys:bins/bin/xcopy -u *.h   df1:ooc/xlator
sys:bins/bin/xcopy -u Makefile  df1:ooc/xlator
sys:bins/bin/xcopy -u backup.sh  df1:ooc/xlator

if exists ooc:tools/Makefile
   sys:bins/bin/xcopy -u ooc:tools/*.c         df1:ooc/tools
   sys:bins/bin/xcopy -u ooc:tools/*.h         df1:ooc/tools
   sys:bins/bin/xcopy -u ooc:tools/Makefile    df1:ooc/tools
endif

echo "---- Done with translator ----"

delete quiet ooc:Maker/#?.c ooc:Maker/#?.bak
sys:bins/bin/xcopy -u ooc:Maker/*.m         df1:ooc/Maker
sys:bins/bin/xcopy -u ooc:Maker/*.h         df1:ooc/Maker
sys:bins/bin/xcopy -u ooc:Maker/Make*       df1:ooc/Maker

delete quiet ooc:amiga/#?.c ooc:amiga/#?.bak
sys:bins/bin/xcopy -u ooc:amiga/*.m         df1:ooc/amiga
sys:bins/bin/xcopy -u ooc:amiga/*.h         df1:ooc/amiga
sys:bins/bin/xcopy -u ooc:amiga/Make*       df1:ooc/amiga

delete quiet ooc:include/objc/#?.bak ooc:include/apps/#?.bak
sys:bins/bin/xcopy -u LIB:oc*.lib           df1:c
sys:bins/bin/xcopy -u ooc:include/*.h       df1:ooc/include
sys:bins/bin/xcopy -u ooc:include/sample.m  df1:ooc/include
sys:bins/bin/xcopy -u ooc:include/mustinit.ooc df1:ooc/include
sys:bins/bin/xcopy -u ooc:include/objc      df1:ooc/include/objc
sys:bins/bin/xcopy -u ooc:include/apps      df1:ooc/include/apps
sys:bins/bin/xcopy -u ooc:include/collect   df1:ooc/include/collect

delete >nil: ooc:methods/#?.bak quiet
sys:bins/bin/xcopy -u ooc:methods/*.m       df1:ooc/methods
sys:bins/bin/xcopy -u ooc:methods/sub*.c    df1:ooc/methods
sys:bins/bin/xcopy -u ooc:methods/Makefile  df1:ooc/methods

delete >nil: ooc:collect/#?.bak quiet
sys:bins/bin/xcopy -u ooc:collect/*.m       df1:ooc/collect
sys:bins/bin/xcopy -u ooc:collect/Makefile  df1:ooc/collect

delete >nil: ooc:docs/#?.bak quiet
sys:bins/bin/xcopy -u ooc:docs/*.ri         df1:ooc/docs
sys:bins/bin/xcopy -u ooc:docs/*.rinc       df1:ooc/docs
sys:bins/bin/xcopy -u ooc:docs/Makefile     df1:ooc/docs

rename df1:LastBackup to df1:PrevBackup
date >df1:LastBackup

echo "---- Backup finished ----"
;  backup file for OOC system and projects
