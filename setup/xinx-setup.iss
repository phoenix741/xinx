; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=XINX
AppVerName=XINX 0.6-4 beta
DefaultDirName={pf}\XINX
DefaultGroupName=XINX
OutputDir=.\
OutputBaseFilename=xinx-0.6-4
Compression=lzma
SolidCompression=true
ShowLanguageDialog=yes
AppID={{AB2A9C86-AD08-4373-98FD-6A9AA8496AEE}
AppCopyright=Generix
UserInfoPage=true
UninstallDisplayIcon={app}\xinx.exe
LicenseFile=..\COPYING
VersionInfoVersion=0.6.4
VersionInfoCompany=Generix
AppMutex=com.generix.xmlstudio.mutex

[Languages]
Name: english; MessagesFile: compiler:Default.isl
Name: french; MessagesFile: compiler:Languages\French.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}
Name: assoxml; Description: Use XML/XSL file with this Application; Flags: unchecked

[Files]
Source: ..\COPYING; DestDir: {app}; Components: application
Source: ..\bin\xinx.exe; DestDir: {app}; Components: application; Flags: replacesameversion
Source: ..\bin\xinx_fr.qm; DestDir: {app}; Components: application; Flags: ignoreversion; Languages: french
Source: C:\Qt\4.2.3\translations\qt_fr.qm; DestDir: {app}; Components: application; Flags: ignoreversion; Languages: french
Source: C:\Qt\4.2.3\bin\QtNetwork4.dll; DestDir: {app}; Components: application; Flags: sharedfile
Source: C:\Qt\4.2.3\bin\QtXml4.dll; DestDir: {app}; Components: application; Flags: sharedfile
Source: C:\Qt\4.2.3\bin\mingwm10.dll; DestDir: {app}; Components: application; Flags: sharedfile
Source: C:\Qt\4.2.3\bin\QtCore4.dll; DestDir: {app}; Components: application; Flags: sharedfile
Source: C:\Qt\4.2.3\bin\QtGui4.dll; DestDir: {app}; Components: application; Flags: sharedfile
Source: ..\xml\completion.cpl; DestDir: {app}\xml; Flags: onlyifdoesntexist; Components: application
;Source: ..\xml\*.xml; DestDir: {app}\xml; Flags: onlyifdoesntexist; Components: objects

Source: ..\xinx.pro; DestDir: {app}\source; Components: source
Source: ..\COPYING; DestDir: {app}\source; Components: source
Source: ..\images\*.png; DestDir: {app}\source\images; Components: source
Source: ..\images\*.ico; DestDir: {app}\source\images; Components: source
Source: ..\src\*.cpp; DestDir: {app}\source\src; Components: source
Source: ..\src\*.h; DestDir: {app}\source\src; Components: source
Source: ..\src\*.xml; DestDir: {app}\source\src; Components: source
Source: ..\src\*.sh; DestDir: {app}\source\src; Components: source
Source: ..\ui\*.ui; DestDir: {app}\source\ui; Components: source
Source: ..\rc\*.rc; DestDir: {app}\source\rc; Components: source
Source: ..\translations\*.ts; DestDir: {app}\source\translations; Components: source
Source: ..\xml\completion.cpl; DestDir: {app}\source\xml; Components: source
Source: ..\setup\xinx-setup.iss; DestDir: {app}\source\setup; Components: source

[Icons]
Name: {group}\XINX; Filename: {app}\xinx.exe
Name: {group}\{cm:UninstallProgram,XINX}; Filename: {uninstallexe}
Name: {userdesktop}\XINX; Filename: {app}\xinx.exe; Tasks: desktopicon

[Registry]
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinx.exe; ValueType: string; ValueName: Path; ValueData: {app}; Tasks: assoxml
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinx.exe; ValueType: string; ValueData: {app}\xinx.exe; Tasks: assoxml
Root: HKCR; SubKey: .xsl; ValueType: string; ValueData: Fichier XSL; Flags: uninsdeletekey; Tasks: assoxml
Root: HKCR; SubKey: Fichier XSL; ValueType: string; ValueData: Fichier de présentation; Flags: uninsdeletekey; Tasks: assoxml
Root: HKCR; SubKey: Fichier XSL\Shell\Open\Command; ValueType: string; ValueData: """{app}\xinx.exe"" ""%1"""; Flags: uninsdeletevalue; Tasks: assoxml

[Components]
Name: application; Description: Application; Flags: fixed; Types: custom compact full; Languages: 
;Name: objects; Description: Java Objects File
Name: source; Description: Source de l'Application; Types: full; Languages: 

[Dirs]
Name: {app}\source\bin

[InstallDelete]
Name: {app}\xmlvisualstudio.exe; Type: files; Components: application
