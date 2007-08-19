; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define AppName "XINX"
#define AppVersion GetFileVersion("..\bin\xinx.exe")
#define QTDIR GetEnv("QTDIR")
#define QtVersion GetFileVersion( GetEnv("QTDIR") + "\bin\QtCore4.dll" );

[Setup]
AppName={#AppName}
AppVerName={#AppName} {#AppVersion} beta
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
VersionInfoDescription=XSL / Javascript editor
ChangesAssociations=true
AppVersion={#AppVersion}
UninstallDisplayName={#AppName}
TerminalServicesAware=false
MinVersion=0,5.0.2195
AllowUNCPath=false

[Languages]
Name: english; MessagesFile: compiler:Default.isl
Name: french; MessagesFile: compiler:Languages\French.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Components: application
Name: assoxml; Description: Associate XSL stylesheet with {#AppName}; Flags: unchecked
Name: remplace_completion; Description: Replace completion.xnx file; Components: 
Name: remplace_template; Description: Replace template.xnx file

[Files]
Source: ..\COPYING; DestDir: {app}; Components: application
Source: ..\bin\xinx.exe; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: ..\translations\xinx_fr.qm; DestDir: {app}\translations; Components: application; Flags: ignoreversion
Source: ..\translations\qt_fr.qm; DestDir: {app}\translations; Components: application; Flags: ignoreversion
Source: {#QTDIR}\bin\mingwm10.dll; DestDir: {app}\bin; Components: mingw; Flags: sharedfile
Source: {#QTDIR}\bin\QtNetwork4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtXml4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtCore4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile; Tasks: ; Languages: 
Source: {#QTDIR}\bin\QtGui4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtDBus4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: ..\xml\completion.xnx; DestDir: {app}\xml; Components: application; Tasks: remplace_completion
Source: ..\xml\template.xnx; DestDir: {app}\xml; Components: application; Tasks: remplace_template
Source: ..\xinx.zip; DestDir: {app}; Components: source; Flags: replacesameversion nocompression skipifsourcedoesntexist; DestName: src.zip
Source: ..\doc\html\*.*; DestDir: {app}\doc\api; Components: documentation; Flags: replacesameversion skipifsourcedoesntexist
Source: {#QTDIR}\bin\qdbusviewer.exe; DestDir: {pf}\dbus\bin; Flags: sharedfile uninsrestartdelete; Components: dbus qt
Source: dbus-pre-1.0.exe; DestDir: {tmp}; Flags: deleteafterinstall nocompression; Components: dbus; Tasks: ; Languages: ; DestName: dbus-install.exe

[Icons]
Name: {group}\{#AppName}; Filename: {app}\bin\xinx.exe; Components: application; Tasks: 
Name: {group}\{cm:UninstallProgram,XINX}; Filename: {uninstallexe}
Name: {userdesktop}\{#AppName}; Filename: {app}\bin\xinx.exe; Tasks: desktopicon; Components: application
Name: {group}\dbus\D-BUS Viewer; Filename: {pf}\dbus\bin\qdbusviewer.exe; Components: dbus

[Registry]
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinx.exe; ValueType: string; ValueName: Path; ValueData: "{app}\bin;{cf}\Qt\{#QtVersion};{cf}\MinGW;{pf}\dbus\bin"
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinx.exe; ValueType: string; ValueData: {app}\bin\xinx.exe
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\qdbusviewer.exe; ValueType: string; ValueName: Path; ValueData: "{app}\bin;{cf}\Qt\{#QtVersion};{cf}\MinGW;{pf}\dbus\bin"; Components: dbus
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\qdbusviewer.exe; ValueType: string; ValueData: {pf}\dbus\bin\qdbusviewer.exe; Components: dbus
Root: HKCR; SubKey: .xsl; ValueType: string; ValueData: Fichier XSL; Flags: uninsdeletekey; Tasks: assoxml
Root: HKCR; SubKey: Fichier XSL; ValueType: string; ValueData: Feuille de style XML; Flags: uninsdeletekey; Tasks: assoxml
Root: HKCR; SubKey: Fichier XSL\Shell\Open\Command; ValueType: string; ValueData: """{app}\bin\xinx.exe"" ""%1"""; Flags: uninsdeletevalue; Tasks: assoxml
Root: HKCR; Subkey: Fichier XSL\DefaultIcon; ValueType: string; ValueData: {app}\bin\xinx.exe,0; Flags: uninsdeletevalue
Root: HKCR; SubKey: .js; ValueType: string; ValueData: Fichier javascript; Flags: uninsdeletekey
Root: HKCR; SubKey: Fichier javascript; ValueType: string; ValueData: Fichier source JavaScript; Flags: uninsdeletekey
Root: HKCR; SubKey: Fichier javascript\Shell\Open\Command; ValueType: string; ValueData: """{app}\bin\xinx.exe"" ""%1"""; Flags: uninsdeletevalue
Root: HKCR; Subkey: Fichier javascript\DefaultIcon; ValueType: string; ValueData: {app}\bin\xinx.exe,0; Flags: uninsdeletevalue

[Components]
Name: application; Description: Application; Flags: fixed; Types: custom compact full; Languages: 
Name: source; Description: Source de l'Application; Types: full; Languages: 
Name: documentation; Description: Technical documentation of XINX; Types: full
Name: dbus; Description: D-Bus support; Types: full custom compact; Flags: fixed
Name: qt; Description: Qt Library; Flags: fixed; Types: custom compact full
Name: mingw; Description: MinGW Library; Flags: fixed; Types: custom compact full

[Run]
Filename: {tmp}\dbus-install.exe; Parameters: "/GROUP=""{groupname}\dbus"" /SP- /SILENT /NOCANCEL /NORESTART"; StatusMsg: Installation de D-BUS; Flags: hidewizard; Components: dbus

[InstallDelete]
Name: {app}\xmlvisualstudio.exe; Type: files; Components: application
Name: {app}\src; Type: filesandordirs
Name: {app}\source; Type: filesandordirs

[_ISToolPreCompile]
Name: clean.bat; Parameters: ; Flags: abortonerror
Name: sources.bat; Parameters: ; Flags: abortonerror
Name: compiler.bat; Parameters: ; Flags: abortonerror

;[Files]
;DestDir: {pf}\dbus\bin; Source: srvany.exe; Flags: sharedfile uninsrestartdelete
;DestDir: {app}\bin; Source: instsrv.exe

;[Registry]
;Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Services\DBusService\Parameters; ValueType: string; ValueName: Application; ValueData: {pf}\dbus\bin\dbus-launch.exe
;Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Services\DBusService; ValueType: none; Flags: uninsdeletekey

;[Run]
;Filename: {app}\bin\instsrv.exe; Parameters: "DBusService ""{pf}\dbus\bin\srvany.exe"""; StatusMsg: Install D-BUS service; Flags: nowait runhidden; Components: dbus
;Filename: net; Parameters: start DBusService; StatusMsg: Lancement de D-BUS; Components: dbus; Flags: runhidden

;[UninstallRun]
;Filename: net; Parameters: stop DBusService; Flags: nowait runhidden; Components: dbus
;Filename: {app}\bin\instsrv.exe; Parameters: DBusService remove; Flags: nowait runhidden; Components: dbus
