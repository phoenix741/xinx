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

#ifndef __JSFILEEDITOR_H__
#define __JSFILEEDITOR_H__

#include "fileeditor.h"

class PrivateJSFileEditor;

/*!
 * Editor for file of type JavaScript. This editor use the JSEditor text editor
 * to edit files.
 */
class JSFileEditor : public FileEditor {
	Q_OBJECT
public:
	/*!
	 * Construct the javascript file editor.
	 * \param parent Parent of the Javascript File Editor 
	 */
	JSFileEditor( QWidget *parent = 0 );
	/*!
	 * Destroy the javascript file editor.
	 */
	virtual ~JSFileEditor();
	
	virtual QString getSuffix() const;
	virtual QIcon icon();
private:
	PrivateJSFileEditor * d;
	friend class PrivateJSFileEditor;
};

#endif // __JSFILEEDITOR_H__
