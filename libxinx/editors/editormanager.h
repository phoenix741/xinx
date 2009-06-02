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

/*!
 * \file editormanager.h
 * \brief This class represent the object that manage the list of editors.
 */

#ifndef _EDITORMANAGER_H_
#define _EDITORMANAGER_H_
#pragma once

// Qt header
#include <QString>

class AbstractEditor;

/*!
 * \class EditorManager
 * \brief Interface that represent the object used to manage the list of editors.
 * Interface used by plugins. The goal is to permit easily access of all existing
 * editors. This class is not a QObject class but the inherited class must emit
 * signal currentChanged(int) when the current editor changed.
 */
class EditorManager {
public:
	//! Create an EditorManager
	EditorManager();
	//! Destroy the class
	virtual ~EditorManager();

	//! Return the current editor
	virtual AbstractEditor * currentEditor() const = 0;
	//! Return the editor at index \e index
	virtual AbstractEditor * editor( int index ) const = 0;
	//! Return the editor of name \e filename
	virtual AbstractEditor * editor( const QString & filename ) const = 0;
	//! Return the list of editors
	virtual QList<AbstractEditor*> editors() const = 0;

	//! Show the next editor
	virtual void changeToNextEditor() = 0;
	//! Show the previous editor
	virtual void changeToPreviousEditor() = 0;
	//! Show to the editor \e index
	virtual void changeToEditor( int index ) = 0;

	//! Return the number of editor
	virtual int editorsCount() const = 0;

	//! Return the editor manager of the application
	static EditorManager * self();

	/*!
	 * \fn currentChanged( int index )
	 * This signal is called when the current editor is changed. This class doesn't
	 * declare the signal (because don't inherit of QObject) but child should.
	 */
//signals:
//	void currentChanged( int index );
private:
	static EditorManager * s_self;
};

#endif // _EDITORMANAGER_H_

