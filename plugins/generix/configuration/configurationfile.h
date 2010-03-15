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

#ifndef __CONFIGURATIONFILE_H__
#define __CONFIGURATIONFILE_H__
#pragma once

// Xinx header
#include <core/exceptions.h>

// Qt header
#include <QString>
#include <QList>
#include <QMultiHash>
#include <QVariant>

// TODO: Changer complétement le fonctionnement de la gestion du fichier de configuration. Voir ticket #291

// TODO: Delete this workaround
#undef major
#undef minor

class ConfigurationVersionIncorectException : public XinxException
{
public:
	ConfigurationVersionIncorectException(QString version);
};

class MetaConfigurationException : public XinxException
{
public:
	MetaConfigurationException(QString message);
};

class MetaConfigurationNotExistException : public MetaConfigurationException
{
public:
	MetaConfigurationNotExistException(QString filename);
};


class PrivateMetaConfigurationFile;

/*!
 * Represent a version of a XML Configuration file. The file version has
 * the form VXX.XX-XX. The version can also have a special edition number.
 */
class ConfigurationVersion
{
public:
	/*!
	 * Construct a configuration version object
	 * \param major The major number of the version.
	 * \param minor The minor number of the version.
	 * \param release The release number of the version
	 * \param build The build number (or special edition number) of the version
	 */
	ConfigurationVersion(int major = -1, int minor = -1, int release = -1, int build = -1);
	/*!
	 * Construct a configuration version as input a strint. The number version translate
	 * the string into a version object. The special number is outside
	 * \param version The version string used to construct the object
	 * \param build The build number (or special edition number) of the version.
	 */
	ConfigurationVersion(const QString & version, int build = 0);
	/*!
	 * Destroy the configuration version object.
	 */
	virtual ~ConfigurationVersion();

	/*!
	 * Return the major part of the version
	 * \return An integer represents the major parts
	 */
	int major() const;
	/*!
	 * Return the minor part of the version
	 * \return An integer represents the minor parts
	 */
	int minor() const;
	/*!
	 * Return the release part of the version
	 * \return An integer represents the release parts
	 */
	int release() const;
	/*!
	 * Return the build part of the version (or called special edition).
	 * \return An integer represents the special edition number
	 */
	int build() const;

	/*!
	 * Return a string a representation of the version. The string has the form :
	 * VXX.XX-XX (XX) where the number in bracket is the special edition number. This
	 * is not necessary and can be omited, if not specified.
	 * \return A string representation of the version.
	 */
	QString toString() const;
	/*!
	 * Test if a version is valid.
	 * A version is valid only if major, minor, and release number are greater than
	 * zero.
	 * \return true if the version is valid, else return false.
	 */
	bool isValid() const;

	/*!
	 * Compare two version number.
	 * \return Return false if the two version are equal
	 */
	bool operator!= (ConfigurationVersion version) const;
	/*!
	 * Compare two version number.
	 * \return Return true if the two version are equal
	 */
	bool operator== (ConfigurationVersion version) const;
	/*!
	 * Compare two version number.
	 * \return Return true if the first is greater than the second
	 */
	bool operator> (ConfigurationVersion version) const;
	/*!
	 * Compare two version number.
	 * \return Return true if the first is greater or equal than the second
	 */
	bool operator>= (ConfigurationVersion version) const;
	/*!
	 * Compare two version number.
	 * \return Return true if the first is lesser than the second
	 */
	bool operator< (ConfigurationVersion version) const;
	/*!
	 * Compare two version number.
	 * \return Return true if the first is lesser or equal than the second
	 */
	bool operator<= (ConfigurationVersion version) const;
private:
	int m_major;
	int m_minor;
	int m_release;
	int m_build;
};

class BusinessViewInformation
{
public:
	BusinessViewInformation();

	const QString & configurationFileName() const;
	void setConfigurationFileName(const QString & value);

	int configurationNumber() const;
	void setConfigurationNumber(int value);

	const QString & businesViewName() const;
	void setBusinessViewName(const QString & value);

	const QString & targetName() const;
	void setTargetName(const QString & value);

	const QString & viewstructName() const;
	void setViewstructName(const QString & value);
private:
	int m_configurationNumber;
	QString m_configurationFilename, m_businessViewName, m_targetName, m_viewStructName;
};

/*!
 * Class represente a configuration file. This class contais method checking the
 * version number of a configuration file, testing if the file exists in the path
 * or other usuable functionnality.
 */
class ConfigurationFile : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString filename READ filename)
	Q_PROPERTY(ConfigurationVersion version READ version)
	Q_PROPERTY(QString xmlPresentationFile READ xmlPresentationFile)
public:
	ConfigurationFile(QObject * parent = 0);
	ConfigurationFile(const QString & filename, QObject * parent = 0);
	ConfigurationFile(const ConfigurationFile & configuration);

	virtual ~ConfigurationFile();

	const QString & filename() const;

	/*! Return the version of the configuration file. */
	ConfigurationVersion version();

	/*! Return the name of presentations files, if defined, else return presentation.xml */
	QString xmlPresentationFile();

	QStringList businessViewPerFiles(const QString & filename);

	/*!
	 * Test if the configuration file exists in the directory path.
	 * The configuration file has the name configuration.xml
	 * \param DirectoryPath The path of the directory where we check if a configuration file exists.
	 * \return True if the configuration file exist, else return false.
	 */
	static bool exists(const QString & directoryPath);
	/*!
	 * Return the version of the configuration file (only if the file exists and the version
	 * can be found).
	 * This method parse the configuration file to find the version number. If he find him,
	 * the parser stop immediatly to not read the entierly.
	 * \param DirectoryPath the path where the configuration file is.
	 * \return A ConfigurationVersion with the version number or an invalid object if the version can't be found.
	 */
	static ConfigurationFile simpleConfigurationFile(const QString & directoryPath);

	ConfigurationFile & operator=(const ConfigurationFile & p);
private:
	QString m_filename;
	ConfigurationVersion m_version;
	QString m_xmlPresentationFile;
	QString m_rootPath;

	bool m_hasRead;
	void read();
};

/*!
 * Definition of a meta configuration file defined in file configurationdef.xml. The meta file defined
 * one or more configuration file. This file define dictionary too.
 */
class MetaConfigurationFile : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QList<ConfigurationFile*> configurations READ configurations);
public:
	/*!
	 * Create a meta configuration file object an load configuration in memory.
	 * \param The file name of the meta configuration file.
	 */
	MetaConfigurationFile(const QString & filename, QObject * parent = 0);
	/*!
	 * Destroy the meta configuration file
	 */
	virtual ~MetaConfigurationFile();

	/*!
	 * Test if the meta configuration file exists in the path \e DirectoryPath
	 * and return true if the file exist
	 */
	static bool exists(const QString & directoryPath);
	/*!
	 * Return the version of one of the configuration file (only if the file exists and the
	 * version can be found).
	 * This method parse configurations file to find the version number. If a version is founded,
	 * this not parse the next files.
	 * \param DirectoryPath path of the meta configuration file.
	 */
	static ConfigurationFile simpleConfigurationFile(const QString & directoryPath);

	const QList<ConfigurationFile*> & configurations() const;
	const QStringList & dictionaryList() const;
private:
	QList<ConfigurationFile*> m_configurations;
	QStringList m_dictionaryList;
};


Q_DECLARE_METATYPE(ConfigurationVersion);


#endif // __CONFIGURATIONFILE_H__
