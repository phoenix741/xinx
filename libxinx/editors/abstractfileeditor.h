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

#ifndef _ABSTRACTFILEEDITOR_H_
#define _ABSTRACTFILEEDITOR_H_

// Xinx header
#include "core/filewatcher.h"
#include "editors/abstracteditor.h"

// Qt header
#include <QPointer>

/*!
 * This class is an editor that can read and write from and to a file. This class
 * open the file (with the correct option) and call loadFromDevice and saveTofDevice.
 */
class AbstractFileEditor : public AbstractEditor {
	Q_OBJECT
	Q_PROPERTY( QString filename READ lastFileName WRITE setWatcher )
public:
	/*!
	 * Constructor of the abstract file editor.
	 * \param parent Parent of the editor
	 */
	AbstractFileEditor( QWidget * parent = 0 );
	//! Destroy the editor
	virtual ~AbstractFileEditor();

	/*!
	 * Return the title of the FileEditor. The title is \e noname if \e getFileName() is Empty
	 * else return the File name (without the path).
	 * \return The title to use with Tab name
	 * \sa getFileName(), hasName()
	 */
	virtual QString getTitle() const;

	virtual QString getLongTitle() const;

	/*! Open and load from the file \e fileName */
	virtual void loadFromFile( const QString & fileName = QString() );
	/*! Open and save to file \e fileName */
	virtual void saveToFile( const QString & fileName = QString() );

	virtual bool isModified();

	/*! Return the last name used with \e loadFromFile() or \e saveToFile() */
	const QString & lastFileName() const;
	/*! Return the name used if no name is defined (ie. noname.txt) */
	virtual QString defaultFileName() const = 0;

	virtual void serialize( XinxProjectSessionEditor * data, bool content );
	virtual void deserialize( XinxProjectSessionEditor * data );
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
