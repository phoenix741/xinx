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

// Xinx header
#include "exceptions.h"

// Qt header
#include <QString>
#include <QDomDocument>
#include <QDomElement>
#include <QStringList>
#include <QPair>
#include <QVariant>

/*!
 * Exception throw by XSLProject when the project can't be opened or saved.
 * The exception contains the message send to the user.
 *
 * The exception is used in internal for the session. If session file can't be
 * read, the exception is catched and the application continue. The project is
 * opened.
 */
class XSLProjectException : public XinxException {
public:
	/*!
	 * Constructor of the exception.
	 * \param message Message for the user.
	 * \param wizard Show a wizard to migrate the project
	 */
	XSLProjectException( const QString & message, bool wizard = false );

	//! True if XINX must start "xinxprojectwizard"
	bool startWizard() const;
private:
	bool m_wizard;
};

class XSLProjectSession;

/*!
 * Represent a session editor A session is an hash table of properties.
 * A property is a couple :
 *  - key   : QString
 *  - value : QVariant
 *
 * The value is a QVariant but must be writtable in a text file. (QString, int, ...)
 */
class XSLProjectSessionEditor : public QObject {
	Q_OBJECT
public:
	/*! Create a session editor with a session file as \e parent. */
	XSLProjectSessionEditor( XSLProjectSession * parent = 0 );
	/*! Destroy the session editor */
	~XSLProjectSessionEditor();

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
private:
	QHash<QString,QVariant> m_properties;
	XSLProjectSession * m_parent;
};

/*!
 * An XSLProjectSession represents the .session file associate to the project and who contains
 * tempory data as last opened file, current opened file. This file is extern of project file,
 * to permit project file to be saved in Revision Control System.
 */
class XSLProjectSession : public QObject {
	Q_OBJECT
	Q_PROPERTY( QStringList lastOpenedFile READ lastOpenedFile )
public:
	/*! Create an empty session */
	XSLProjectSession();
	/*!
	 * Create a session object and load the file \e filename
	 * \throw XSLProjectException When the application can't read the session file.
	 * \sa loadFromFile
	 */
	XSLProjectSession( const QString & filename );
	/*! Destroy the session */
	virtual ~XSLProjectSession();

	/*!
	 * Load the file \e filename
	 * \sa saveToFile
	 * \throw XSLProjectException When the application can't read the session file.
	 */
	void loadFromFile( const QString & filename );

	/*!
	 * Save the session in the file \e filename, or in the loaded file if possible
	 * \sa loadFromFile
	 * \throw XSLProjectException When the application can't save the session file.
	 */
	void saveToFile( const QString & filename = QString() );

	/*!
	 * List all the last opend files in the project.
	 * \return the list of the last opend file.
	 */
	QStringList & lastOpenedFile();

	/*! List of serialized editor. */
	const QList<XSLProjectSessionEditor*> & serializedEditors() const;
private:
	QString m_filename;
	QStringList m_lastOpenedFile;
	QList<XSLProjectSessionEditor*> m_sessions;

	friend class XSLProjectSessionEditor;
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
	Q_PROPERTY( QString fileName READ fileName )
	Q_PROPERTY( XSLProjectSession* session READ session )
	Q_PROPERTY( QString projectName READ projectName WRITE setProjectName )
	Q_PROPERTY( ProjectOptions options READ options WRITE setOptions )
	Q_PROPERTY( QString projectRCS READ projectRCS WRITE setProjectRCS )
	Q_PROPERTY( QString defaultLang READ defaultLang WRITE setDefaultLang )
	Q_PROPERTY( QString defaultNav READ defaultNav WRITE setDefaultNav )
	Q_PROPERTY( QString projectPath READ projectPath WRITE setProjectPath )
	Q_PROPERTY( QString specifiquePathName READ specifiquePathName WRITE setSpecifiquePathName )
	Q_PROPERTY( QString logProjectDirectory READ logProjectDirectory WRITE setLogProjectDirectory )
	Q_PROPERTY( QString specifiquePrefix READ specifiquePrefix WRITE setSpecifiquePrefix )
	Q_PROPERTY( QStringList specifiquePrefixes READ specifiquePrefixes )
	Q_PROPERTY( QStringList searchPathList READ searchPathList )
public:
	/*!
	 * This enumeration is a list of options that can be used by a project. This options active
	 * some functionnalities in XINX has the customization of stylesheet (make it specifique) and
	 * usability of webservices.
	 */
	enum ProjectOption {
		NoOptions = 0x0, ///< No options are defined.
		hasSpecifique = 0x1, ///< The project can have specifique stylesheet
		hasWebServices = 0x2 ///< The project can connect to WebServices (plugin must be activated)
	};
	Q_DECLARE_FLAGS( ProjectOptions, ProjectOption );

