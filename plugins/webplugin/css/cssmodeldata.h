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

#ifndef CSSMODELDATA_H_
#define CSSMODELDATA_H_

// Xinx header
#include <contentview/contentviewparser.h>

// Qt header
#include <QApplication>

class CSSFileContentParser : public ContentViewParser {
	Q_DECLARE_TR_FUNCTIONS(CSSFileContentParser)
public:
	CSSFileContentParser( bool autoDelete = false );
	virtual ~CSSFileContentParser();

protected:
	virtual void loadFromDeviceImpl();

private:
	enum ParsingState {
		CssDefault,
		CssIdentifier
	};

	int m_line;

	ContentViewNode * attacheNewPropertyNode( ContentViewNode * parent, const QString & name, int line );
	ContentViewNode * attacheNewIdentifierNode( ContentViewNode * parent, const QString & name, int line );
	ContentViewNode * attacheNewClassNode( ContentViewNode * parent, const QString & name, int line );
	ContentViewNode * attacheNewTagNode( ContentViewNode * parent, const QString & name, int line );
	ContentViewNode * attacheNewIdNode( ContentViewNode * parent, const QString & name, int line );
};

#endif /*CSSMODELDATA_H_*/
