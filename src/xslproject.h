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
	 * This enumeration is a list of options that can be used by a project. This options active 
	 * some functionnalities in XINX has the customization of stylesheet (make it specifique) and 
	 * usability of webservices.
	 */
	enum ProjectOption {
		NoOptions = 0x0, ///< No options are defined.
		hasSpecifique = 0x1, ///< The project can have specifique stylesheet
		hasWebServices = 0x2 ///< The project can connect to WebServices
	};
	Q_DECLARE_FLAGS( ProjectOptions, ProjectOption );
	
	/*!
	 * The different Concurent file repository managed by XINX. 
	 * At this time only CVS is partially managed.
	 */
	enum enumProjectRCS { 
		NORCS = 0, ///< The Concurent file repository is not managed.
		CVS = 1, ///< CVS is used.
		SUBVERSION = 2 ///< Subversion is used (XINX don't care at the moment of this choice).
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
	 * Assigned operator to copy object contents at assignation
	 */ 
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

	/*!
	 * List all the last opend files in the project.
	 * \return the list of the last opend file.
	 */
	QStringList & lastOpenedFile();
	
	/*!
	 * List of all saved sessions editors. All informations about sessions are stored in a QByteArray.
	 * This to made more flexible the deserialization. The file name is stored in the editor to get it
	 * easily.
	 * \return list of structSessions;
	 */
	QList<QByteArray> & sessionsEditor();
	
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
	 * Set the specifique project path name with the value stored in \e value. The specifique project
	 * path name is the project that must be replaced by <project>
	 */
	void setSpecifiquePathName( const QString & value );
	/*!
	 * Return the specifique project path name.
	 * \return specifique project path name
	 */
	QString specifiquePathName(); 
	
	/*!
	 * Set the log projet directory with the new \e value.
	 */
	void setLogProjectDirectory( const QString & value );
	/*!
	 * Return the log project directory.
	 */
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

	/*!
	 * List of path where the application must search. This list is used in importations.
	 * \return List of search path
	 */
	QStringList & searchPathList();
	
	/*!
	 * List of processed path where the application must searh.
	 */
	QStringList processedSearchPathList();

	/*!
	 * Get the specifique project path (as <project>/langue/<langue>/nav/projet). The specifique 
	 * project path is in the QStringList.
	 * \return the path
	 * \sa projectPath(), setProjectPath(), setSpecifPath()
	 */
	int indexOfSpecifiquePath() const;
	/*!
	 * Set the specifique project path (as <project>/langue/<langue>/nav/projet)
	 * \param value The specifique path
	 * \sa projectPath(), setProjectPath(), languePath(), navPath(), languesPath(), specifPath()
	 */
	void setIndexOfSpecifiquePath( int value );
	/*!
	 * Get the specifique project path but replace <lang>, <nav>, and <project> pattern.
	 * \return the pattern modified path
	 * \sa specifPath(), setSpecifPath()
	 */
	QString processedSpecifiquePath() const;
	
	/*!
	 * List of Web services link. The link must point to the WSDL.
	 * \return list of WSDL link.
	 */
	QStringList & serveurWeb();
private:
	PrivateXSLProject * d;
	friend class PrivateXSLProject;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(XSLProject::ProjectOptions);

#endif
