# Auto-generated by EclipseNSIS Script Wizard
# 14 avr. 2009 11:19:10

Name XINX
SetCompressor lzma

# General Symbol Definitions
!define VERSION 0.8.1.0
!define COMPANY Shadoware.Org
!define URL http://xinx.shadoware.org
!define REGKEY "SOFTWARE\${COMPANY}\$(^Name)"

# MUI Symbol Definitions
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install-colorful.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_LICENSEPAGE_CHECKBOX
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER XINX
!define MUI_FINISHPAGE_SHOWREADME $INSTDIR\CHANGELOG

# MUI Symbol Definition for uninstall
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall-colorful.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# Included files
!include Sections.nsh
!include MUI2.nsh

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE ..\COPYING
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

# Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE French
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile xinx-${VERSION}.exe
CRCCheck on
XPStyle on
InstallDir $PROGRAMFILES\${COMPANY}\XINX
ShowInstDetails show
VIProductVersion ${VERSION}
VIAddVersionKey /LANG=${LANG_FRENCH} ProductName XINX
VIAddVersionKey /LANG=${LANG_FRENCH} ProductVersion "${VERSION}"
VIAddVersionKey /LANG=${LANG_FRENCH} CompanyName "${COMPANY}"
VIAddVersionKey /LANG=${LANG_FRENCH} CompanyWebsite "${URL}"
VIAddVersionKey /LANG=${LANG_FRENCH} FileVersion "${VERSION}"
VIAddVersionKey /LANG=${LANG_FRENCH} FileDescription "Installation of XINX"
VIAddVersionKey /LANG=${LANG_FRENCH} LegalCopyright "2009 (c) Ulrich VANDENHEKKE"
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show

InstType "full"
InstType "minimal"

# Installer sections
SectionGroup Xinx GRP_XNX
	Section Application SEC_XNX_APP
		SectionIn 1 2 RO
	    SetOutPath $INSTDIR\bin
	    SetOverwrite on
	    File ..\xinx\xinx.exe
	    File ..\xinxprojectwizard\xinxprojectwizard.exe
	    SetOutPath $INSTDIR
	    File ..\CHANGELOG
	    File ..\COPYING
	    File ..\INSTALL
		WriteRegStr HKLM "${REGKEY}\Components" Application 1
	SectionEnd
	SectionGroup Librairies
		Section "Librairies Applicatif" SEC_XNX_LIB
			SectionIn 1 2 RO
		    SetOutPath $INSTDIR\bin
		    SetOverwrite on
		    File ..\libxinx\sharedxinx.dll
		    File ..\components\xinxcmp.dll
		    File ..\ext\qcodeedit\qcodeedit.dll
			WriteRegStr HKLM "${REGKEY}\Components" "Librairies Applicatif" 1
		SectionEnd
		Section Qt SEC_XNX_QT
			SectionIn 1 2 RO
		    SetOutPath $INSTDIR\bin
		    SetOverwrite on
		    File C:\Qt\4.5.0\bin\QtCore4.dll
		    File C:\Qt\4.5.0\bin\QtDBus4.dll
		    File C:\Qt\4.5.0\bin\QtGui4.dll
		    File C:\Qt\4.5.0\bin\QtNetwork4.dll
		    File C:\Qt\4.5.0\bin\QtScript4.dll
		    File C:\Qt\4.5.0\bin\QtSql4.dll
		    File C:\Qt\4.5.0\bin\QtXml4.dll
		    File C:\Qt\4.5.0\bin\QtXmlPatterns4.dll
			WriteRegStr HKLM "${REGKEY}\Components" Qt 1
		SectionEnd
		Section MinGW SEC_XNX_MINGW
			SectionIn 1 2 RO
		    SetOutPath $INSTDIR\bin
		    SetOverwrite on
			File C:\Qt\4.5.0\bin\mingwm10.dll
			WriteRegStr HKLM "${REGKEY}\Components" MinGW  1
		SectionEnd
	SectionGroupEnd
	SectionGroup Plugins
		Section "CVS Plugin" SEC_XNX_PLUGINCVS
			SectionIn 1 2 RO
		    SetOutPath $INSTDIR\plugins
		    SetOverwrite on
		    File ..\plugins\cvsplugin.dll
			WriteRegStr HKLM "${REGKEY}\Components" "CVS Plugin" 1
		SectionEnd
		Section "Web Plugin" SEC_XNX_PLUGINWEB
			SectionIn 1 2 RO
			SetOutPath $INSTDIR\xml
			File ..\xinx\xml\baseplugin_js.xml
			File ..\xinx\xml\baseplugin_xml.xml
			File ..\xinx\xml\template.xml
			WriteRegStr HKLM "${REGKEY}\Components" "Web Plugin" 1
		SectionEnd
		Section "Scripts XQuery" SEC_XNX_SCRIPT
			SectionIn 1 2 RO
			SetOutPath $INSTDIR\xml
		    File ..\scripts\conf_businessviewlist.xq
		    File ..\scripts\conf_version.xq
			WriteRegStr HKLM "${REGKEY}\Components" "Scripts XQuery" 1
		SectionEnd
	SectionGroupEnd
