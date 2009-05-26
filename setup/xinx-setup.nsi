# Auto-generated by EclipseNSIS Script Wizard
# 14 avr. 2009 11:19:10

Name XINX
SetCompressor /SOLID lzma

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

# Installer sections
# Installer functions
Function .onInit
    InitPluginsDir

	ReadRegStr $0 HKLM SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{AB2A9C86-AD08-4373-98FD-6A9AA8496AEE}_is1 "QuietUninstallString"
	StrCmp $0 "" TestDBus 0
	MessageBox MB_OKCANCEL "$(UNINSTALL_OLD_XINX)" IDOK 0 IDCANCEL Annulation
	ExecWait $0
	Goto TestDBus
Annulation:
	Abort
TestDBus:
	ReadRegStr $0 HKLM SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{5561D91E-B126-4C8A-A2A1-E9C095528FD9}_is1 "QuietUninstallString"
	StrCmp $0 "" Install 0
	MessageBox MB_OKCANCEL "$(UNINSTALL_OLD_DBUS)" IDOK 0 IDCANCEL Annulation
	ExecWait $0
	Goto Install
Install:

FunctionEnd

Section Application SEC_XNX_APP
	SectionIn RO
	SetOutPath $INSTDIR
	SetOverwrite on
	File ..\CHANGELOG
	File ..\COPYING
	File ..\INSTALL

	SetOutPath $INSTDIR\bin
	SetOverwrite on
	File ..\xinx\xinx.exe
	File ..\xinxprojectwizard\xinxprojectwizard.exe

	File ..\libxinx\sharedxinx.dll
	File ..\components\xinxcmp.dll
	File ..\ext\qcodeedit\qcodeedit.dll

	File C:\Qt\4.5.1\bin\QtCore4.dll
	File C:\Qt\4.5.1\bin\QtGui4.dll
	File C:\Qt\4.5.1\bin\QtNetwork4.dll
	File C:\Qt\4.5.1\bin\QtScript4.dll
	File C:\Qt\4.5.1\bin\QtXml4.dll
	File C:\Qt\4.5.1\bin\QtXmlPatterns4.dll
	File C:\Qt\4.5.1\bin\QtWebKit4.dll

	File C:\Qt\4.5.1\bin\mingwm10.dll

	SetOutPath $INSTDIR\xml
	SetOverwrite on
	File ..\xml\baseplugin_js.xml
	File ..\xml\baseplugin_xml.xml
	File ..\xml\template.xml

	File ..\scripts\conf_businessviewlist.xq
	File ..\scripts\conf_version.xq

	WriteRegStr HKLM "${REGKEY}\Components" Application 1
SectionEnd

SectionGroup Extra
	Section Scripts SEC_EXT_SCRIPT
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
	
	Section "CVS Plugin" SEC_XNX_PLUGINCVS
		SectionIn RO
		SetOutPath $INSTDIR\plugins
		SetOverwrite on
		File ..\plugins\cvsplugin.dll
		WriteRegStr HKLM "${REGKEY}\Components" "CVS Plugin" 1
	SectionEnd
	
	Section "SVN Plugin" SEC_EXT_PLUGINSVN
	    SetOutPath $INSTDIR\plugins
	    SetOverwrite on
	    File ..\plugins\svnplugin.dll
	    WriteRegStr HKLM "${REGKEY}\Components" "SVN Plugin" 1
	SectionEnd
	
	Section "WebServices Plugin" SEC_EXT_SERVICES
	    SetOutPath $INSTDIR\plugins
	    SetOverwrite on
	    File ..\plugins\services.dll
	    WriteRegStr HKLM "${REGKEY}\Components" "WebServices Plugin" 1
	SectionEnd

	Section "Dictionary Plugin" SEC_EXT_DICO
	    SetOutPath $INSTDIR\plugins
	    SetOverwrite on
	    File ..\plugins\dictionary.dll
	    WriteRegStr HKLM "${REGKEY}\Components" "Dictionary Plugin" 1
	SectionEnd
