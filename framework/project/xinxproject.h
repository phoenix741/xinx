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

#ifndef _XINXPROJECT_H_
#define _XINXPROJECT_H_
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>

// Qt header
#include <QString>
#include <QStringList>
#include <QPair>
#include <QVariant>

/* Constante */

#define XINX_PROJECT_VERSION_0 0
#define XINX_PROJECT_VERSION 0

#define XINX_PROJECT_EXTENTION ".xinx_project"
#define XINX_SESSION_EXTENTION ".xinx_session"

/* Classes */

class QDomDocument;
class QDomElement;

/*!
 * Exception throw by XinxProject when the project can't be opened or saved.
 * The exception contains the message send to the user.
 *
 * The exception is used in internal for the session. If session file can't be
 * read, the exception is catched and the application continue. The project is
 * opened.
 */
class LIBEXPORT XinxProjectException : public XinxException {
public:
	/*!
	 * Constructor of the exception.
	 * \param message Message for the user.
	 * \param wizard Show a wizard to migrate the project
	 */
	XinxProjectException( const QString & message, bool wizard = false );

	//! True if XINX must start "xinxprojectwizard"
	bool startWizard() const;
private:
	bool m_wizard;
};

class XinxProjectSession;

/*!
 * Represent a session editor A session is an hash table of properties.
 * A property is a couple :
 *  - key   : QString
 *  - value : QVariant
 *
 * The value is a QVariant but must be writtable in a text file. (QString, int, ...)
 */
class LIBEXPORT XinxProjectSessionEditor : public QObject {
	Q_OBJECT
public:
	/*! Create a session editor with a session file as \e parent. */
	XinxProjectSessionEditor( XinxProjectSession * parent = 0 );
	/*! Destroy the session editor */
	~XinxProjectSessionEditor();

	/*!
	 * Load the session from a DOM Element.
	 * \param element The DOM Element to read
	 */
	void loadFromNode( const QDomElement & element );
	/*!
	 * Save the session to a DOM Element.
	 * \param document The document used to create new QDomElement.
	 * \param element The DOM Element to create.
	 */
	void saveToNode( QDomDocument & document, QDomElement & element );

	/*! Store the property \e key with the \e value in the session. */
	void writeProperty( const QString & key, QVariant value );
	/*! Read the property \e key */
	QVariant readProperty( const QString & key );

	/*! Project path, used to have relative path */
	QString projectPath() const;
private:
	QHash<QString,QVariant> m_properties;
	XinxProjectSession * m_parent;
};

class XinxProject;

/*!
 * An XinxProjectSession represents the .session file associate to the project and who contains
 * tempory data as last opened file, current opened file. This file is extern of project file,
 * to permit project file to be saved in Revision Control System.
 */
class LIBEXPORT XinxProjectSession : public QObject {
	Q_OBJECT
	Q_PROPERTY( QStringList lastOpenedFile READ lastOpenedFile )
public:
	/*! Create an empty session */
	XinxProjectSession( XinxProject * project );
	/*!
	 * Create a session object and load the file \e filename
	 * \throw XinxProjectException When the application can't read the session file.
	 * \sa loadFromFile
	 */
	XinxProjectSession( XinxProject * project, const QString & filename );
	/*! Destroy the session */
	virtual ~XinxProjectSession();

	/*!
	 * Load the file \e filename
	 * \sa saveToFile
	 * \throw XinxProjectException When the application can't read the session file.
	 */
	void loadFromFile( const QString & filename );

	/*!
	 * Save the session in the file \e filename, or in the loaded file if possible
	 * \sa loadFromFile
	 * \throw XinxProjectException When the application can't save the session file.
	 */
	void saveToFile( const QString & filename = QString() );

	/*!
	 * List all the last opend files in the project.
	 * \return the list of the last opend file.
	 */
	QStringList & lastOpenedFile();

	/*! List of serialized editor. */
	QList<XinxProjectSessionEditor*> & serializedEditors();

	/*! The project of the session */
	XinxProject * project() const;
private:
	XinxProject * m_project;
	QString m_filename;
	QStringList m_lastOpenedFile;
	QList<XinxProjectSessionEditor*> m_sessions;

	friend class XinxProjectSessionEditor;
};

class WebServices;
class WebServicesModel;
class PrivateXinxProject;
class ContentViewCache;

/*!
 * Represent a project in XINX.
 * A project specify where the user can find path to stylesheet, and javascript,
 * and where the application must store project file.
 * \todo Read Configuration file and manage standard/specifique
 *
 * \throw XinxProjectException When the application can't read a project file or can't
 * save it.
 */
