#define AppName "xinx-services"
#define AppVersion GetFileVersion("..\plugins\services1.dll")

[Setup]
AppName={#AppName}
AppVerName={#AppName} {#AppVersion}
OutputDir=.\
OutputBaseFilename=xinx-services-{#AppVersion}
Compression=lzma/ultra64
SolidCompression=true
ShowLanguageDialog=auto
AppCopyright=Ulrich Van Den Hekke
UserInfoPage=false
LicenseFile=..\COPYING
VersionInfoVersion={#AppVersion}
VersionInfoCompany=Ulrich Van Den Hekke
VersionInfoDescription=Upgrade the stylesheet plugins over XINX 0.8.1.2
ChangesAssociations=true
AllowCancelDuringInstall=true
CreateAppDir=false
DisableProgramGroupPage=true
Uninstallable=false
VersionInfoCopyright=2009 (c) Ulrich Van Den Hekke

[Languages]
Name: english; MessagesFile: compiler:Default.isl
Name: french; MessagesFile: compiler:Languages\French.isl

[Tasks]
Name: assofws; Description: Associate WebServices stream with XINX; Flags: unchecked

[Files]
DestDir: {code:GetInstallPathStr}\plugins; Source: ..\plugins\services1.dll; Tasks: ; Languages: ; DestName: services.dll; Flags: replacesameversion

[Registry]
Root: HKCR; SubKey: .fws; ValueType: string; ValueData: Fichier FWS; Flags: uninsdeletekey; Tasks: assofws
Root: HKCR; SubKey: Fichier FWS; ValueType: string; ValueData: Fichier WebServices; Flags: uninsdeletekey; Tasks: assofws
Root: HKCR; SubKey: Fichier FWS\Shell\Open\Command; ValueType: string; ValueData: """{code:GetInstallPathStr}\bin\xinx.exe"" ""%1"""; Flags: uninsdeletevalue; Tasks: assofws
Root: HKCR; Subkey: Fichier FWS\DefaultIcon; ValueType: string; ValueData: {code:GetInstallPathStr}\bin\xinx.exe,0; Flags: uninsdeletevalue; Tasks: assofws

[CustomMessages]
INSTALL_XINX=Il est necessaire d'installer XINX avant d'installer ce plugin.

[Code]
var InstallPathStr : String;

function InitializeSetup(): Boolean;
var InstallXinxMsg : String;
	DialogReturn   : Integer;
begin
	Result           := true;
	InstallXinxMsg := CustomMessage('INSTALL_XINX');
	if not RegKeyExists( HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{AB2A9C86-AD08-4373-98FD-6A9AA8496AEE}_is1' ) then
	begin
		DialogReturn := MsgBox( InstallXinxMsg, mbError, MB_OK );
		Result := False
	end else begin
		RegQueryStringValue( HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{AB2A9C86-AD08-4373-98FD-6A9AA8496AEE}_is1', 'InstallLocation', InstallPathStr );
	end;
end;

function GetInstallPathStr(Param: String): String;
begin
	Result := InstallPathStr;
end;
