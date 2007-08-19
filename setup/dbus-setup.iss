; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define DBUSDIR GetEnv("DBUS_DIR")

[Setup]
AppName=DBus for Windows
AppVerName=DBus for Windows pre-1.0
DefaultDirName={pf}\dbus
DefaultGroupName=D-Bus
OutputDir=.\
OutputBaseFilename=dbus-pre-1.0
Compression=lzma
SolidCompression=true
UserInfoPage=true
TerminalServicesAware=false
MinVersion=0,5.0.2195
AllowUNCPath=false
ChangesEnvironment=true
AppID={{5561D91E-B126-4C8A-A2A1-E9C095528FD9}

[Tasks]
Name: modifypath; Description: &Add application directory to your system path

[Files]
Source: {#DBUSDIR}\bin\dbus-daemon.exe; DestDir: {app}\bin; Flags: uninsrestartdelete restartreplace
Source: {#DBUSDIR}\bin\dbus-env.bat; DestDir: {app}\bin; Flags: uninsrestartdelete restartreplace
Source: {#DBUSDIR}\bin\dbus-launch.exe; DestDir: {app}\bin; Flags: uninsrestartdelete restartreplace
Source: {#DBUSDIR}\bin\dbus-monitor.exe; DestDir: {app}\bin; Flags: uninsrestartdelete restartreplace
Source: {#DBUSDIR}\bin\dbus-send.exe; DestDir: {app}\bin
Source: {#DBUSDIR}\bin\libdbus-1.dll; DestDir: {app}\bin; Flags: uninsrestartdelete restartreplace
Source: {#DBUSDIR}\bin\libexpat.dll; DestDir: {app}\bin; Flags: uninsrestartdelete restartreplace
Source: {#DBUSDIR}\etc\session.conf; DestDir: {app}\etc
Source: {#DBUSDIR}\etc\system.conf; DestDir: {app}\etc
Source: {#DBUSDIR}\include\dbus\dbus-address.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-arch-deps.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-bus.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-connection.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-errors.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-macros.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-memory.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-message.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-misc.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-pending-call.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-protocol.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-server.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-shared.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-signature.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-threads.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus-types.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\include\dbus\dbus.h; DestDir: {app}\include\dbus
Source: {#DBUSDIR}\lib\libdbus-1.dll.a; DestDir: {app}\lib

[Icons]
Name: {group}\{cm:UninstallProgram,DBUS}; Filename: {uninstallexe}
Name: {group}\D-BUS Launcher; Filename: {app}\bin\dbus-launch.exe

[Code]
// Represente a path for a soft : the Home variable, The Value, and The Line to add to path
type
	TPathRecord = record
		HomeVariable: String;
		HomeValue   : String;
		PathValue   : String;
	end;
	TPathArray = Array of TPathRecord;

function ModPathDir(): TPathArray;
var Dir: TPathArray;
begin
	setArrayLength( Dir, 1 );

	Dir[0].HomeVariable := 'DBUSDIR';
	Dir[0].HomeValue    := '{app}';
	Dir[0].PathValue    := '%DBUSDIR%\bin';

	Result   := Dir;
end;

procedure ModPath();
var
	oldpath:	String;
	newpath:	String;
	curpath:	String;
	i, d:		Integer;
	Maxdir:		Integer;
	pathdir:	TPathArray;
begin
	// Get array of new directories and act on each individually
	pathdir := ModPathDir();
	MaxDir  := GetArrayLength( pathdir );
	for d := 0 to MaxDir-1 do begin
		// Get current path, split into an array
		RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', oldpath);
		oldpath := oldpath + ';';
		i := 0;
		while (Pos(';', oldpath) > 0) do begin
			curpath := Copy(oldpath, 0, Pos(';', oldpath)-1);
			oldpath := Copy(oldpath, Pos(';', oldpath)+1, Length(oldpath));
			i := i + 1;

			// Check if current directory matches app dir
			if ExpandConstant( pathdir[d].pathvalue ) = curpath then begin
				// if uninstalling, remove dir from path
				if IsUninstaller() = true then begin
					continue;
				// if installing, abort because dir was already in path
				end else begin
					abort;
				end;
			end;

			// Add current directory to new path
			if i = 1 then begin
				newpath := curpath;
			end else begin
				newpath := newpath + ';' + curpath;
			end;
		end;

		// Append app dir to path if not already included
		if IsUninstaller() = false then
			newpath := newpath + ';' + ExpandConstant( pathdir[d].pathvalue );

		// Write new path
		RegWriteStringValue(HKEY_LOCAL_MACHINE, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', newpath);
		RegWriteStringValue(HKEY_LOCAL_MACHINE, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', ExpandConstant( pathdir[d].homevariable ), ExpandConstant( pathdir[d].homevalue ));

		// Write file to flag modifypath was selected
		//   Workaround since IsTaskSelected() cannot be called at uninstall and AppName and AppId cannot be "read" in Code section
		if IsUninstaller() = false then
			SaveStringToFile(ExpandConstant('{app}') + '\uninsTasks.txt', WizardSelectedTasks(False), False);
	end;
end;

procedure CurStepChanged( CurStep: TSetupStep );
begin
	if CurStep = ssPostInstall then
		if IsTaskSelected('modifypath') then
			ModPath();
end;

procedure CurUninstallStepChanged( CurUninstallStep: TUninstallStep );
var
	appdir:			String;
	selectedTasks:	String;
begin
	appdir := ExpandConstant('{app}')
	if CurUninstallStep = usUninstall then begin
		if LoadStringFromFile(appdir + '\uninsTasks.txt', selectedTasks) then
			if Pos('modifypath', selectedTasks) > 0 then
				ModPath();
		DeleteFile(appdir + '\uninsTasks.txt')
	end;
end;