SectionGroupEnd

SectionGroup /e Developpement GRP_DEV
	Section /o Source SEC_DEV_SRC
		SetOutPath $INSTDIR
		SetOverwrite on
		File ..\src.7z
		WriteRegStr HKLM "${REGKEY}\Components" Source 1
	SectionEnd
	Section /o Documentation SEC_DEV_DOC
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
Section /o -un.Application UNSEC0000
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
    Delete /REBOOTOK $INSTDIR\bin\QtWebKit4.dll
    Delete /REBOOTOK $INSTDIR\bin\QtCore4.dll
    Delete /REBOOTOK $INSTDIR\bin\qcodeedit.dll
    Delete /REBOOTOK $INSTDIR\bin\xinxprojectwizard.exe
    Delete /REBOOTOK $INSTDIR\xinxcmp.dll
    Delete /REBOOTOK $INSTDIR\bin\sharedxinx.dll
    Delete /REBOOTOK $INSTDIR\bin\xinx.exe
    DeleteRegValue HKLM "${REGKEY}\Components" Application
SectionEnd

Section /o "-un.CVS Plugin" UNSEC0001
    Delete /REBOOTOK $INSTDIR\plugins\cvsplugin.dll
    DeleteRegValue HKLM "${REGKEY}\Components" "CVS Plugin"
SectionEnd

Section /o "-un.SVN Plugin" UNSEC0002
    Delete /REBOOTOK $INSTDIR\plugins\svnplugin.dll
    DeleteRegValue HKLM "${REGKEY}\Components" "SVN Plugin"
SectionEnd

Section /o "-un.WebServices Plugin" UNSEC0003
    Delete /REBOOTOK $INSTDIR\plugins\services.dll
    DeleteRegValue HKLM "${REGKEY}\Components" "WebServices Plugin"
SectionEnd

Section /o "-un.WebServices Plugin" UNSEC0008
    Delete /REBOOTOK $INSTDIR\plugins\dictionary.dll
    DeleteRegValue HKLM "${REGKEY}\Components" "Dictionary Plugin"
SectionEnd

Section /o -un.Scripts UNSEC0004
    Delete /REBOOTOK $INSTDIR\scripts\xslvalue.js
    Delete /REBOOTOK $INSTDIR\scripts\xslkey.js
    Delete /REBOOTOK $INSTDIR\scripts\test_script.html
    Delete /REBOOTOK $INSTDIR\scripts\tab.js
    Delete /REBOOTOK $INSTDIR\scripts\rtrim.js
    Delete /REBOOTOK $INSTDIR\scripts\exemple.js
    Delete /REBOOTOK $INSTDIR\scripts\changeFileName.js
    DeleteRegValue HKLM "${REGKEY}\Components" Scripts
SectionEnd

Section /o -un.Source UNSEC0005
    Delete /REBOOTOK $INSTDIR\doc\html\*.*
    DeleteRegValue HKLM "${REGKEY}\Components" Documentation
SectionEnd

Section /o -un.Documentation UNSEC0006
    Delete /REBOOTOK $INSTDIR\doc\html\*.*
    DeleteRegValue HKLM "${REGKEY}\Components" Documentation
SectionEnd

Section -un.post UNSEC0007
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

# Uninstaller functions
Function un.onInit
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_UNSECTION Application ${UNSEC0000}
    !insertmacro SELECT_UNSECTION "CVS Plugin" ${UNSEC0001}
    !insertmacro SELECT_UNSECTION "SVN Plugin" ${UNSEC0002}
    !insertmacro SELECT_UNSECTION "WebServices Plugin" ${UNSEC0003}
    !insertmacro SELECT_UNSECTION "Dictionary Plugin" ${UNSEC0008}
    !insertmacro SELECT_UNSECTION Scripts ${UNSEC0004}
    !insertmacro SELECT_UNSECTION Source ${UNSEC0005}
    !insertmacro SELECT_UNSECTION Documentation ${UNSEC0006}
FunctionEnd

