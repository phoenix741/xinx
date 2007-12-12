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

// Xinx header
#include "editorthreadxsl.h"
#include "xsllistview.h"

/* PrivateEditorThreadXSL */

class PrivateEditorThreadXSL {
public:
	PrivateEditorThreadXSL( EditorThreadXSL * parent );
	~PrivateEditorThreadXSL();
	
private:
	EditorThreadXSL * m_parent;
};

PrivateEditorThreadXSL::PrivateEditorThreadXSL( EditorThreadXSL * parent ) : m_parent( parent ) {
	
}

PrivateEditorThreadXSL::~PrivateEditorThreadXSL() {
	
}

/* EditorThreadXSL */

EditorThreadXSL::EditorThreadXSL( QObject * parent ) : EditorThread( parent ) {
	d = new PrivateEditorThreadXSL( this );
}

EditorThreadXSL::~EditorThreadXSL() {
	delete d;
}

void EditorThreadXSL::threadrun() {
	parser() = new XSLFileContentParser();
	
	EditorThread::threadrun();
	
	delete parser(); parser() = 0;
}