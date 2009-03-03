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

#ifndef _JSCONTENTVIEWPARSER_H_
#define _JSCONTENTVIEWPARSER_H_

// Xinx header
#include <contentview/contentviewparser.h>

// Qt header
#include <QApplication>

class QIODevice;

class JsContentViewParser : public ContentViewParser {
	Q_DECLARE_TR_FUNCTIONS(JsContentViewParser)
public:
	JsContentViewParser( bool autoDelete = false );
	virtual ~JsContentViewParser();

protected:
	virtual void loadFromDeviceImpl();
private:
	enum JAVASCRIPT_TOKEN { TOKEN_UNKNOWN, TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER, TOKEN_PONCTUATION, TOKEN_EOF };

	void nextIdentifier( QIODevice * device, enum JAVASCRIPT_TOKEN & symbType, QString & symbName );
	void loadVariables( ContentViewNode * parent, QIODevice * device );
	ContentViewNode * loadFunction( ContentViewNode * parent, QIODevice * device );
	void loadInstruction( QIODevice * buffer, QString & name, JAVASCRIPT_TOKEN & type );

	ContentViewNode * attacheNewParamNode( ContentViewNode * parent, const QString & name, int line );
	ContentViewNode * attacheNewVariableNode( ContentViewNode * parent, const QString & name, int line );
	ContentViewNode * attacheNewFunctionNode( ContentViewNode * parent, const QString & name, int line );

	int m_line;
};

#endif /* _JSCONTENTVIEWPARSER_H_ */