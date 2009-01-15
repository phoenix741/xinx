@echo off
PATH=..\libxinx\;..\components\;..\ext\qcodeedit;%PATH%
rem Test au format XML
.\testxinxcodeedit\textxinxcodeedit.exe -xml > unittest.xml
rem Test pour le log
.\testxinxcodeedit\textxinxcodeedit.exe
