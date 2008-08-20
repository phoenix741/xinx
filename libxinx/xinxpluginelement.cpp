/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Xinx header
#include "xinxpluginelement.h"

XinxPluginElement::XinxPluginElement( QObject * plugin, bool isStatic ) : m_plugin( plugin ), m_isStatic( isStatic ) {
}

QObject * XinxPluginElement::plugin() const {
	return m_plugin;
}

bool XinxPluginElement::isModifiable() const {
	return !m_isStatic;
}

bool XinxPluginElement::isConfigurable() const {
	return qobject_cast<IXinxPluginConfiguration*>( m_plugin );
}

QPixmap XinxPluginElement::pixmap() const {
	IXinxPlugin * plugin = qobject_cast<IXinxPlugin*>( m_plugin );
	if( plugin->getPluginAttribute( IXinxPlugin::PLG_ICON ).isValid() )
		return plugin->getPluginAttribute( IXinxPlugin::PLG_ICON ).value<QPixmap>();
	else
		return PluginElement::pixmap();
}

QString XinxPluginElement::name() const {
	IXinxPlugin * plugin = qobject_cast<IXinxPlugin*>( m_plugin );
	return plugin->getPluginAttribute( IXinxPlugin::PLG_NAME ).toString();
}

QString XinxPluginElement::description() const {
	IXinxPlugin * plugin = qobject_cast<IXinxPlugin*>( m_plugin );
	return plugin->getPluginAttribute( IXinxPlugin::PLG_DESCRIPTION ).toString();
}
