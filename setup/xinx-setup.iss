#define AppName "XINX"
#define AppVersion GetFileVersion("..\xinx\xinx.exe")
#define QTDIR GetEnv("QTDIR")
#define QtVersion GetFileVersion( GetEnv("QTDIR") + "\bin\QtCore4.dll" );

[Setup]
AppName={#AppName}
AppVerName={#AppName} {#AppVersion} pre-version
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
OutputDir=.\
OutputBaseFilename=xinx-{#AppVersion}
Compression=lzma
SolidCompression=true
ShowLanguageDialog=auto
AppID={{AB2A9C86-AD08-4373-98FD-6A9AA8496AEE}
AppCopyright=Ulrich Van Den Hekke
UserInfoPage=true
UninstallDisplayIcon={app}\xinx.exe
LicenseFile=..\COPYING
VersionInfoVersion={#AppVersion}
VersionInfoCompany=Ulrich Van Den Hekke
VersionInfoDescription=Project Editor
ChangesAssociations=true
AppVersion={#AppVersion}
UninstallDisplayName={#AppName}
TerminalServicesAware=false
MinVersion=0,5.0.2195
AllowUNCPath=false
InfoAfterFile=..\CHANGELOG
AppPublisher=Shadoware.Org
AppPublisherURL=http://xinx.shadoware.org/
AppSupportURL=http://xinx.shadoware.org/newticket
AppUpdatesURL=http://xinx.shadoware.org/downloads
AppContact=xinx@shadoware.org

[Languages]
Name: english; MessagesFile: compiler:Default.isl
Name: french; MessagesFile: compiler:Languages\French.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Components: application
Name: assoxml; Description: Associate XSL stylesheet with {#AppName}; Flags: unchecked
Name: assojs; Description: Associate JS with {#AppName}; Flags: unchecked
Name: assofws; Description: Associate WebServices stream with {#AppName}; Flags: unchecked
Name: remplace_template; Description: Replace XINX data files (please backup your modification)

[Files]
Source: ..\COPYING; DestDir: {app}; Components: application
Source: ..\xinx\xinx.exe; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: ..\components\xinxcmp.dll; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: ..\libxinx\sharedxinx.dll; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: ..\xinxprojectwizard\xinxprojectwizard.exe; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: {#QTDIR}\bin\mingwm10.dll; DestDir: {app}\bin; Components: mingw; Flags: sharedfile
Source: {#QTDIR}\bin\QtNetwork4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtXml4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtCore4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile; Tasks: ; Languages: 
Source: {#QTDIR}\bin\QtGui4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtDBus4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtScript4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtXmlPatterns4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: ..\xinx\xml\*.xml; DestDir: {app}\xml; Components: application; Tasks: remplace_template
Source: ..\xinx.zip; DestDir: {app}; Components: source; Flags: replacesameversion nocompression skipifsourcedoesntexist; DestName: src.zip
Source: ..\doc\html\*.*; DestDir: {app}\doc\api; Components: documentation; Flags: replacesameversion
Source: {#QTDIR}\bin\qdbusviewer.exe; DestDir: {pf}\dbus\bin; Flags: sharedfile uninsrestartdelete; Components: dbus qt
Source: dbus-1.1.exe; DestDir: {tmp}; Flags: deleteafterinstall nocompression skipifsourcedoesntexist; Components: dbus; Tasks: ; Languages: ; DestName: dbus-install.exe
DestDir: {app}\plugins; Source: ..\plugins\services.dll; Components: services
DestDir: {app}\plugins; Source: ..\plugins\cvsplugin.dll; Components: cvsplugin
DestDir: {app}\plugins; Source: ..\plugins\svnplugin.dll; Components: svnplugin
DestDir: {app}\plugins; Source: ..\plugins\empty.dll; Components: empty
DestDir: {app}\plugins; Source: ..\plugins\xslgui.dll; Components: experimental
DestDir: {app}\scripts; Source: ..\scripts\*.js; Components: scripts
DestDir: {app}\scripts; Source: ..\scripts\*.xq; Components: scripts
DestDir: {app}\plugins\styles; Source: {#QTDIR}\plugins\styles\explorerstyle.dll; Flags: skipifsourcedoesntexist; Components: " styles"; MinVersion: 0,6.0.6000; Tasks: ; Languages: 

[Icons]
Name: {group}\{#AppName}; Filename: {app}\bin\xinx.exe; Components: application; Tasks: 
Name: {group}\style\{#AppName} (Plastique); Filename: {app}\bin\xinx.exe; Components: application; Tasks: ; Parameters: -style Plastique; IconIndex: 0
Name: {group}\style\{#AppName} (CleanLooks); Filename: {app}\bin\xinx.exe; Components: application; Tasks: ; Parameters: -style CleanLooks; IconIndex: 0
Name: {group}\style\{#AppName} (CDE); Filename: {app}\bin\xinx.exe; Components: application; Tasks: ; Parameters: -style cde; IconIndex: 0
Name: {group}\style\{#AppName} (Motif); Filename: {app}\bin\xinx.exe; Components: application; Tasks: ; Parameters: -style motif; IconIndex: 0
Name: {group}\{#AppName} Project Wizard; Filename: {app}\bin\xinxprojectwizard.exe; Components: application; Tasks: 
Name: {group}\{cm:UninstallProgram,XINX}; Filename: {uninstallexe}
Name: {userdesktop}\{#AppName}; Filename: {app}\bin\xinx.exe; Tasks: desktopicon; Components: application
Name: {group}\dbus\D-BUS Viewer; Filename: {pf}\dbus\bin\qdbusviewer.exe; Components: dbus
Name: {group}\Documentation (API); Filename: {app}\doc\api\index.html; Comment: Documentation API de XINX; Components: documentation

[Registry]
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinxprojectwizard.exe; ValueType: string; ValueName: Path; ValueData: "{app}\bin;{pf}\dbus\bin"
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinxprojectwizard.exe; ValueType: string; ValueData: {app}\bin\xinxprojectwizard.exe
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinx.exe; ValueType: string; ValueName: Path; ValueData: "{app}\bin;{pf}\dbus\bin"
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinx.exe; ValueType: string; ValueData: {app}\bin\xinx.exe
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\qdbusviewer.exe; ValueType: string; ValueName: Path; ValueData: "{app}\bin;{pf}\dbus\bin"; Components: dbus
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\qdbusviewer.exe; ValueType: string; ValueData: {pf}\dbus\bin\qdbusviewer.exe; Components: dbus
Root: HKCR; SubKey: .xsl; ValueType: string; ValueData: Fichier XSL; Flags: uninsdeletekey; Tasks: assoxml
Root: HKCR; SubKey: Fichier XSL; ValueType: string; ValueData: Feuille de style XML; Flags: uninsdeletekey; Tasks: assoxml
Root: HKCR; SubKey: Fichier XSL\Shell\Open\Command; ValueType: string; ValueData: """{app}\bin\xinx.exe"" ""%1"""; Flags: uninsdeletevalue; Tasks: assoxml
Root: HKCR; Subkey: Fichier XSL\DefaultIcon; ValueType: string; ValueData: {app}\bin\xinx.exe,0; Flags: uninsdeletevalue; Tasks: assoxml
Root: HKCR; SubKey: .js; ValueType: string; ValueData: Fichier javascript; Flags: uninsdeletekey; Tasks: assojs
Root: HKCR; SubKey: Fichier javascript; ValueType: string; ValueData: Fichier source JavaScript; Flags: uninsdeletekey; Tasks: assojs
Root: HKCR; SubKey: Fichier javascript\Shell\Open\Command; ValueType: string; ValueData: """{app}\bin\xinx.exe"" ""%1"""; Flags: uninsdeletevalue; Tasks: assojs
Root: HKCR; Subkey: Fichier javascript\DefaultIcon; ValueType: string; ValueData: {app}\bin\xinx.exe,0; Flags: uninsdeletevalue; Tasks: assojs
Root: HKCR; SubKey: .fws; ValueType: string; ValueData: Fichier FWS; Flags: uninsdeletekey; Tasks: assofws
Root: HKCR; SubKey: Fichier FWS; ValueType: string; ValueData: Fichier WebServices; Flags: uninsdeletekey; Tasks: assofws
Root: HKCR; SubKey: Fichier FWS\Shell\Open\Command; ValueType: string; ValueData: """{app}\bin\xinx.exe"" ""%1"""; Flags: uninsdeletevalue; Tasks: assofws
Root: HKCR; Subkey: Fichier FWS\DefaultIcon; ValueType: string; ValueData: {app}\bin\xinx.exe,0; Flags: uninsdeletevalue; Tasks: assofws

[Components]
Name: application; Description: Application; Flags: fixed; Types: custom compact full; Languages: 
Name: source; Description: Source de l'Application; Types: full; Languages: 
Name: documentation; Description: Technical documentation of XINX; Types: full
Name: dbus; Description: D-Bus support; Types: full custom compact; Flags: fixed
Name: qt; Description: Qt Library; Flags: fixed; Types: custom compact full
Name: mingw; Description: MinGW Library; Flags: fixed; Types: custom compact full
Name: cvsplugin; Description: Plugin's Wrapper for CVS; Types: custom full
Name: svnplugin; Description: Plugin's Wrapper for SubVersion; Types: custom full
Name: scripts; Description: Scriptes utilitaire; Types: custom compact full
Name: services; Description: Plugin with a WebServices editor; Types: custom full
Name: experimental; Description: Plugins experimentals; Languages: 
Name: styles; Description: Styles Supplémentaires (Vista only); Types: full; MinVersion: 0,6.0.6000
Name: empty; Description: Plugin vide (pour dev)

[Run]
Filename: {tmp}\dbus-install.exe; Parameters: "/GROUP=""{groupname}\dbus"" /SP- /SILENT /NOCANCEL /NORESTART"; StatusMsg: Installation de D-BUS; Flags: hidewizard; Components: dbus