SectionGroupEnd

SectionGroup Extra
	Section Scripts SEC_EXT_SCRIPT
		SectionIn 1
	    SetOutPath $INSTDIR\scripts
	    SetOverwrite on
	    File ..\scripts\changeFileName.js
	    File ..\scripts\exemple.js
	    File ..\scripts\rtrim.js
	    File ..\scripts\tab.js
	    File ..\scripts\test_script.html
	    File ..\scripts\xslkey.js
	    File ..\scripts\xslvalue.js
	    WriteRegStr HKLM "${REGKEY}\Components" Scripts 1
	SectionEnd
	Section "SVN Plugin" SEC_EXT_PLUGINSVN
		SectionIn 1
	    SetOutPath $INSTDIR\plugins
	    SetOverwrite on
	    File ..\plugins\svnplugin.dll
	    WriteRegStr HKLM "${REGKEY}\Components" "SVN Plugin" 1
	SectionEnd
	Section "WebServices Plugin" SEC_EXT_SERVICES
		SectionIn 1
	    SetOutPath $INSTDIR\plugins
	    SetOverwrite on
	    File ..\plugins\services.dll
	    WriteRegStr HKLM "${REGKEY}\Components" "WebServices Plugin" 1
	SectionEnd
SectionGroupEnd

SectionGroup /e Developpement GRP_DEV
	Section /o Source SEC_DEV_SRC
		SectionIn 1
		SetOutPath $INSTDIR
		SetOverwrite on
		File /oname=src.zip ..\xinx.zip
		WriteRegStr HKLM "${REGKEY}\Components" Source 1
	SectionEnd
	Section /o Documentation SEC_DEV_DOC
		SectionIn 1
		SetOutPath $INSTDIR\doc\html
		SetOverwrite on
		File ..\doc\html\*.*
		WriteRegStr HKLM "${REGKEY}\Components" Documentation 1
	SectionEnd
SectionGroupEnd

Section -post SEC0006
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath $SMPROGRAMS\$StartMenuGroup
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk" $INSTDIR\uninstall.exe
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

# Uninstaller sections
Section /o "-un.WebServices Plugin" UNSEC0005
    Delete /REBOOTOK $INSTDIR\plugins\services.dll
    DeleteRegValue HKLM "${REGKEY}\Components" "WebServices Plugin"
SectionEnd

Section /o "-un.SVN Plugin" UNSEC0004
    Delete /REBOOTOK $INSTDIR\plugins\svnplugin.dll
    DeleteRegValue HKLM "${REGKEY}\Components" "SVN Plugin"
SectionEnd

Section /o "-un.CVS Plugin" UNSEC0003
    Delete /REBOOTOK $INSTDIR\plugins\cvsplugin.dll
    DeleteRegValue HKLM "${REGKEY}\Components" "CVS Plugin"
SectionEnd

Section /o -un.Scripts UNSEC0002
    Delete /REBOOTOK $INSTDIR\scripts\xslvalue.js
    Delete /REBOOTOK $INSTDIR\scripts\xslkey.js
    Delete /REBOOTOK $INSTDIR\scripts\test_script.html
    Delete /REBOOTOK $INSTDIR\scripts\tab.js
    Delete /REBOOTOK $INSTDIR\scripts\rtrim.js
    Delete /REBOOTOK $INSTDIR\scripts\exemple.js
    Delete /REBOOTOK $INSTDIR\scripts\changeFileName.js
    DeleteRegValue HKLM "${REGKEY}\Components" Scripts
