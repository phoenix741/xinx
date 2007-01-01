/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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
 
#ifndef _XSLPROJECT_H_
#define _XSLPROJECT_H_

#include <QString>
#include <QDomDocument>

class XSLProject {
public:
	XSLProject();
	XSLProject(const XSLProject &);
	XSLProject(const QString &);
	~XSLProject();
	
	void loadFromFile(const QString &);
	void saveToFile(const QString & = QString());
	
	QString defaultLang() const;
	void setDefaultLang(const QString &);
	
	QString defaultNav() const;
	void setDefaultNav(const QString &);
	
	QString projectPath() const;
	void setProjectPath(const QString &);
	
	QString specifPath() const;
	void setSpecifPath(const QString &);
	
	QString specifPrefix() const;
	void setSpecifPrefix(const QString &);	
	
	const QString & fileName() const;
private:
	QString getValue( const QString & node ) const;
	void setValue( const QString & node, const QString & value );

	QString m_fileName;
	QDomDocument m_projectDocument;
};

#endif
