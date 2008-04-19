import org.freedesktop.dbus.DBusConnection;
import org.freedesktop.dbus.exceptions.DBusException;
import org.shadoware.xinx;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;

import com.trolltech.qt.QtJambiGeneratedClass;
import com.trolltech.qt.QtJambiGuiInternal;
import com.trolltech.qt.QtJambiInterface;
import com.trolltech.qt.QtJambiInternal;
import com.trolltech.qt.QtJambiObject;
import com.trolltech.qt.core.Qt;
import com.trolltech.qt.gui.QApplication;
import com.trolltech.qt.gui.QCloseEvent;
import com.trolltech.qt.gui.QInputDialog;
import com.trolltech.qt.gui.QListWidgetItem;
import com.trolltech.qt.gui.QWidget;

public class XinxQuery extends QWidget{
	private DBusConnection conn = null;
	private xinx xinxInterface = null;
	private Ui_XinxQueryUI ui = new Ui_XinxQueryUI();
	private final ArrayList<String> hiddenMethodeName = new ArrayList<String>() {
		/* constructor */ {
			add("hashCode");
			add("equals");
			add("toString");
			add("Introspect");
			add("isRemote");
			add("Get");
			add("Set");
			add("isProxyClass");
			add("getProxyClass");
			add("newProxyInstance");
			add("getInvocationHandler");
			add("getClass");
			add("wait");
			add("notify");
			add("notifyAll");
		}
	};
	
	public XinxQuery( QWidget parent ) {
		super( parent );
		ui.setupUi( this );
		
		ui.logTextEdit.append(tr("<b>Welcom in XINX D-Bus Query</b>"));
	}
	
	public void closeEvent( QCloseEvent event ) {
		if( conn != null )
			conn.disconnect();
		super.closeEvent( event );
	}
	
	public void createConnection() throws DBusException {
		conn = DBusConnection.getConnection( DBusConnection.SESSION );
		ui.logTextEdit.append( "<span style=\"color: green\">Succefully connected to D-BUS.</span>" );
		xinxInterface = (xinx)conn.getRemoteObject( "org.shadoware.xinx", "/" );
		ui.logTextEdit.append( "<span style=\"color: green\">Succefully created interface.</span>" );
		
		Method[] methods = xinxInterface.getClass().getMethods();
		for( int i = 0 ; i < methods.length ; i++ ) {
			if( hiddenMethodeName.contains( methods[i].getName() ) ) continue;
			QListWidgetItem item = new QListWidgetItem( methods[i].toGenericString() );
			item.setData( Qt.ItemDataRole.UserRole, methods[i] );
			ui.methodListWidget.addItem( item );
		}
	}
	
	private void on_connectPushButton_clicked() {
		if( conn == null ) {
			try {
				createConnection();
				ui.connectPushButton.setText( tr("Disconnect") );
				ui.callPushButton.setEnabled( true );
			} catch( DBusException e ) {
				if( conn != null ) { 
					conn.disconnect();
					conn = null;
				}
				ui.logTextEdit.append( "<span style=\"color: red\">Can't connect to D-BUS or find XINX interface</span>" );
			}
		} else {
			conn.disconnect();
			conn = null;
			ui.connectPushButton.setText( tr("Connect") );
			ui.methodListWidget.clear();
			ui.logTextEdit.append( "<span style=\"color: green\">Succefully disconnected.</span>" );
			ui.callPushButton.setEnabled( false );
		}
	}
	
	private void on_callPushButton_clicked() {
		if( ui.methodListWidget.currentRow() < 0 ) return;
		Method method = (Method)ui.methodListWidget.currentItem().data( Qt.ItemDataRole.UserRole );
		Object[] params = new Object[ method.getParameterTypes().length ];
		for( int i = 0 ; i < method.getParameterTypes().length ; i++ ) {
			String type = method.getParameterTypes()[i].getCanonicalName();
			String name = type;
			if( type == "java.lang.String" ) {
				params[i] = QInputDialog.getText( this, tr("Enter parameter ") + i,  name + " : " );
			} else if( type == "java.lang.Integer" ) { 
				params[i] = new Integer( QInputDialog.getInteger( this, tr("Enter parameter ") + i,  name + " : " ) );
			} else if( type == "java.lang.Double" ) { 
				params[i] = new Double( QInputDialog.getDouble( this, tr("Enter parameter ") + i,  name + " : " ) );
			} else {
				ui.logTextEdit.append( tr("<span style=\"color: orange\">Parameter type %1 not managed.</span>").replaceAll( "%1", type ) );
				return;
			}
		}
		try {
			method.invoke( xinxInterface, params );
			ui.logTextEdit.append( "Method invoked" );
		} catch( IllegalAccessException e ) {
			ui.logTextEdit.append( tr("<span style=\"color: red\">Can't invok method : Illegal Access.</span>") );
		} catch( InvocationTargetException e ) {
			ui.logTextEdit.append( tr("<span style=\"color: red\">Can't invok method : Target error.</span>") );
		}
	}
	
	public static void main( String[] args ) {
		QApplication.initialize(args);

		XinxQuery main = new XinxQuery( null );
		main.show();
	        
	    QApplication.exec();
	}
};