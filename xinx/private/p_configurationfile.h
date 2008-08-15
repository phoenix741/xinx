/*************************'{}'**************************************************
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

#ifndef __P_CONFIGURATIONFILE_H__
#define __P_CONFIGURATIONFILE_H__

// Qt header
#include <QXmlStreamReader>
#include <QIODevice>
#include <QString>

class ConfigurationBusinessView {
public:
	QString target,
			viewstruct,
			type,
			fileRef,
			lang,
			support;
};

class ConfigurationParser : public QXmlStreamReader {
public:
	ConfigurationParser( bool minimal = true );

	const QString & version() const { return m_version; };
	int build() const { return m_build; };
	const QString & xmlPresentationFile() const { return m_xmlPresentationFile; };

	bool loadFromDevice( QIODevice * device );
private: // Lecture de nouveau éléments à l'aide de Plugins.
	void readUnknownElement();
	void readConfigElement();
	void readVersionElement();
	void readApplicationElement();

	void readBusinessViewDef();

	void readPresentation();
	void readPresentationElement();

	bool m_minimal;
	QString m_version, m_xmlPresentationFile;
	int m_build, m_elementToRead;

	QHash<QString,ConfigurationBusinessView> m_configurations;
};

#endif // __P_CONFIGURATIONFILE_H__
