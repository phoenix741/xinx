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

#ifndef __WEBSERVICESEDITOR_H__
#define __WEBSERVICESEDITOR_H__

// Xinx header
#include <editors/textfileeditor.h>

// Qt header
#include <QHash>
#include <QString>

class WebServices;
class Operation;
class QComboBox;
class QToolButton;

class WebServicesEditor : public TextFileEditor {
	Q_OBJECT
public:
	WebServicesEditor( QWidget *parent = 0 );
	~WebServicesEditor();

	WebServices * service();
	Operation * operation();
	const QHash<QString,QString> & values();

	virtual void loadFromDevice( QIODevice & d );
	virtual void saveToDevice( QIODevice & d );

	virtual void serialize( XinxProjectSessionEditor * data, bool content );
	virtual void deserialize( XinxProjectSessionEditor * data );

	virtual bool autoIndent();

	virtual QIcon icon() const;
	virtual QString defaultFileName() const;
private slots:
	void paramListEditingFinished();
	void updateActions();

	void webServicesChanged();
	void webServicesActivated( int );
	void webServicesParamActivated( int );
	void webServicesValueActivated();

	void run();
private:
	void loadServicesList();
	void loadActionsList( int index );
	void loadValuesList( int index );

private:
	void store( const QString & );
	void restore( const QString & );

	QString m_serviceName, m_operationName, m_oldParamValue;
	QComboBox * m_servicesList, * m_paramList, * m_actionList;
	QToolButton * m_updateButton, * m_runButton;
	QHash<QString,QString> m_paramValues;
};

#endif // __WEBSERVICESEDITOR_H__
