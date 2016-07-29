#define AppName "XINX"
#define AppVersion GetFileVersion(SourcePath + "install\bin\xinx.exe")
#define QTDIR GetEnv("QTDIR")
#define LIBXML2 GetEnv("LIBXML2")
#define LIBXSLT GetEnv("LIBXSLT")
#define ICONV GetEnv("ICONV")
#define LIBSVN  GetEnv("SUBVERSION")
#define QtVersion GetFileVersion( GetEnv("QTDIR") + "\bin\QtCore4.dll" );

[Setup]
AppName={#AppName}
AppVerName={#AppName} {#AppVersion} pre-version
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
OutputDir=.\
OutputBaseFilename=xinx-{#AppVersion}
Compression=lzma/ultra64
SolidCompression=true
ShowLanguageDialog=auto
AppID={{84FAD124-CAB0-4424-8873-FC50C42E6D96}
AppCopyright=Ulrich Van Den Hekke
UserInfoPage=false
UninstallDisplayIcon={app}\xinx.exe
LicenseFile=..\COPYING
VersionInfoVersion={#AppVersion}
VersionInfoCompany=Ulrich Van Den Hekke
VersionInfoDescription=XSL Stylesheet Editor
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
VersionInfoCopyright=2011 (c) Ulrich VANDENHEKKE
InfoBeforeFile=..\README

[Languages]
Name: english; MessagesFile: compiler:Default.isl
Name: french; MessagesFile: compiler:Languages\French.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Components: application
Name: assoxml; Description: Associate XSL stylesheet with {#AppName}; Flags: unchecked
Name: assojs; Description: Associate JS with {#AppName}; Flags: unchecked
Name: assofws; Description: Associate WebServices stream with {#AppName}; Flags: unchecked

[Files]
Source: ..\COPYING; DestDir: {app}; Components: application

Source: install\bin\xinx.exe; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: install\bin\xinxcrash.exe; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: install\bin\libxinxcmp.dll; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: install\bin\libframework.dll; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: install\bin\libqcodeedit.dll; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: install\bin\libideality.dll; DestDir: {app}\bin; Components: application; Flags: replacesameversion

Source: {#QTDIR}\..\..\mingw\bin\mingwm10.dll; DestDir: {app}\bin; Components: application
Source: {#QTDIR}\..\..\mingw\bin\libgcc_s_dw2-1.dll; DestDir: {app}\bin; Components: application
Source: {#QTDIR}\..\..\mingw\bin\libstdc++-6.dll; DestDir: {app}\bin; Components: application
Source: {#QTDIR}\..\..\mingw\bin\zlib1.dll; DestDir: {app}\bin; Components: application

Source: {#LIBXML2}\bin\libxml2.dll; DestDir: {app}\bin; Components: application
Source: {#LIBXSLT}\bin\libxslt.dll; DestDir: {app}\bin; Components: application
Source: {#ICONV}\bin\iconv.dll; DestDir: {app}\bin; Components: application
Source: {#ICONV}\bin\iconv.exe; DestDir: {app}\bin; Components: application
Source: {#LIBXSLT}\bin\libexslt.dll; DestDir: {app}\bin; Components: application
Source: {#LIBXSLT}\bin\xsltproc.exe; DestDir: {app}\bin; Components: application

Source: {#QTDIR}\bin\phonon4.dll; DestDir: {app}\bin; Components: application; Flags: sharedfile; Tasks: ; Languages: 
Source: {#QTDIR}\bin\QtCore4.dll; DestDir: {app}\bin; Components: application; Flags: sharedfile; Tasks: ; Languages: 
Source: {#QTDIR}\bin\QtGui4.dll; DestDir: {app}\bin; Components: application; Flags: sharedfile
Source: {#QTDIR}\bin\QtNetwork4.dll; DestDir: {app}\bin; Components: application; Flags: sharedfile
Source: {#QTDIR}\bin\QtScript4.dll; DestDir: {app}\bin; Components: application; Flags: sharedfile
Source: {#QTDIR}\bin\QtWebKit4.dll; DestDir: {app}\bin; Components: application; Flags: sharedfile
Source: {#QTDIR}\bin\QtXml4.dll; DestDir: {app}\bin; Components: application; Flags: sharedfile
Source: {#QTDIR}\bin\QtXmlPatterns4.dll; DestDir: {app}\bin; Components: application; Flags: sharedfile
Source: {#QTDIR}\bin\QtSql4.dll; DestDir: {app}\bin; Components: application; Flags: sharedfile
Source: {#QTDIR}\bin\libwinpthread-1.dll; DestDir: {app}\bin; Components: application; Flags: sharedfile

Source: {#LIBSVN}\bin\*.*; DestDir: {app}\bin; Components: svnplugin

DestDir: {app}\scripts; Source: install\scripts\*.xq; Components: application
DestDir: {app}\scripts; Source: install\scripts\*.js; Components: scripts
DestDir: {app}\datas; Source: install\datas\*.xml; Components: application
DestDir: {app}\templates; Source: install\templates\*.xml; Components: application
DestDir: {app}\i18n; Source: install\i18n\*.qm; Components: application

Source: ..\src.7z; DestDir: {app}; Components: source; Flags: replacesameversion
Source: ..\doc\framework.chm; DestDir: {app}\doc; Components: documentation; Flags: replacesameversion

DestDir: {app}\plugins; Source: install\plugins\libservices.dll; Components: services; Flags: replacesameversion
DestDir: {app}\plugins; Source: install\plugins\libcvsplugin.dll; Components: cvsplugin; Flags: replacesameversion
DestDir: {app}\plugins; Source: install\plugins\libsvnplugin.dll; Components: svnplugin; Flags: replacesameversion
DestDir: {app}\plugins; Source: install\plugins\libsvnpluginwrapper.dll; Components: svnpluginwrapper; Flags: replacesameversion
DestDir: {app}\plugins; Source: install\plugins\libgenerix.dll; Components: generixplugin; Flags: replacesameversion

DestDir: {app}\plugins\sqldrivers; Source: {#QTDIR}\plugins\sqldrivers\qsqlite4.dll; Components: application
[Icons]
Name: {group}\{#AppName}; Filename: {app}\bin\xinx.exe; Components: application; Tasks: 
Name: {group}\style\{#AppName} (Plastique); Filename: {app}\bin\xinx.exe; Components: application; Tasks: ; Parameters: -style Plastique; IconIndex: 0
Name: {group}\style\{#AppName} (CleanLooks); Filename: {app}\bin\xinx.exe; Components: application; Tasks: ; Parameters: -style CleanLooks; IconIndex: 0
Name: {group}\style\{#AppName} (CDE); Filename: {app}\bin\xinx.exe; Components: application; Tasks: ; Parameters: -style cde; IconIndex: 0
Name: {group}\style\{#AppName} (Motif); Filename: {app}\bin\xinx.exe; Components: application; Tasks: ; Parameters: -style motif; IconIndex: 0
Name: {group}\style\{#AppName} (Float); Filename: {app}\bin\xinx.exe; Components: application; Tasks: ; Parameters: -style Float; IconIndex: 0
Name: {group}\style\{#AppName} (Explorer); Filename: {app}\bin\xinx.exe; Components: application; Tasks: ; Parameters: -style Explorer; IconIndex: 0
Name: {group}\{cm:UninstallProgram,XINX}; Filename: {uninstallexe}
Name: {userdesktop}\{#AppName}; Filename: {app}\bin\xinx.exe; Tasks: desktopicon; Components: application
Name: {group}\Documentation (API); Filename: {app}\doc\framework.chm; Comment: Documentation API de XINX; Components: documentation

[Registry]

Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinx.exe; ValueType: string; ValueName: Path; ValueData: {app}\bin; Flags: uninsdeletekey
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinx.exe; ValueType: string; ValueData: {app}\bin\xinx.exe; Flags: uninsdeletekey

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

Root: HKCU; Subkey: Software\Shadoware.Org\XINX; Flags: deletekey; Check: CheckVersion

[Components]
Name: application; Description: Application & Bibliothèques nécessaires; Flags: fixed; Types: custom compact full; Languages: 
Name: source; Description: Source de l'Application; Types: full; Languages: 
Name: documentation; Description: APIs de XINX; Types: full
Name: generixplugin; Description: Mode de fonctionnement Generix; Types: custom full
Name: cvsplugin; Description: Encapsulation de CVS; Types: custom full
Name: svnpluginwrapper; Description: Encapsulation de SubVersion; Types: custom full
Name: svnplugin; Description: Plugins SubVersion pour XINX; Types: custom full
Name: services; Description: Appel de WebServices type RPC; Types: custom full
Name: scripts; Description: Quelques scripts; Types: custom compact full
Name: styles; Description: Styles Supplémentaires (Vista only); Types: full; MinVersion: 0,6.0.6000

[CustomMessages]
UNINSTALL_XINX=Il est necessaire de désinstaller l'ancienne version de XINX avant d'installer cette nouvelle version. Voulez-vous désinstaller XINX ?
UNINSTALL_DBUS=Il est necessaire de désinstaller l'ancienne version de D-BUS avant d'installer cette nouvelle version. Voulez-vous désinstaller D-BUS ?
UNINSTALL_REGISTERY=Voulez-vous supprimer les entrées de XINX dans la base de registre ?

[InstallDelete]
Name: {app}\plugins\services.dll; Type: files; Components: services; Tasks: 

[Code]
function InitializeSetup(): Boolean;
var UninstallXinxMsg, UninstallDbusMsg: String;
	DialogReturn, ResultCode          : Integer;
	UninstallString                   : String;
begin
	Result           := true;
	UninstallXinxMsg := CustomMessage('UNINSTALL_XINX');
	UninstallDbusMsg := CustomMessage('UNINSTALL_DBUS');
	if RegKeyExists( HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{AB2A9C86-AD08-4373-98FD-6A9AA8496AEE}_is1' ) then
	begin
		RegQueryStringValue( HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{AB2A9C86-AD08-4373-98FD-6A9AA8496AEE}_is1', 'QuietUninstallString', UninstallString );
		DialogReturn := MsgBox( UninstallXinxMsg, mbConfirmation, MB_YESNO );
		if DialogReturn = IDNO then
			Result := False
		else
			Exec( '>', UninstallString, '', 1, ewWaitUntilTerminated, ResultCode );
	end;
	if Result and RegKeyExists( HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{5561D91E-B126-4C8A-A2A1-E9C095528FD9}_is1' ) then
	begin
		RegQueryStringValue( HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{5561D91E-B126-4C8A-A2A1-E9C095528FD9}_is1', 'QuietUninstallString', UninstallString );
		DialogReturn := MsgBox( UninstallDbusMsg, mbConfirmation, MB_YESNO );
		if DialogReturn = IDNO then
			Result := False
		else
			Exec( '>', UninstallString, '', 1, ewWaitUntilTerminated, ResultCode );
	end;
end;

function CheckVersion(): Boolean;
var VersionString            : String;
begin
  Result := false;
  if RegKeyExists( HKEY_CURRENT_USER, 'Software\Shadoware.Org\XINX' ) then
  begin
	  RegQueryStringValue( HKEY_CURRENT_USER, 'Software\Shadoware.Org\XINX', 'Version', VersionString );
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
	DialogReturn			 : Integer;
	UninstallRegisteryMsg	 : String;
begin
  if(CurUninstallStep = usUninstall) then
  begin
    if RegKeyExists( HKEY_CURRENT_USER, 'Software\Shadoware.Org\XINX' ) then
    begin
    	UninstallRegisteryMsg := CustomMessage('UNINSTALL_REGISTERY');
  	  DialogReturn := MsgBox( UninstallRegisteryMsg, mbConfirmation, MB_YESNO );
     	if DialogReturn = IDYES then begin
        RegDeleteKeyIncludingSubkeys( HKEY_CURRENT_USER, 'Software\Shadoware.Org\XINX' );
      end;
    end;
  end;
end;
