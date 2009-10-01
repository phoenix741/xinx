@echo off
PATH=..\framework\;..\components\;..\ext\qcodeedit;%PATH%
rem Test au format XML
.\testxinxcodeedit.exe -xml > unittest.xml
.\testcontentview.exe -xml > unittest2.xml
.\testsnipets.exe -xml > unittest3.xml
.\testutils.exe -xml > unittest4.xml
rem Test pour le log
.\testxinxcodeedit.exe
.\testcontentview.exe
.\testsnipets.exe
.\testutils.exe
