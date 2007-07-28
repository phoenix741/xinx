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

/*!
 * Exception throw by XSLProject when the project can't be opened or saved.
 * The exception contains the message send to the user.
 * 
 * The exception is used in internal for the session. If session file can't be 
 * read, the exception is catched and the application continue. The project is
 * opened. 
 */ 
class XSLProjectException {
public:
	/*!
	 * Constructor of the exception.
	 * \param message Message for the user.
	 */
	XSLProjectException( const QString & message );
	/*!
	 * Method to read the message in the exception.
	 * \return a string contains the error.
	 */
	const QString & getMessage() const; 
private:
	QString m_message;
};

class WebServices;
class WebServicesModel;
class PrivateXSLProject;

/*!
 * Represent a project in XINX.
 * A project specify where the user can find path to stylesheet, and javascript,
 * and where the application must store project file.
 * \todo Read Configuration file and manage standard/specifique
 *
 * \throw XSLProjectException When the application can't read a project file or can't
 * save it.
 */
class XSLProject : public QObject {
	Q_OBJECT
public:
	/*!
	 * XINX can manage two type of project, WebServices project and Web application.
	 * In case of WebServices, the function serveurWeb() returns WSDL files, else she
	 * must return null.
	 * \sa projectType(), setProjectType()
	 */
	enum enumProjectType { 
		/// Type used when the file is a web application (like btoe, btob, ...)
		WEB = 0, 
		/// Type used when the file is a web services.
		SERVICES = 1 
	} ;
	/*!
	 * This is the different version managed by XINX. 
	 * At this time the version don't change anything in the application. All is version
	 * independant.
	 * \todo Read the version from the Generix configuration.
	 * \todo The version musn't be constant, but must be more flexible.
	 * \sa projectVersion(), setProjectVersion()
	 */
	enum enumProjectVersion { EGX500ES1 = 100, EGX500ES2 = 101, GCE110 = 110, GCE120 = 120, GCE130 = 130, GCE140 = 140 };
	/*!
	 * The different Concurent file repository managed by XINX. 
	 * At this time only CVS is partially managed.
	 */
	enum enumProjectRCS { 
		/// The Concurent file repository is not managed.
		NORCS = 0, 
		/// CVS is used.
		CVS = 1, 
		/// Subversion is used (XINX don't care at the moment of this choice).
		SUBVERSION = 2
	};

	/*!
	 * Create an empty project.
	 */
	XSLProject();
	/*!
	 * Create a project with another project. This is the copy constructor used in
	 * assignation.
	 */
	XSLProject( const XSLProject & );
	/*!
	 * Create a project and read the content of the file, to initalize variables.
	 * \param filename The name of the file to be read.
	 * \sa loadFromFile()
	 * \throw XSLProjectException When the application can't read the project file.
	 */
	XSLProject( const QString & filename );
	/*!
	 * Destroy the project
	 */
	~XSLProject();
	
	/*!
	 * Read the content of the file in the XSLProject structure.
	 * \param filename Name of file (in XML format) where the XSLProject
	 * is serialized.
	 * \sa saveToFile()
	 * \throw XSLProjectException When the application can't read the project file.
	 */
	void loadFromFile( const QString & filename );
	/*!
	 * Save the XSLProject structure in a file. If the file name is not specified
	 * the project is saved in the file used to open the project.
	 * The project is serialized in a XML file.
	 * \param filename The file where we save the project. 
	 * \sa loadFromFile()
	 * \throw XSLProjectException When the application can't save the project file.
	 */
	void saveToFile( const QString & filename = QString() );
	
	/*!
	 * Name of the project. The name is just an information, nothing else.
	 * \return The name of the project.
	 * \sa setProjectName();
	 */
	QString projectName() const;
	/*!
	 * Set the name of the project. 
	 * \param value The new name of the project.
	 * \sa projectName()
	 */
	void setProjectName( const QString & value );

	/*!
	 * Get the project type ported by the XSLProject. The project can be 
	 * a WebServices project or a Web project.
	 * \return the project type.
	 * \sa projectType();
	 */
	enumProjectType projectType() const;
	/*!
	 * Set the project type.
	 * \param value The new type.
	 * \sa setProjectType()
	 */
	void setProjectType( const enumProjectType & value );
	
	/*!
	 * Get the project version used by XINX. In the moment it's just for information.
	 * The information isn't used by XINX.
	 * \return The project version.
	 * \sa setProjectVersion()
	 */
	enumProjectVersion projectVersion() const;
	/*!
	 * Set the project version used by XINX.
	 * \param value The new version.
	 * \sa projectVersion()
	 */
	void setProjectVersion( const enumProjectVersion & value );
	
