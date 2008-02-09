/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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

#ifndef __P_WEBSERVICESEDITOR_H__
#define __P_WEBSERVICESEDITOR_H__
 
// Xinx header
#include "texteditor.h"
#include "webserviceseditor.h"
#include "webservices.h"
#include "xslproject.h"
#include "xinxconfig.h"
#include "exceptions.h"

// Qt header
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QDomDocument>

class PrivateWebServicesEditor : public QObject {
	Q_OBJECT
public:
	PrivateWebServicesEditor( WebServicesEditor * parent );
	~PrivateWebServicesEditor();
	
	void store( const QString & );
	void restore( const QString & );

	QString m_serviceName, m_operationName, m_oldParamValue;
	QComboBox * m_servicesList, * m_paramList, * m_actionList;
	QHash<QString,QString> m_paramValues;
	bool m_isModified;

public slots:
	void webServicesChanged();
	void webServicesActivated( int );
	void webServicesParamActivated( int );
	void webServicesValueActivated();

public:
	void loadServicesList();
	void loadActionsList( int index );
	void loadValuesList( int index );

private:
	WebServicesEditor * m_parent;
};

#endif // __P_WEBSERVICESEDITOR_H__
