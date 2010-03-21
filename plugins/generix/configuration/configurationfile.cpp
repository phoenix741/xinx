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

// Xinx header
#include "configurationfile.h"

// Qt header
#include <QDir>
#include <QFile>

#include <QtXmlPatterns>

#include <QDomDocument>
#include <QDomElement>

// TODO: Delete this workaround
#undef major
#undef minor

/* ConfigurationVerstionIncorectException */

ConfigurationVersionIncorectException::ConfigurationVersionIncorectException(QString version) : XinxException(QString("Wrong version number %1").arg(version))
{

}

/* MetaConfigurationException */

MetaConfigurationException::MetaConfigurationException(QString message) : XinxException(message)
{

}

/* MetaConfigurationNotExistException */

MetaConfigurationNotExistException::MetaConfigurationNotExistException(QString filename) : MetaConfigurationException(QString("Meta configuration \"%1\" not found").arg(filename))
{

}

/* ConfigurationVersion */

ConfigurationVersion::ConfigurationVersion(int major, int minor, int release, int build)
{
	m_major = major;
	m_minor = minor;
	m_release = release;
	m_build = build;
}

ConfigurationVersion::ConfigurationVersion(const QString & version, int build)
{
	QString major, minor, release;

	m_build = build;
	if (! version.length())
		throw ConfigurationVersionIncorectException(version);
	if (version.at(0) != 'V')
		throw ConfigurationVersionIncorectException(version);
	int posDot = version.indexOf("."),
	             posLine = version.indexOf("-", posDot);
	if ((posDot < 0) || (posLine < 0))
		throw ConfigurationVersionIncorectException(version);
	major = version.mid(1, posDot - 1);
	minor = version.mid(posDot + 1, posLine - posDot - 1);
	release = version.mid(posLine + 1);
	m_major = major.toInt();
	m_minor = minor.toInt();
	m_release = release.toInt();
}

ConfigurationVersion::~ConfigurationVersion()
{
}

int ConfigurationVersion::major() const
{
	return m_major;
}

int ConfigurationVersion::minor() const
{
	return m_minor;
}

int ConfigurationVersion::release() const
{
	return m_release;
}

int ConfigurationVersion::build() const
{
	return m_build;
}

QString ConfigurationVersion::toString() const
{
	QString version = QString("V%1.%2-%3").arg(m_major, 2, 10, QLatin1Char('0'))
	                  .arg(m_minor, 2, 10, QLatin1Char('0'))
	                  .arg(m_release, 2, 10, QLatin1Char('0'));
	if (m_build > 0)
		version += QString(" (%1)")     .arg(m_build, 2, 10, QLatin1Char('0'));

	return version;
}

bool ConfigurationVersion::isValid() const
{
	return (m_major >= 0) && (m_minor >= 0) && (m_release >= 0);
}

bool ConfigurationVersion::operator!= (ConfigurationVersion version) const
{
	return !(*this == version);
}

