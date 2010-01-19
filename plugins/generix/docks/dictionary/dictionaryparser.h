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

#ifndef DICTIONARYPARSER_H
#define DICTIONARYPARSER_H
#pragma once

// Xinx header
#include <contentview/contentviewparser.h>

// Qt header
#include <QXmlStreamReader>

class DictionaryParser : public ContentViewParser, private QXmlStreamReader {
	Q_DECLARE_TR_FUNCTIONS(DictionaryParser)
public:
	DictionaryParser( bool autoDelete = false );
	virtual ~DictionaryParser();

	QTextCodec * codec() { return m_codec; }

	QString trad( const QString & text, const QString & lang ) const;
	QString trad( const QString & text, const QString & lang, const QString & ctxt ) const;

	void setFileList( const QStringList & files );
	virtual void setFilename( const QString & filename );
protected:
	virtual void loadFromDeviceImpl();
private:
	void readRootNode();
	void readLabelsNode();
	void readLabelNode( ContentViewNode * parent );

	QTextCodec * m_codec;
	QStringList m_files;
	ContentViewNode * m_rootNode;
};

#endif // DICTIONARYPARSER_H
