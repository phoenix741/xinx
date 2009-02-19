/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "xslgui.h"
#include "xuieditor.h"

// Qt header
#include <QString>
#include <QVariant>
#include <QTranslator>
#include <QApplication>

/* XSLStyleSheetFileType */

class XUIFileType : public QObject, public IFileTypePlugin {
public:
	virtual QString description() {	return tr( "XML User Interface File" ); };
	virtual QString match() { return "*.xui"; };
	virtual QIcon icon() { return QIcon( ":/xslgui/images/typexui.png" ); };

	virtual AppSettings::struct_extentions properties() {
		AppSettings::struct_extentions p;
		p.canBeCommitToRcs = true;
		p.canBeFindInConfiguration = true;
		p.canBeSaveAsSpecifique = true;
		p.specifiqueSubDirectory = "xui/";
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		XUIEditor * editor = new XUIEditor();
		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );
		return editor;
	}

	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename ) {
		return NULL;
	}

	virtual ContentViewParser * createParser() {
		return 0;
	}
};

/* XslGuiPlugin */

XslGuiPlugin::XslGuiPlugin() {
    Q_INIT_RESOURCE(xslgui);

	m_fileTypes << new XUIFileType;
}

XslGuiPlugin::~XslGuiPlugin() {
}

bool XslGuiPlugin::initializePlugin( const QString & lang ) {
	QTranslator * tranlator = new QTranslator( this );
	tranlator->load( QString(":/xslgui/translations/xslgui_%1").arg( lang ) );
	qApp->installTranslator(tranlator);

	return true;
}

QVariant XslGuiPlugin::getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) {
	switch( attr ) {
	case PLG_NAME:
		return tr("User Interface for editing Stylesheet");
	case PLG_DESCRIPTION:
		return tr("EXPERIMENTAL / Helper to develop stylesheet in What You See Is What You Get");
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
	case PLG_ICON:
		return QPixmap( ":/xslgui/images/xslgui.png" );
	case PLG_EMAIL:
		return "ulrich.vdh@shadoware.org";
	case PLG_WEBSITE:
		return "http://www.shadoware.org";
	case PLG_VERSION:
		return "0.1";
	case PLG_LICENCE:
		return "GPL v2.0 or later";
	}
	return QVariant();
}

QList<IFileTypePlugin*> XslGuiPlugin::fileTypes() {
	return m_fileTypes;
}

Q_EXPORT_PLUGIN2(xslgui, XslGuiPlugin)