SectionEnd

Section /o -un.Documentation UNSEC0001
    Delete /REBOOTOK $INSTDIR\doc\html\*.*
    DeleteRegValue HKLM "${REGKEY}\Components" Documentation
SectionEnd

Section /o -un.Main UNSEC0000
    Delete /REBOOTOK $INSTDIR\INSTALL
    Delete /REBOOTOK $INSTDIR\COPYING
    Delete /REBOOTOK $INSTDIR\CHANGELOG
    Delete /REBOOTOK $INSTDIR\xmlk\template.xml
    Delete /REBOOTOK $INSTDIR\xmlk\baseplugin_xml.xml
    Delete /REBOOTOK $INSTDIR\xmlk\baseplugin_js.xml
    Delete /REBOOTOK $INSTDIR\scripts\conf_version.xq
    Delete /REBOOTOK $INSTDIR\scripts\conf_businessviewlist.xq
    Delete /REBOOTOK $INSTDIR\bin\mingwm10.dll
    Delete /REBOOTOK $INSTDIR\bin\QtXmlPatterns4.dll
    Delete /REBOOTOK $INSTDIR\bin\QtXml4.dll
    Delete /REBOOTOK $INSTDIR\bin\QtSql4.dll
    Delete /REBOOTOK $INSTDIR\bin\QtScript4.dll
    Delete /REBOOTOK $INSTDIR\bin\QtNetwork4.dll
    Delete /REBOOTOK $INSTDIR\bin\QtGui4.dll
    Delete /REBOOTOK $INSTDIR\bin\QtDBus4.dll
    Delete /REBOOTOK $INSTDIR\bin\QtCore4.dll
    Delete /REBOOTOK $INSTDIR\bin\qcodeedit.dll
    Delete /REBOOTOK $INSTDIR\bin\xinxprojectwizard.exe
    Delete /REBOOTOK $INSTDIR\xinxcmp.dll
    Delete /REBOOTOK $INSTDIR\bin\sharedxinx.dll
    Delete /REBOOTOK $INSTDIR\bin\xinx.exe
    DeleteRegValue HKLM "${REGKEY}\Components" Main
SectionEnd

Section -un.post UNSEC0006
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk"
    Delete /REBOOTOK $INSTDIR\uninstall.exe
    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    RmDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RmDir /REBOOTOK $INSTDIR
SectionEnd

# Installer functions
Function .onInit
    InitPluginsDir
FunctionEnd

# Uninstaller functions
Function un.onInit
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_UNSECTION Main ${UNSEC0000}
    !insertmacro SELECT_UNSECTION Documentation ${UNSEC0001}
    !insertmacro SELECT_UNSECTION Scripts ${UNSEC0002}
    !insertmacro SELECT_UNSECTION "CVS Plugin" ${UNSEC0003}
    !insertmacro SELECT_UNSECTION "SVN Plugin" ${UNSEC0004}
    !insertmacro SELECT_UNSECTION "WebServices Plugin" ${UNSEC0005}
FunctionEnd

# Section Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DEV_SRC} $(SEC_DEV_SRC_DESC)
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DEV_DOC} $(SEC_DEV_DOC_DESC)
!insertmacro MUI_DESCRIPTION_TEXT ${SEC0002} $(SEC0002_DESC)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

# Installer Language Strings
# TODO Update the Language Strings with the appropriate translations.

LangString ^UninstallLink ${LANG_FRENCH} "Uninstall $(^Name)"
LangString ^UninstallLink ${LANG_ENGLISH} "Uninstall $(^Name)"

LangString SEC_DEV_SRC_DESC ${LANG_FRENCH} "Les sources de XINX"
LangString SEC_DEV_DOC_DESC ${LANG_FRENCH} "La documentation technique de XINX"
LangString SEC0002_DESC ${LANG_FRENCH} "Script utilitaire � XINX"

LangString SEC_DEV_SRC_DESC ${LANG_ENGLISH} "Source of XINX"
LangString SEC_DEV_DOC_DESC ${LANG_ENGLISH} "Technical documentation"
LangString SEC0002_DESC ${LANG_ENGLISH} "Script utilitaire � XINX"
