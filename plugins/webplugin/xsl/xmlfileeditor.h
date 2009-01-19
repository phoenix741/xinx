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

#ifndef _XMLFILEEDITOR_H_
#define _XMLFILEEDITOR_H_

// Xinx header
#include <textfileeditor.h>

class XmlFileEditor : public TextFileEditor {
	Q_OBJECT
public:
	XmlFileEditor( QWidget *parent = 0 );
	virtual ~XmlFileEditor();

	virtual QString defaultFileName() const;

	virtual bool autoIndent();

	virtual QIcon icon() const;
	virtual QTextCodec * codec() const;

	virtual void loadFromDevice( QIODevice & d );
private:
	QTextCodec * m_codec;
};

#endif //_XMLFILEEDITOR_H_
