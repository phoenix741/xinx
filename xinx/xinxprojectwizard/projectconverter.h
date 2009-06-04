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

#ifndef PROJECTCONVERTER_H_
#define PROJECTCONVERTER_H_

// Qt header
#include <QObject>
#include <QDomDocument>

// Xinx header
#include <core/exceptions.h>

#define XINX_PROJECT_VERSION_0 0
#define XINX_PROJECT_VERSION_1 1
#define XINX_PROJECT_VERSION_2 2
#define XINX_PROJECT_VERSION_3 3
#define XINX_PROJECT_VERSION_4 4
#define XINX_PROJECT_VERSION 4

class ProjectConverter : public QObject {
	Q_OBJECT
public:
	ProjectConverter( const QString & filename );
	virtual ~ProjectConverter();

	int version() const;
	int nbSession() const;
	QString type() const;

	void process();
	void save();
signals:
	void setMaximum( int value );
	void setValue( int value );
private:
	QDomDocument m_projectDocument;
	QDomDocument m_sessionDocument;

	QString m_filename, m_type;
	int m_version, m_nbSession;
};

#endif /*PROJECTCONVERTER_H_*/
