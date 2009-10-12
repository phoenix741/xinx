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
#include "xmlpres.h"

// Qt header
#include <QString>
#include <QVariant>
#include <QTranslator>
#include <QApplication>

/* XmlPresPlugin */

XmlPresPlugin::XmlPresPlugin() : m_dock( 0 ) {
		Q_INIT_RESOURCE(xmlpres);
}

XmlPresPlugin::~XmlPresPlugin() {
}

bool XmlPresPlugin::initializePlugin( const QString & lang ) {
	QTranslator * tranlator = new QTranslator( this );
	tranlator->load( QString(":/xmlpres/translations/xmlpres_%1").arg( lang ) );
	qApp->installTranslator(tranlator);

	return true;
}

QVariant XmlPresPlugin::getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) {
	switch( attr ) {
	case PLG_NAME:
		return tr("Xml Pres Plugin");
	case PLG_DESCRIPTION:
		return tr("This plugin manage a dock for Xml Pres");
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
    case PLG_ICON:
		return QPixmap( ":/xmlpres/images/xmlpres.png" );
	case PLG_EMAIL:
		return "ulrich.vdh@shadoware.org";
	case PLG_WEBSITE:
		return "http://www.shadoware.org";
	case PLG_VERSION:
		return "1.0.0.1";
	case PLG_LICENCE:
		return "GPL v2.0 or later";
	}
	return QVariant();
}

QDockWidget * XmlPresPlugin::createDockWidget( QWidget * parent ) {
	if( ! m_dock ) {
		m_dock = new XmlPresentationDockWidget( parent );
		m_dock->setObjectName( QString::fromUtf8( "m_xmlPresDock" ) );
	}
	if( ( m_dock->parent() != parent ) && ( parent != 0 ) ) {
		m_dock->setParent( parent );
	}
	return m_dock;
}

XmlPresentationDockWidget * XmlPresPlugin::dock() {
	return static_cast<XmlPresentationDockWidget*>( createDockWidget( 0 ) );
}

Q_EXPORT_PLUGIN2(xmlpresplugin, XmlPresPlugin)
