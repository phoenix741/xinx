package org.shadoware;

import org.freedesktop.dbus.DBusInterface;
import org.freedesktop.dbus.DBusInterfaceName;

@DBusInterfaceName("org.shadoware.xinx")
public interface xinx extends DBusInterface {
	void newFile( String suffix );
	void newWebservicesFile();
	void newTemplate();
	void newDefaultFile();

	void openFile( String filename );
	void saveFileAs( String filename );
	
	void saveAllFile();
	void closeFile();
	void closeAllFile();
	
	void newProject();
	void openProject( String filename );
	void closeProjectNoSessionData();
	void closeProjectWithSessionData();
	
	void callWebservices();
	void updateWebServicesList();
}
