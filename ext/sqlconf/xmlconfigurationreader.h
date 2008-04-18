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

#ifndef XMLCONFIGURATIONREADER_H_
#define XMLCONFIGURATIONREADER_H_

#include <QXmlStreamReader>

class XmlConfigurationReader : public QObject, public QXmlStreamReader {
	Q_OBJECT
public:
	XmlConfigurationReader();
	virtual ~XmlConfigurationReader();
	
	bool read( QIODevice * device );
signals:
	void findElement( const QString & element );
private:
	void readConfiguration();
	void readVersion();
	void readApplication();
	void readDocumentRowDef();
	void readDocumentDef();
	void readViewDef();
	void readViewStructDef();
	void readBusinessviewDef();
	void readPresentation();

	void readUnknownElement();
};

#endif /*XMLCONFIGURATIONREADER_H_*/
