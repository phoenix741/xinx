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

#ifndef XMLPRESPLUGIN_H_
#define XMLPRESPLUGIN_H_

// Xinx header
#include <plugins/plugininterfaces.h>
#include "xmlpresentationdockwidget.h"

class XmlPresPlugin : public QObject, public IDockPlugin {
	Q_OBJECT
	Q_INTERFACES(IXinxPlugin)
	Q_INTERFACES(IDockPlugin)
public:
	XmlPresPlugin();
	virtual ~XmlPresPlugin();

	virtual bool initializePlugin( const QString & lang );
	virtual QVariant getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr );

	virtual QDockWidget * createDockWidget( QWidget * parent );

	XmlPresentationDockWidget * dock();
private:
	XmlPresentationDockWidget * m_dock;
};

#endif /* XMLPRESPLUGIN_H_*/
