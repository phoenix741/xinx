@echo off
PATH=..\framework\;..\components\;..\ext\qcodeedit;%PATH%
rem Test au format XML
.\bin\testxinxcodeedit.exe -xml > unittest.xml
.\bin\testcontentview.exe -xml > unittest2.xml
.\bin\testsnipets.exe -xml > unittest3.xml
.\bin\testutils.exe -xml > unittest4.xml
.\bin\testcontentview2.exe -xml > unittest5.xml
rem Test pour le log
.\bin\testxinxcodeedit.exe
.\bin\testcontentview.exe
.\bin\testsnipets.exe
.\bin\testutils.exe
.\bin\testcontentview2.exe