# Section Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_XNX_APP} $(SEC_XNX_APP_DESC)
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_EXT_SCRIPT} $(SEC_EXT_SCRIPT_DESC)
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_XNX_PLUGINCVS} $(SEC_XNX_PLUGINCVS_DESC)
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_EXT_PLUGINSVN} $(SEC_EXT_PLUGINSVN_DESC)
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_EXT_SERVICES} $(SEC_EXT_SERVICES_DESC)
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_EXT_DICO} $(SEC_EXT_DICO_DESC)
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DEV_SRC} $(SEC_DEV_SRC_DESC)
!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DEV_DOC} $(SEC_DEV_DOC_DESC)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

# Installer Language Strings
# TODO Update the Language Strings with the appropriate translations.

LangString ^UninstallLink ${LANG_FRENCH} "Uninstall $(^Name)"
LangString ^UninstallLink ${LANG_ENGLISH} "Uninstall $(^Name)"

LangString UNINSTALL_OLD_XINX ${LANG_FRENCH} "Il est necessaire de d�sinstaller l'ancienne version de XINX avant d'installer cette nouvelle version. Voulez-vous d�sinstaller XINX ?"
LangString UNINSTALL_OLD_DBUS ${LANG_FRENCH} "Il est necessaire de d�sinstaller l'ancienne version de D-BUS avant d'installer cette nouvelle version. Voulez-vous d�sinstaller D-BUS ?"
LangString SEC_XNX_APP_DESC ${LANG_FRENCH} "Le necessaire au d�marrage de l'application XINX et � l'utilisation des foncitonnalit�s de base"
LangString SEC_EXT_SCRIPT_DESC ${LANG_FRENCH} "Des scriptes suppl�mentaire pour l'utilisation de XINX"
LangString SEC_XNX_PLUGINCVS_DESC ${LANG_FRENCH} "Un plugin permettant d'acc�der � referenciel CVS"
LangString SEC_EXT_PLUGINSVN_DESC ${LANG_FRENCH} "Un plugin permettant d'acc�der � referenciel SVN"
LangString SEC_EXT_SERVICES_DESC ${LANG_FRENCH} "Un plugin permettant d'utiliser les services internet"
LangString SEC_EXT_DICO_DESC ${LANG_FRENCH} "Un plugin permettant d'afficher le dictionnaire"
LangString SEC_DEV_SRC_DESC ${LANG_FRENCH} "Les sources de XINX"
LangString SEC_DEV_DOC_DESC ${LANG_FRENCH} "La documentation technique de XINX"
LangString SEC0002_DESC ${LANG_FRENCH} "Script utilitaire � XINX"

LangString UNINSTALL_OLD_XINX ${LANG_ENGLISH} "You must uninstall old XINX version to install this XINX version. Do you want uninstall XINX ?"
LangString UNINSTALL_OLD_DBUS ${LANG_ENGLISH} "You must uninstall old D-BUS version to install this XINX version. Do you want uninstall D-BUS ?"
LangString SEC_XNX_APP_DESC ${LANG_ENGLISH} "The necessary for XINX start"
LangString SEC_EXT_SCRIPT_DESC ${LANG_ENGLISH} "Some script for XINX"
LangString SEC_XNX_PLUGINCVS_DESC ${LANG_ENGLISH} "A plugin to access to CVS repository"
LangString SEC_EXT_PLUGINSVN_DESC ${LANG_ENGLISH} "A plugin to access to SubVersion repository"
LangString SEC_EXT_SERVICES_DESC ${LANG_ENGLISH} "A plugin to use Web Services"
LangString SEC_EXT_DICO_DESC ${LANG_ENGLISH} "A plugin to use dictionary"
LangString SEC_DEV_SRC_DESC ${LANG_ENGLISH} "Source of XINX"
LangString SEC_DEV_DOC_DESC ${LANG_ENGLISH} "Technical documentation"
LangString SEC0002_DESC ${LANG_ENGLISH} "Script utilitaire � XINX"
