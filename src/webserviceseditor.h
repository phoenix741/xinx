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

#ifndef __WEBSERVICESEDITOR_H__
#define __WEBSERVICESEDITOR_H__

#include "fileeditor.h"

#include <QHash>
#include <QString>

class WebServices;
class Operation;
class QComboBox;

class PrivateWebServicesEditor;

class WebServicesEditor : public FileEditor {
	Q_OBJECT
public:
	WebServicesEditor( QWidget *parent = 0 );
	~WebServicesEditor();

	virtual QString getSuffix() const;
	
	WebServices * service();
	Operation * operation();
	const QHash<QString,QString> & values() const;
	
	virtual void loadFile( const QString &fileName = "" );
	virtual bool saveFile( const QString &fileName = "" );

	virtual void serialize( XSLProjectSessionEditor * data, bool content );
	virtual void deserialize( XSLProjectSessionEditor * data );

	virtual void setModified( bool modified );
	virtual bool isModified();

	virtual QIcon icon();
private:
	PrivateWebServicesEditor * d;
	friend class PrivateWebServicesEditor;
};

#endif // __WEBSERVICESEDITOR_H__
