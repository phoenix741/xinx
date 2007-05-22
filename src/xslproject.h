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
#include <QDomElement>
#include <QStringList>

/*
TODO : 
Conf standard/Specifique
*/

class WebServices;
class WebServicesModel;

class XSLProject : public QObject {
	Q_OBJECT
public:
	enum enumProjectType { WEB = 0, SERVICES = 1 } ;
	enum enumProjectVersion { EGX500ES1 = 100, EGX500ES2 = 101, GCE110 = 110, GCE120 = 120 };

	XSLProject();
	XSLProject( const XSLProject & );
	XSLProject( const QString & );
	~XSLProject();
	
	void loadFromFile( const QString & );
	void saveToFile( const QString & = QString() );
	
	QString projectName() const;
	void setProjectName( const QString & );

	enumProjectType projectType() const;
	void setProjectType( const enumProjectType & );

	enumProjectVersion projectVersion() const;
	void setProjectVersion( const enumProjectVersion & );
	
	QString defaultLang() const;
	void setDefaultLang( const QString & );
	
	QString defaultNav() const;
	void setDefaultNav( const QString & );
	
	QString projectPath() const;
	void setProjectPath( const QString & );
	
	QString languePath() const;
	QString navPath() const;

	QString specifPath() const;
	void setSpecifPath( const QString & );
	
	QString specifPrefix() const;
	void setSpecifPrefix( const QString & );
	
	QString	standardConfigurationFile() const;
	void setStandardConfigurationFile( const QString & );
	
	QString	specifiqueConfigurationFile() const;
	void setSpecifiqueConfigurationFile( const QString & );

	QStringList & serveurWeb() { return m_webServiceLink; };

	QDomDocument & sessionDocument() { return m_sessionDocument; };
	QDomElement & sessionNode() { return m_sessionNode; };
	void clearSessionNode();
	QStringList & lastOpenedFile() { return m_lastOpenedFile; }

	const QString & fileName() const;
private:
	QString getValue( const QString & node ) const;
	void setValue( const QString & node, const QString & value );
	
	void loadSessionFile( const QString fileName );
	void saveSessionFile( const QString fileName );

	void loadWebServicesLink();
	void saveWebServicesLink();

	QString m_fileName;
	
	QDomDocument m_projectDocument;
	QDomDocument m_sessionDocument;
	QDomElement m_sessionNode, m_rootSession;
	
	QStringList m_webServiceLink, m_lastOpenedFile;
};

#endif