	/*!
	 * The concurent file system used by XINX (only CVS can be used).
	 * \return Return the number version. 
	 * \sa setProjectRCS()
	 */
	enumProjectRCS projectRCS() const;
	/*!
	 * Set the concurent file system used by XINX.
	 * \param value The new system used.
	 * \sa projectRCS()
	 */
	void setProjectRCS( const enumProjectRCS & value );
	
	/*!
	 * Langue used by Generix. Used to determine the project path.
	 * \todo How the langue is managed on GCE130 ? and GCE140 ? with the new traduction
	 * method.
	 * \return The lang used by the project.
	 * \sa setDefaultLang()
	 */
	QString defaultLang() const;
	/*!
	 * Set the langue used by Generix.
	 * \param value The new value for the langue.
	 * \sa defaultLang()
	 */
	void setDefaultLang( const QString & value );
	
	/*!
	 * Get the navigator defined as defined in the configuration file.
	 * This is used for information, this not used by XINX.
	 * \return Return the default navigator.
	 * \sa setDefaultNav()
	 */
	QString defaultNav() const;
	/*!
	 * Get the default navigator.
	 * \param value The navigator saved in XINX.
	 * \sa defaultNav()
	 */
	void setDefaultNav( const QString & value );
	
	/*!
	 * Get the project path. The project path is the path where the configuration file
	 * is positioned.
	 * \return the path of the project.
	 * \sa setProjectPath(), languePath(), navPath(), languesPath(), specifPath(), setSpecifPath()
	 */
	QString projectPath() const;
	/*!
	 * Set the project path.
	 * \param value Value of the project path
	 * \sa projectPath(), languePath(), navPath(), languesPath(), specifPath(), setSpecifPath()
	 */
	void setProjectPath( const QString & value );
	
	/*!
	 * Get the project path with the langues directory (as <project>/langue).
	 * \return the path
	 * \sa projectPath(), setProjectPath(), navPath(), languePath(), specifPath(), setSpecifPath()
	 */
	QString languesPath() const;
	/*!
	 * Get the project path with the langue directory (as <project>/langue/<langue>)
	 * \return the path
	 * \sa projectPath(), setProjectPath(), languePath(), navPath(), languesPath(), specifPath(), setSpecifPath()
	 */
	QString languePath() const;
	/*!
	 * Get the project path with the langue directory and nav directory (as <project>/langue/<langue>/nav)
	 * \return the path
	 * \sa projectPath(), setProjectPath(), languePath(), languesPath(), specifPath(), setSpecifPath()
	 */
	QString navPath() const;

	/*!
	 * Get the specifique project path (as <project>/langue/<langue>/nav/projet)
	 * \return the path
	 * \sa projectPath(), setProjectPath(), languePath(), navPath(), languesPath(), setSpecifPath()
	 */
	QString specifPath() const;
	/*!
	 * Set the specifique project path (as <project>/langue/<langue>)
	 * \param value The specifique path
	 * \sa projectPath(), setProjectPath(), languePath(), navPath(), languesPath(), specifPath()
	 */
	void setSpecifPath( const QString & value );
	
	/*!
	 * Get the specifique prefix.
	 * \return the specifique prefix.
	 * \sa setSpecifPrefix()
	 */
	QString specifPrefix() const;
	/*!
	 * Set the specifique prefix.
	 * \param value Value of prefix.
	 * \sa setSpecifPrefix()
	 */
	void setSpecifPrefix( const QString & value );
	
	/*!
	 * List of Web services link. The link must point to the WSDL.
	 * \return list of WSDL link.
	 */
	QStringList & serveurWeb();

	/*!
	 * The session document is an XML document contains the sessions.
	 * \return the XML document.
	 * \sa sessionNode(), clearSessionNode()
	 */
	QDomDocument & sessionDocument();
	/*!
	 * The session node is the node where the session is stored.
	 * \return the node where the session is stored.
	 * \sa sessionDocument(), clearSessionNode()
	 */
	QDomElement & sessionNode();
	/*!
	 * Clear all sessions.
	 * \sa sessionDocument(), sessionNode()
	 */
	void clearSessionNode();
	/*!
	 * List all the last opend files in the project.
	 * \return the list of the last opend file.
	 */
	QStringList & lastOpenedFile();
	/*!
	 * Return the file name where the project is stored.
	 * \return the file name.
	 */
	const QString & fileName() const;
private:
	PrivateXSLProject * d;
	friend class PrivateXSLProject;
};

#endif