	/*! Create an empty project. */
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
	/*! Destroy the project */
	~XSLProject();

	/*! Assigned operator to copy object contents at assignation */
	XSLProject& operator=( const XSLProject& p );

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
	 * Return the file name where the project is stored.
	 * \return the file name.
	 */
	const QString & fileName() const;

	/*! Return the session object used to store session information. */
	XSLProjectSession * session() const;

	/*!
	 * Save the session in the file.
	 * \throw XSLProjectException when the application can't save the session file.
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
	 * Return the options of the project.
	 * \return The options of the project.
	 * \sa setOptions()
	 */
	ProjectOptions & options();
	/*!
	 * Set the options of the project
	 * \param options The new options of the project.
	 * \sa options()
	 */
	void setOptions( ProjectOptions options );

	/*!
	 * The concurent file system used by XINX (only CVS can be used).
	 * \return Return the number version.
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
	 * Set the specifique project path name with the value stored in \e value. The specifique project
	 * path name is the project that must be replaced by &lt;project&gt;
	 */
	void setSpecifiquePathName( const QString & value );
	/*!
	 * Return the specifique project path name.
	 * \return specifique project path name
	 */
	QString specifiquePathName();

	/*! Set the log projet directory with the new \e value. */
	void setLogProjectDirectory( const QString & value );
	/*! Return the log project directory. */
	const QString & logProjectDirectory();

	/*!
	 * Get the specifique prefix.
	 * \return the specifique prefix.
	 * \sa setSpecifPrefix()
	 */
	QString specifiquePrefix() const;
	/*!
	 * Set the specifique prefix.
	 * \param value Value of prefix.
	 * \sa setSpecifPrefix()
	 */
	void setSpecifiquePrefix( const QString & value );

	/*! List of all specifique prefix. */
	QStringList & specifiquePrefixes();

	/*!
	 * List of path where the application must search. This list is used in importations.
	 * \return List of search path
	 */
	QStringList & searchPathList();

	/*! List of processed path where the application must searh. */
	QStringList processedSearchPathList();

	/*!
	 * Get the specifique project path (as &lt;project&gt;/langue/&lt;langue&gt;/nav/projet). The specifique
	 * project path is in the QStringList.
	 * \return the path
	 * \sa projectPath(), setProjectPath(), setSpecifPath()
	 */
	int indexOfSpecifiquePath() const;
	/*!
	 * Set the specifique project path (as &lt;project>/langue/&lt;langue&gt;/nav/projet)
	 * \param value The specifique path
	 * \sa projectPath(), setProjectPath(), languePath(), navPath(), languesPath(), specifPath()
	 */
	void setIndexOfSpecifiquePath( int value );
	/*!
	 * Get the specifique project path but replace &lt;lang&gt;,&lt;nav&gt;, and &lt;project&gt; pattern.
	 * \return the pattern modified path
	 * \sa specifPath(), setSpecifPath()
	 */
	QString processedSpecifiquePath() const;

	/*! Block the signal temporary */
	bool blockSignals( bool block );
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
	QVariant readProperty( const QString & key );
signals:
	/*! Emited when a property has changed. */
	void changed();
private:
	PrivateXSLProject * d;
	friend class PrivateXSLProject;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(XSLProject::ProjectOptions);

/*!
 * The XINX Project Manager is used to manage \e XSLProject file. One project
 * only by process is authorized.
 */
class XINXProjectManager : public QObject {
	Q_OBJECT
public:
	/*! Destroy the manager */
	virtual ~XINXProjectManager();

	/*! Create the manager if necessary, else return the singleton */
	static XINXProjectManager * self();

	/*! Change the current projet to \e project */
	void setCurrentProject( XSLProject * project );
	/*! Return the project */
	XSLProject * project() const;

	/*! Delete the project */
	void deleteProject();
signals:
	/*! Signal emited when the project is changed or when the project emit \e changed() */
	void changed();
private:
	/*! Create the XINX Project Manager */
	XINXProjectManager();

	XSLProject * m_project;
	static XINXProjectManager * s_self;
};

#endif
