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
Name: assojs; Description: Associate JS with {#AppName}; Flags: unchecked
Name: remplace_completion; Description: Replace completion.xnx file; GroupDescription: Data files; Components:
Name: remplace_template; Description: Replace template.xnx file; GroupDescription: Data files

[Files]
Source: ..\COPYING; DestDir: {app}; Components: application
Source: ..\bin\xinx.exe; DestDir: {app}\bin; Components: application; Flags: replacesameversion
Source: {#QTDIR}\bin\mingwm10.dll; DestDir: {app}\bin; Components: mingw; Flags: sharedfile
Source: {#QTDIR}\bin\QtNetwork4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtXml4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtCore4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile; Tasks: ; Languages: 
Source: {#QTDIR}\bin\QtGui4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: {#QTDIR}\bin\QtDBus4.dll; DestDir: {app}\bin; Components: qt; Flags: sharedfile
Source: ..\xml\completion.xnx; DestDir: {app}\xml; Components: application; Tasks: remplace_completion; AfterInstall: MergeCompletionFile('{app}\xml'); DestName: completion.xnx.new; BeforeInstall: CompareCompletionFile('{app}\xml')
Source: ..\xml\template.xnx; DestDir: {app}\xml; Components: application; Tasks: remplace_template; AfterInstall: MergeTemplateFile('{app}\xml'); DestName: template.xnx.new; BeforeInstall: CompareTemplateFile('{app}\xml')
Source: ..\xinx.zip; DestDir: {app}; Components: source; Flags: replacesameversion nocompression skipifsourcedoesntexist; DestName: src.zip
Source: ..\doc\html\*.*; DestDir: {app}\doc\api; Components: documentation; Flags: replacesameversion skipifsourcedoesntexist
Source: {#QTDIR}\bin\qdbusviewer.exe; DestDir: {pf}\dbus\bin; Flags: sharedfile uninsrestartdelete; Components: dbus qt
Source: dbus-pre-1.0.exe; DestDir: {tmp}; Flags: deleteafterinstall nocompression; Components: dbus; Tasks: ; Languages: ; DestName: dbus-install.exe

[Icons]
Name: {group}\{#AppName}; Filename: {app}\bin\xinx.exe; Components: application; Tasks: 
Name: {group}\{cm:UninstallProgram,XINX}; Filename: {uninstallexe}
Name: {userdesktop}\{#AppName}; Filename: {app}\bin\xinx.exe; Tasks: desktopicon; Components: application
Name: {group}\dbus\D-BUS Viewer; Filename: {pf}\dbus\bin\qdbusviewer.exe; Components: dbus
Name: {group}\Documentation (API); Filename: {app}\doc\api\index.html; Comment: Documentation API de XINX; Components: documentation

[Registry]
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

[Run]
Filename: {tmp}\dbus-install.exe; Parameters: "/GROUP=""{groupname}\dbus"" /SP- /SILENT /NOCANCEL /NORESTART"; StatusMsg: Installation de D-BUS; Flags: hidewizard; Components: dbus

[InstallDelete]
Name: {app}\xmlvisualstudio.exe; Type: files; Components: application
Name: {app}\src; Type: filesandordirs
Name: {app}\source; Type: filesandordirs
Name: {app}\translations; Type: filesandordirs

[_ISToolPreCompile]
Name: clean.bat; Parameters: ; Flags: abortonerror
Name: sources.bat; Parameters: ; Flags: abortonerror
Name: compiler.bat; Parameters: ; Flags: abortonerror

[Code]
var
	FilesWizardPage: TInputFileWizardPage;
	DeveloppementMsgPage: TOutputMsgWizardPage;

	CompletionResult, TemplateResult: boolean;

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

  DeveloppementMsgPage := CreateOutputMsgPage(wpWelcome,
    'Type of Installation', 'What is done to install developpement environment?',
    'Note: To install the developement environment of XINX, the installer will ' +
    'make this step:'#10#13 +
    '- Download and Install MinGW'#10#13 +
    '- Download and Install expat xml library'#10#13 +
    '- Download and Install cmake'#10#13 +
    '- Download and Install win32libs'#10#13
    '- Download (by svn), compile, and install windbus (patched for windows)'#10#13 +
    '- Download, and Compile Qt with QtDbus support'#10#13 +
    '- Compile Xinx'#10#13 +
    '- Install optional program as Doxygen, QDevelop, ...');

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

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  { Skip pages that shouldn't be shown }
  if PageID = DeveloppementMsgPage.ID then
    Result := True
  else
    Result := False;
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

procedure MergeFile( Param: String; isComparer: boolean );
var WinMergeApp, Exp: String;
    ResultCode : Integer;
begin
  Exp := ExpandConstant( Param );
  FileCopy( Exp + '.new', Exp, False );
  if( not isComparer ) then begin
	WinMergeApp := GetWinmergePath( '' );
	Exec( WinMergeApp, '"' + Exp + '" "' + Exp + '.old"', '', SW_SHOW, ewWaitUntilTerminated, ResultCode );
  end;
end;

procedure MergeCompletionFile( Param: String );
begin
  MergeFile( Param + '\completion.xnx', CompletionResult );
end;

procedure MergeTemplateFile( Param: String );
begin
  MergeFile( Param + '\template.xnx', TemplateResult );
end;

function CompareFile( Param: String ) : Boolean;
var SizeOld, SizeNew: Integer;
    Exp: String;
begin
  Exp := ExpandConstant( Param );
  Result := True;
  FileSize( Exp, SizeOld );
  FileSize( Exp + '.new', SizeNew );
  if( SizeOld <> SizeNew ) then
    Result := False;
  FileCopy( Exp, Exp + '.old', False );
end;

procedure CompareCompletionFile( Param: String );
begin
  CompletionResult := CompareFile( Param + '\completion.xnx' );
end;

procedure CompareTemplateFile( Param: String );
begin
  TemplateResult := CompareFile( Param + '\template.xnx' );
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
