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

#ifndef _CSSFILEEDITOR_H_
#define _CSSFILEEDITOR_H_

// Xinx header
#include <itemmodelfileeditor.h>

class CSSFileContentParser;

class CSSFileEditor : public ItemModelFileEditor {
	Q_OBJECT
public:
	CSSFileEditor( QWidget *parent = 0 );
	virtual ~CSSFileEditor();

	virtual QString defaultFileName() const;
	virtual QIcon icon() const;
private:
	CSSFileContentParser * m_parser;
};

#endif //_JSFILEEDITOR_H_
