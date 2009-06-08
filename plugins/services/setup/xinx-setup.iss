#define AppName "xinx-services"
#define AppVersion GetFileVersion("..\..\services0.dll")

[Setup]
AppName={#AppName}
AppVerName={#AppName} {#AppVersion}

AppID={{AB2A9C86-AD08-4373-98FD-6A9AA8496AEE}
DefaultDirName={pf}\XINX

OutputDir=.\
OutputBaseFilename=xinx-services-{#AppVersion}
Compression=lzma/ultra64
SolidCompression=true
ShowLanguageDialog=auto
AppCopyright=Ulrich Van Den Hekke
UserInfoPage=false
LicenseFile=..\..\..\COPYING
VersionInfoVersion={#AppVersion}
VersionInfoCompany=Ulrich Van Den Hekke
VersionInfoDescription=Upgrade the stylesheet plugins

[Languages]
Name: english; MessagesFile: compiler:Default.isl
Name: french; MessagesFile: compiler:Languages\French.isl

[Files]
DestDir: {app}\plugins; Source: ..\plugins\services0.dll

[Registry]
Root: HKCR; SubKey: .fws; ValueType: string; ValueData: Fichier FWS; Flags: uninsdeletekey; Tasks: assofws
Root: HKCR; SubKey: Fichier FWS; ValueType: string; ValueData: Fichier WebServices; Flags: uninsdeletekey; Tasks: assofws
Root: HKCR; SubKey: Fichier FWS\Shell\Open\Command; ValueType: string; ValueData: """{app}\bin\xinx.exe"" ""%1"""; Flags: uninsdeletevalue; Tasks: assofws
Root: HKCR; Subkey: Fichier FWS\DefaultIcon; ValueType: string; ValueData: {app}\bin\xinx.exe,0; Flags: uninsdeletevalue; Tasks: assofws

[CustomMessages]
INSTALL_XINX=Il est necessaire d'installer XINX avant d'installer ce plugin.

[Code]
function InitializeSetup(): Boolean;
var InstallXinxMsg           : String;
	DialogReturn, ResultCode : Integer;
begin
	Result           := true;
	InstallXinxMsg := CustomMessage('INSTALL_XINX');
	if not RegKeyExists( HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{AB2A9C86-AD08-4373-98FD-6A9AA8496AEE}_is1' ) then
	begin
		DialogReturn := MsgBox( InstallXinxMsg, mbError, MB_OK );
		Result := False
	else
//		RegQueryStringValue( HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{AB2A9C86-AD08-4373-98FD-6A9AA8496AEE}_is1', 'QuietUninstallString', UninstallString );
	end;
end;