bool ConfigurationVersion::operator== (ConfigurationVersion version) const
{
	if (m_major == version.m_major)
	{
		if (m_minor == version.m_minor)
		{
			if (m_release == version.m_release)
			{
				if (m_build == version.m_build)
				{
					return true;
				}
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

bool ConfigurationVersion::operator> (ConfigurationVersion version) const
{
	if (m_major == version.m_major)
	{
		if (m_minor == version.m_minor)
		{
			if (m_release == version.m_release)
			{
				if (m_build == version.m_build)
				{
					return false;
				}
				else
					return m_build > version.m_build;
			}
			else
				return m_release > version.m_release;
		}
		else
			return m_minor > version.m_minor;
	}
	else
		return m_major > version.m_major;
}

bool ConfigurationVersion::operator>= (ConfigurationVersion version) const
{
	return (*this > version) || (*this == version);
}

bool ConfigurationVersion::operator< (ConfigurationVersion version) const
{
	return !(*this >= version);
}

bool ConfigurationVersion::operator<= (ConfigurationVersion version) const
{
	return !(*this > version);
}

/* BusinessViewInformation */

BusinessViewInformation::BusinessViewInformation()
{
}

const QString & BusinessViewInformation::configurationFileName() const
{
	return m_configurationFilename;
}

void BusinessViewInformation::setConfigurationFileName(const QString & value)
{
	m_configurationFilename = value;
}

int BusinessViewInformation::configurationNumber() const
{
	return m_configurationNumber;
}

void BusinessViewInformation::setConfigurationNumber(int value)
{
	m_configurationNumber = value;
}

const QString & BusinessViewInformation::businesViewName() const
{
	return m_businessViewName;
}

void BusinessViewInformation::setBusinessViewName(const QString & value)
{
	m_businessViewName = value;
}

const QString & BusinessViewInformation::targetName() const
{
	return m_targetName;
}

void BusinessViewInformation::setTargetName(const QString & value)
{
	m_targetName = value;
}

const QString & BusinessViewInformation::viewstructName() const
{
	return m_viewStructName;
}

void BusinessViewInformation::setViewstructName(const QString & value)
{
	m_viewStructName = value;
}

/* ConfigurationFileXmlReceiver */

class ConfigurationFileXmlReceiver : public QAbstractXmlReceiver
{
public:
	ConfigurationFileXmlReceiver(QXmlQuery * query, QString * version, int * build, QString * xmlPresentationFile, QString * rootPath)
			: QAbstractXmlReceiver(), m_query(query), m_version(version), m_xmlPresentationFile(xmlPresentationFile), m_rootPath(rootPath), m_build(build)
	{
	};
	virtual ~ConfigurationFileXmlReceiver() {};

	virtual void atomicValue(const QVariant & value)
	{
		if (m_currentElement == "numero")
		{
			*m_version = value.toString();
		}
		else if (m_currentElement == "edition_speciale")
		{
			*m_build = value.toString().toInt();
		}
		else if (m_currentElement == "xmlpres")
		{
			*m_xmlPresentationFile = value.toString();
		}
		else if (m_currentElement == "rootPath")
		{
			*m_rootPath = value.toString();
		}
	};
	virtual void attribute(const QXmlName &, const QStringRef &) {};
	virtual void comment(const QString &) {};
	virtual void endDocument() {};
	virtual void endOfSequence() {};
	virtual void namespaceBinding(const QXmlName &) {};
	virtual void processingInstruction(const QXmlName &, const QString &) {};
	virtual void startDocument() {};
	virtual void startOfSequence() {};
	virtual void endElement() {};

	virtual void startElement(const QXmlName & name)
	{
		m_currentElement = name.localName(m_query->namePool());
	};
	virtual void characters(const QStringRef & value)
	{
		atomicValue(value.toString());
	};
private:
	QXmlQuery * m_query;
	QString m_currentElement;
	QString * m_version, * m_xmlPresentationFile, * m_rootPath;
	int * m_build;

};

/* ConfigurationFile */

ConfigurationFile::ConfigurationFile(QObject * parent) : QObject(parent), m_hasRead(false)
{
}

ConfigurationFile::ConfigurationFile(const QString & filename, QObject * parent) : QObject(parent), m_filename(filename), m_hasRead(false)
{
}

ConfigurationFile::ConfigurationFile(const ConfigurationFile & configuration) : QObject()
{
	*this = configuration;
}

ConfigurationFile::~ConfigurationFile()
{
}

ConfigurationFile & ConfigurationFile::operator=(const ConfigurationFile & p)
{
	this->m_version = p.m_version;
	this->m_filename = p.m_filename;
	this->m_xmlPresentationFile = p.m_xmlPresentationFile;
	this->m_hasRead = p.m_hasRead;

	return *this;
}

const QString & ConfigurationFile::filename() const
{
	return m_filename;
}

void ConfigurationFile::read()
{
	if (m_hasRead) return;

	// Initialisation
	QString versionInfo;
	int buildInfo = 0;
	m_xmlPresentationFile = "Presentation.xml";

	// Open the file to read it
	QFile sourceDocument;
	sourceDocument.setFileName(m_filename);
	if (! sourceDocument.open(QIODevice::ReadOnly))
	{
		qWarning(qPrintable(tr("Error while opening configuration file : %1").arg(sourceDocument.errorString())));
		return;
	}

	// Open the query
	QFile sourceCode;
	sourceCode.setFileName("scripts:conf_version.xq");
	if (! sourceCode.open(QIODevice::ReadOnly))
	{
		qWarning(qPrintable(tr("Error while opening version query : %1").arg(sourceCode.errorString())));
		return ;
	}

	// Prepare the query
	QXmlQuery query;
	ConfigurationFileXmlReceiver receiver(&query, &versionInfo, &buildInfo, &m_xmlPresentationFile, &m_rootPath);

	// Execute the query
	query.bindVariable("inputDocument", &sourceDocument);
	query.setQuery(&sourceCode);
	query.evaluateTo(&receiver);

	// Interprete the result
	try
	{
		m_version = ConfigurationVersion(versionInfo, buildInfo);
	}
	catch (ConfigurationVersionIncorectException)
	{
		m_version = ConfigurationVersion();
	}
	m_hasRead = true;
}

ConfigurationVersion ConfigurationFile::version()
{
	read();
	return m_version;
}

QString ConfigurationFile::xmlPresentationFile()
{
	read();
	return m_xmlPresentationFile;
}

QStringList ConfigurationFile::businessViewPerFiles(const QString & filename)
{
	read();
	// Initialisation
	QStringList businessViewList;
	QString relativeFileName = filename;
	relativeFileName.remove(m_rootPath);
	while (relativeFileName.startsWith('/')) relativeFileName.remove(0, 1);

	// Open the source document
	QFile sourceDocument;
	sourceDocument.setFileName(m_filename);
	if (! sourceDocument.open(QIODevice::ReadOnly))
	{
		qWarning(qPrintable(tr("Error while opening configuration file : %1").arg(sourceDocument.errorString())));
		return businessViewList;
	}

	// Open the source query
	QFile sourceCode;
	sourceCode.setFileName("scripts:conf_businessviewlist.xq");
	if (! sourceCode.open(QIODevice::ReadOnly))
	{
		qWarning(qPrintable(tr("Error while opening businessview list query : %1").arg(sourceCode.errorString())));
		return businessViewList;
	}

	// Prepare the query
	QXmlQuery query;
	QXmlResultItems result;

	query.bindVariable("inputDocument", &sourceDocument);
	query.bindVariable("relativeFileName", QVariant::fromValue(relativeFileName));
	query.setQuery(&sourceCode);

	// Execute the query
	query.evaluateTo(&result);

	// Interprete the result
	QXmlItem item(result.next());
	while (!item.isNull())
	{
		businessViewList << item.toAtomicValue().toString();

		item = result.next();
	}

	return businessViewList;
}

bool ConfigurationFile::exists(const QString & directoryPath)
{
	return QFile::exists(QDir(directoryPath).absoluteFilePath("configuration.xml"));
}

ConfigurationFile ConfigurationFile::simpleConfigurationFile(const QString & directoryPath)
{
	QString fileName = QFileInfo(directoryPath).isDir() ?
	                   QDir(directoryPath).absoluteFilePath("configuration.xml") :
	                   directoryPath;

	return ConfigurationFile(fileName);
}

/* MetaConfigurationFile */

MetaConfigurationFile::MetaConfigurationFile(const QString & filename, QObject * parent) : QObject(parent)
{
	QString directoryPath = QFileInfo(filename).absolutePath();

	QDomDocument document("configurationdef.xml");
	QFile file(filename);
	if (! file.open(QIODevice::ReadOnly))
	{
		// If MetaConfigurationFile not exists, there is one ;)
		m_configurations << new ConfigurationFile(QDir(directoryPath).absoluteFilePath("configuration.xml"), this);
	}
	else if (! document.setContent(&file))
	{
		file.close();
		throw MetaConfigurationException(QString("I can't read \"%1\" as XML Document.").arg(filename));
	}
	else
	{
		QDomElement root = document.documentElement();

		QDomElement configuration = root.firstChildElement("configuration");
		if (! configuration.isNull())
		{
			QDomElement conffile = configuration.firstChildElement("file");
			while (! conffile.isNull())
			{
				m_configurations.insert(0, new ConfigurationFile(QDir(directoryPath).absoluteFilePath(conffile.attribute("name"))));
				conffile = conffile.nextSiblingElement("file");
			}
		}
		QDomElement translation = root.firstChildElement("translation");
		if (! translation.isNull())
		{
			QDomElement translationFile = translation.firstChildElement("file");
			while (! translationFile.isNull())
			{
				m_dictionaryList += QDir(directoryPath).absoluteFilePath(translationFile.attribute("name"));
				translationFile = translationFile.nextSiblingElement("file");
			}
		}
	}

	if (m_configurations.count() == 0)
		throw MetaConfigurationException(QString("No configuration file found in \"%1\"").arg(filename));
}

MetaConfigurationFile::~MetaConfigurationFile()
{
	qDeleteAll(m_configurations);
}

bool MetaConfigurationFile::exists(const QString & directoryPath)
{
	return QFile::exists(QDir(directoryPath).absoluteFilePath("configurationdef.xml"));
}

ConfigurationFile MetaConfigurationFile::simpleConfigurationFile(const QString & directoryPath)
{
	try
	{
		MetaConfigurationFile meta(QDir(directoryPath).absoluteFilePath("configurationdef.xml"));
		foreach(ConfigurationFile * conf, meta.m_configurations)
		{
			if (conf->version().isValid())
			{
				return *conf;
			}
		}
		return ConfigurationFile::simpleConfigurationFile(directoryPath);
	}
	catch (MetaConfigurationException)
	{
		return ConfigurationFile::simpleConfigurationFile(directoryPath);
	}
}

const QList<ConfigurationFile*> & MetaConfigurationFile::configurations() const
{
	return m_configurations;
}

const QStringList & MetaConfigurationFile::dictionaryList() const
{
	return m_dictionaryList;
}

