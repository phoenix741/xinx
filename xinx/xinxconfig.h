/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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
 
#ifndef XINXCONFIG_H
#define XINXCONFIG_H

#include "appsettings.h"

class PrivateXINXConfig;

class XINXConfig : public QObject, public AppSettings {
	Q_OBJECT
public:
	XINXConfig( const XINXConfig & origine );
	XINXConfig();
	virtual ~XINXConfig();
	
	static XINXConfig * self();
	virtual void load();
	virtual void save();

	void setXinxDataFiles( const QString & path );
	QString getTools( const QString & tool );

	XINXConfig& operator=(const XINXConfig& p);
signals:
	void changed();
protected:
	virtual struct_globals getDefaultGlobals();
	virtual struct_editor getDefaultEditor();
private:
	static XINXConfig * s_self;
	
	friend class PrivateXINXConfig;
	PrivateXINXConfig * d;
};

#endif