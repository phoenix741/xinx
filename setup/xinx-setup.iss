#define AppName "XINX"
#define AppVersion GetFileVersion("..\xinx\xinx.exe")
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
Name: assojs; Description: Associate JS with {#AppName}; Flags: unchecked
Name: remplace_completion; Description: Replace completion.xnx file; GroupDescription: Data files; Components: 
Name: remplace_template; Description: Replace template.xnx file; GroupDescription: Data files

[Files]
Source: ..\COPYING; DestDir: {app}; Components: application
Source: ..\xinx\xinx.exe; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: ..\xinxprojectwizard\xinxprojectwizard.exe; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: {#QTDIR}\bin\mingwm10.dll; DestDir: {app}\bin; Components: mingw; Flags: sharedfile
Source: {#QTDIR}\bin\QtNetwork4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtXml4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtCore4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile; Tasks: ; Languages: 
Source: {#QTDIR}\bin\QtGui4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtDBus4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: ..\xinx\xml\baseplugin_xml.xml; DestDir: {app}\xml; Components: application; Tasks: remplace_completion; AfterInstall: MergeFile('{app}\xml\baseplugin_xml.xml'); DestName: baseplugin_xml.xml.new; Flags: skipifsourcedoesntexist
Source: ..\xinx\xml\baseplugin_js.xml; DestDir: {app}\xml; Components: application; Tasks: remplace_completion; AfterInstall: MergeFile('{app}\xml\baseplugin_js.xml'); DestName: baseplugin_js.xml.new; Flags: skipifsourcedoesntexist
Source: ..\xinx\xml\baseplugin_css.xml; DestDir: {app}\xml; Components: application; Tasks: remplace_completion; AfterInstall: MergeFile('{app}\xml\baseplugin_css.xml'); DestName: baseplugin_css.xml.new; Flags: skipifsourcedoesntexist; Languages: 
Source: ..\xinx\xml\template.xnx; DestDir: {app}\xml; Components: application; Tasks: remplace_template; AfterInstall: MergeFile('{app}\xml\template.xnx'); DestName: template.xnx.new
Source: ..\xinx.zip; DestDir: {app}; Components: source; Flags: replacesameversion nocompression skipifsourcedoesntexist; DestName: src.zip
Source: ..\doc\html\*.*; DestDir: {app}\doc\api; Components: documentation; Flags: replacesameversion skipifsourcedoesntexist
Source: {#QTDIR}\bin\qdbusviewer.exe; DestDir: {pf}\dbus\bin; Flags: sharedfile uninsrestartdelete skipifsourcedoesntexist; Components: dbus qt
Source: dbus-pre-1.0.exe; DestDir: {tmp}; Flags: deleteafterinstall nocompression skipifsourcedoesntexist; Components: dbus; Tasks: ; Languages: ; DestName: dbus-install.exe

[Icons]
Name: {group}\{#AppName}; Filename: {app}\bin\xinx.exe; Components: application; Tasks: 
Name: {group}\{#AppName} Project Wizard; Filename: {app}\bin\xinxprojectwizard.exe; Components: application; Tasks: 
Name: {group}\{cm:UninstallProgram,XINX}; Filename: {uninstallexe}
Name: {userdesktop}\{#AppName}; Filename: {app}\bin\xinx.exe; Tasks: desktopicon; Components: application
Name: {group}\dbus\D-BUS Viewer; Filename: {pf}\dbus\bin\qdbusviewer.exe; Components: dbus
Name: {group}\Documentation (API); Filename: {app}\doc\api\index.html; Comment: Documentation API de XINX; Components: documentation

[Registry]
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinxprojectwizard.exe; ValueType: string; ValueName: Path; ValueData: "{app}\bin;{cf}\Qt\{#QtVersion};{cf}\MinGW;{pf}\dbus\bin"
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinxprojectwizard.exe; ValueType: string; ValueData: {app}\bin\xinxprojectwizard.exe
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinx.exe; ValueType: string; ValueName: Path; ValueData: "{app}\bin;{cf}\Qt\{#QtVersion};{cf}\MinGW;{pf}\dbus\bin"
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\xinx.exe; ValueType: string; ValueData: {app}\bin\xinx.exe
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\qdbusviewer.exe; ValueType: string; ValueName: Path; ValueData: "{app}\bin;{cf}\Qt\{#QtVersion};{cf}\MinGW;{pf}\dbus\bin"; Components: dbus
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\qdbusviewer.exe; ValueType: string; ValueData: {pf}\dbus\bin\qdbusviewer.exe; Components: dbus
Root: HKCR; SubKey: .xsl; ValueType: string; ValueData: Fichier XSL; Flags: uninsdeletekey; Tasks: assoxml
Root: HKCR; SubKey: Fichier XSL; ValueType: string; ValueData: Feuille de style XML; Flags: uninsdeletekey; Tasks: assoxml
Root: HKCR; SubKey: Fichier XSL\Shell\Open\Command; ValueType: string; ValueData: """{app}\bin\xinx.exe"" ""%1"""; Flags: uninsdeletevalue; Tasks: assoxml
Root: HKCR; Subkey: Fichier XSL\DefaultIcon; ValueType: string; ValueData: {app}\bin\xinx.exe,0; Flags: uninsdeletevalue; Tasks: assoxml
Root: HKCR; SubKey: .js; ValueType: string; ValueData: Fichier javascript; Flags: uninsdeletekey; Tasks: assojs
Root: HKCR; SubKey: Fichier javascript; ValueType: string; ValueData: Fichier source JavaScript; Flags: uninsdeletekey; Tasks: assojs
Root: HKCR; SubKey: Fichier javascript\Shell\Open\Command; ValueType: string; ValueData: """{app}\bin\xinx.exe"" ""%1"""; Flags: uninsdeletevalue; Tasks: assojs
Root: HKCR; Subkey: Fichier javascript\DefaultIcon; ValueType: string; ValueData: {app}\bin\xinx.exe,0; Flags: uninsdeletevalue; Tasks: assojs
Root: HKCU; Subkey: Software\Shadoware.Org\XINX\Tools; ValueType: string; ValueName: cvs; ValueData: {code:GetCVSPath}; Components: ; Tasks: ; Languages: 
Root: HKCU; Subkey: Software\Shadoware.Org\XINX\Tools; ValueType: string; ValueName: diff; ValueData: {code:GetWinmergePath}; Components: ; Tasks: ; Languages: 

[Components]
Name: application; Description: Application; Flags: fixed; Types: custom compact full; Languages: 
Name: source; Description: Source de l'Application; Types: full; Languages: 
Name: documentation; Description: Technical documentation of XINX; Types: full
Name: dbus; Description: D-Bus support; Types: full custom compact; Flags: fixed
Name: qt; Description: Qt Library; Flags: fixed; Types: custom compact full
Name: mingw; Description: MinGW Library; Flags: fixed; Types: custom compact full
Name: webplugin; Description: Plugin for XML, XSL, HTML, CSS, JS; Flags: fixed; Types: custom compact full

[Run]
Filename: {tmp}\dbus-install.exe; Parameters: "/GROUP=""{groupname}\dbus"" /SP- /SILENT /NOCANCEL /NORESTART"; StatusMsg: Installation de D-BUS; Flags: hidewizard; Components: dbus

;[_ISToolPreCompile]
;Name: clean.bat; Parameters: ; Flags: abortonerror
;Name: sources.bat; Parameters: ; Flags: abortonerror
;Name: doc.bat; Parameters: ; Flags: abortonerror
;Name: compiler.bat; Parameters: ; Flags: abortonerror

[Code]
var
	FilesWizardPage: TInputFileWizardPage;

procedure Replace( var Chaine: String; c1, c2: Char );
var I: Integer;
begin
  for I := 1 to Length( Chaine ) do
    if( Chaine[I] = c1 ) then
		Chaine[I] := c2;
end;

procedure InitializeWizard;
var DefaultCVSPath,
    DefaultMergePath: String;
begin
  { Create the pages }

  FilesWizardPage := CreateInputFilePage(wpSelectComponents,
    'Select tools locations', 'Where is located your tools?',
    'Select where CVS and Winmerge is located then click Next.');

  FilesWizardPage.Add('Location of cvs.exe',
    'Executable files|*.exe|All files|*.*',
    '.exe');
  FilesWizardPage.Add('Location of winmerge.exe',
    'Executable files|*.exe|All files|*.*',
    '.exe');

  RegQueryStringValue(HKEY_CURRENT_USER, 'Software\Shadoware.Org\XINX\Tools', 'cvs', DefaultCVSPath );
  RegQueryStringValue(HKEY_CURRENT_USER, 'Software\Shadoware.Org\XINX\Tools', 'diff', DefaultMergePath );
  if( DefaultCVSPath = '' ) then
	DefaultCVSPath := ExpandConstant('{pf}\TortoiseCVS\cvs.exe')
  else
    Replace( DefaultCVSPath, '/', '\' );

  if( DefaultMergePath = '' ) then
	DefaultMergePath := ExpandConstant('{pf}\WinMerge\WinmergeU.exe')
  else
    Replace( DefaultMergePath, '/', '\' );

  FilesWizardPage.Values[0] := DefaultCVSPath;
  FilesWizardPage.Values[1] := DefaultMergePath;
end;

function NextButtonClick( CurPageID: Integer ): Boolean;
begin
  { Validate certain pages before allowing the user to proceed }
  if CurPageID = FilesWizardPage.ID then begin
    if not FileExists( FilesWizardPage.Values[0] ) then begin
      MsgBox('You must enter a correct CVS path', mbError, MB_OK);
      Result := False;
    end else
    if not FileExists( FilesWizardPage.Values[1] ) then begin
      MsgBox('You must enter a correct WinMerge path', mbError, MB_OK);
      Result := False;
    end else
      Result := True;
  end else
    Result := True;
end;

function GetCVSPath( Param: String ): String;
begin
  Result := FilesWizardPage.Values[0];
end;

function GetWinmergePath( Param: String ): String;
begin
  Result := FilesWizardPage.Values[1];
end;

procedure MergeFile( Param: String );
var SizeOld, SizeNew, ResultCode: Integer;
    WinMergeApp, Exp: String;
    Diff : bool;
begin
  Diff  := False;
  Exp := ExpandConstant( Param );

  FileSize( Exp, SizeOld ); FileSize( Exp + '.new', SizeNew );

  if( SizeOld <> SizeNew ) then
    Diff := True;

  FileCopy( Exp, Exp + '.old', False );
  FileCopy( Exp + '.new', Exp, False );

  if( Diff ) then begin
	WinMergeApp := GetWinmergePath( '' );
	Exec( WinMergeApp, '"' + Exp + '" "' + Exp + '.old"', '', SW_SHOW, ewWaitUntilTerminated, ResultCode );
  end;
end;

function UpdateReadyMemo(Space, NewLine, MemoUserInfoInfo, MemoDirInfo, MemoTypeInfo,
  MemoComponentsInfo, MemoGroupInfo, MemoTasksInfo: String): String;
var
  S: String;
begin
  { Fill the 'Ready Memo' with the normal settings and the custom settings }
  S := '';
  S := S + MemoUserInfoInfo + NewLine;
  S := S + NewLine;
  S := S + MemoDirInfo + NewLine;
  S := S + NewLine;
  S := S + 'Tools:' + NewLine;
  S := S + Space + 'CVS: ' + FilesWizardPage.Values[0] + NewLine;
  S := S + Space + 'WinMerge: ' + FilesWizardPage.Values[1] + NewLine;
  S := S + NewLine;
  S := S + MemoTypeInfo + NewLine;
  S := S + NewLine;
  S := S + MemoComponentsInfo + NewLine;
  S := S + NewLine;
  S := S + MemoGroupInfo + NewLine;
  S := S + NewLine;
  S := S + MemoTasksInfo + NewLine;
  S := S + NewLine;

  Result := S;
end;

