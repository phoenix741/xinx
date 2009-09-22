@echo off
PATH=..\framework\;..\components\;..\ext\qcodeedit;%PATH%
rem Test au format XML
.\testxinxcodeedit\testxinxcodeedit.exe -xml > unittest.xml
.\testcontentview\testcontentview.exe -xml > unittest2.xml
.\testsnipets\testsnipets.exe -xml > unittest3.xml
rem Test pour le log
.\testxinxcodeedit\testxinxcodeedit.exe
.\testcontentview\testcontentview.exe
.\testsnipets\testsnipets.exe
