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

#ifndef _ABSTRACTFILEEDITOR_H_
#define _ABSTRACTFILEEDITOR_H_

// Xinx header
#include "abstracteditor.h"
#include "filewatcher.h"

// Qt header
#include <QPointer>

class AbstractFileEditor : public AbstractEditor {
	Q_OBJECT
	Q_PROPERTY( QString filename READ lastFileName WRITE setWatcher )
public:
	AbstractFileEditor( QWidget * parent = 0 );
	virtual ~AbstractFileEditor();

	virtual void loadFromFile( const QString & fileName = QString() );
	virtual void saveToFile( const QString & fileName = QString() );

	virtual bool isModified();

	const QString & lastFileName() const;
	virtual QString defaultFileName() const = 0;

	virtual void serialize( XSLProjectSessionEditor * data, bool content );
	virtual void deserialize( XSLProjectSessionEditor * data );
protected slots:
	virtual void setModified( bool isModified );
private slots:
	void fileChanged();
private:
	void desactivateWatcher();
	void activateWatcher();
	void setWatcher( const QString & path );

	bool m_isSaving, m_modified;
	QPointer<FileWatcher> m_watcher;
	QString m_lastFileName;
};

#endif //  _ABSTRACTFILEEDITOR_H_