class LIBEXPORT XinxProject : public QObject {
	Q_OBJECT
	Q_PROPERTY( QString fileName READ fileName )
	Q_PROPERTY( XinxProjectSession* session READ session )
	Q_PROPERTY( QString projectName READ projectName WRITE setProjectName )
	Q_PROPERTY( QString projectRCS READ projectRCS WRITE setProjectRCS )
	Q_PROPERTY( QString projectPath READ projectPath WRITE setProjectPath )
public:
	/*! Create an empty project. */
	XinxProject();
	/*!
	 * Create a project with another project. This is the copy constructor used in
	 * assignation.
	 */
	XinxProject( const XinxProject & );
	/*!
	 * Create a project and read the content of the file, to initalize variables.
	 * \param filename The name of the file to be read.
	 * \sa loadFromFile()
	 * \throw XinxProjectException When the application can't read the project file.
	 */
	XinxProject( const QString & filename );
	/*! Destroy the project */
	~XinxProject();

	/*! Assigned operator to copy object contents at assignation */
	XinxProject& operator=( const XinxProject& p );

	/*!
	 * Read the content of the file in the XinxProject structure.
	 * \param filename Name of file (in XML format) where the XinxProject
	 * is serialized.
	 * \sa saveToFile()
	 * \throw XinxProjectException When the application can't read the project file.
	 */
	void loadFromFile( const QString & filename );
	/*!
	 * Save the XinxProject structure in a file. If the file name is not specified
	 * the project is saved in the file used to open the project.
	 * The project is serialized in a XML file.
	 * \param filename The file where we save the project.
	 * \sa loadFromFile()
	 * \throw XinxProjectException When the application can't save the project file.
	 */
	void saveToFile( const QString & filename = QString() );

	/*!
	 * Return the file name where the project is stored.
	 * \return the file name.
	 */
	const QString & fileName() const;

	/*! Return the session object used to store session information. */
	XinxProjectSession * session() const;

	/*!
	 * Save the session in the file.
	 * \throw XinxProjectException when the application can't save the session file.
	 * \sa loadFromFile(), saveToFile()
	 */
	void saveOnlySession();

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
	 * The concurent file system used by XINX.
	 * \return Return the name of the RCS.
	 * \sa setProjectRCS()
	 */
	const QString & projectRCS() const;
	/*!
	 * Set the concurent file system used by XINX.
	 * \param value The new system used.
	 * \sa projectRCS()
	 */
	void setProjectRCS( const QString & value );

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

	/*! List of file to preload at XINX start */
	QStringList & preloadedFiles();

	/*! Content the cache of preloaded files */
	ContentViewCache * filesCache();

	/*! Preload file that can be used later */
	void preloadFilesCache();

	/*! Activated plugin for the project (the plugin must control this it-self, it's not a XINX control) */
	QStringList activatedPlugin() const;

	/*! Set activated plugin for the project */
	void setActivatedPlugin( const QStringList & value );
public slots:
	/*!
	 * Write a property in the project file (used by the plugin).
	 * \param key The property to write
	 * \param value The value to write
	 */
	void writeProperty( const QString & key, QVariant value );
	/*!
	 * Read a written property from the project file (used by the plugin).
	 * \param key The Property to read
	 * \return The value of the property
	 */
	QVariant readProperty( const QString & key ) const;
signals:
	/*! Emited when a property has changed. */
	void changed();
private:
	PrivateXinxProject * d;
	friend class PrivateXinxProject;
};

/*!
 * The XINX Project Manager is used to manage \e XinxProject file. One project
 * only by process is authorized.
 */
class LIBEXPORT XINXProjectManager : public QObject {
	Q_OBJECT
public:
	/*! Destroy the manager */
	virtual ~XINXProjectManager();

	/*! Create the manager if necessary, else return the singleton */
	static XINXProjectManager * self();

	/*! Change the current projet to \e project */
	void setCurrentProject( XinxProject * project );
	/*! Return the project */
	XinxProject * project() const;

	/*! Delete the project */
	void deleteProject();
signals:
	/*! Signal emited when the project is changed or when the project emit \e changed() */
	void changed();
private:
	/*! Create the XINX Project Manager */
	XINXProjectManager();

	XinxProject * m_project;
	static XINXProjectManager * s_self;
};

#endif /* _XINXPROJECT_H_ */
