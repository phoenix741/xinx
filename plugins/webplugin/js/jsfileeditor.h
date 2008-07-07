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

#ifndef _JSFILEEDITOR_H_
#define _JSFILEEDITOR_H_

// Xinx header
#include <itemmodelfileeditor.h>
#include "javascriptmodelcompleter.h"

class JSFileEditor : public ItemModelFileEditor {
	Q_OBJECT
public:
	JSFileEditor( QWidget *parent = 0 );
	virtual ~JSFileEditor();

	virtual QString defaultFileName() const;
	virtual QIcon icon() const;
	JavascriptModelCompleter * completionModel() const;
private:
	JavaScriptParser * m_parser;
	JavascriptModelCompleter * m_completionModel;
};

#endif //_JSFILEEDITOR_H_
