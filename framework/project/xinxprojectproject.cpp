/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Xinx header
#include "xinxprojectproject.h"
#include "xinxprojectprojectexception.h"
#include "externalfileresolver.h"

// Qt header
#include <QDir>
#include <QDomElement>
#include <QDomDocument>

#define XINX_PROJECT_FILENAME ".xinx"

/*!
 * \defgroup XinxProject Project Management
 */

/*!
 * \ingroup XinxProject
 * \brief Namespace to manage all project
 * \since 0.9.1.0
 *
 * The namespace XinxProject contains all classes used by XINX to manage project.
 * The project is stored in a file named \em .xinx that contains all project information
 * and can be share on multiple computers
 *
 * An empty project must be created when no project is opened. The project will be in memory.
 */
namespace XinxProject {

/* PrivateXinxProject */

class PrivateXinxProject
{
public:
	PrivateXinxProject();
	~PrivateXinxProject();

	void initialisation();

	static QString getValue(const QDomDocument & document, const QString & node);
	static void setValue(QDomDocument & document, const QString & node, const QString & value);

	static QStringList loadList(QDomDocument document, const QString & list, const QString & element);
	static void saveList(QDomDocument & document, const QString & list, const QString & element, const QStringList & slist);

	void loadFromFile();
	void saveToFile();
    QString tr(const char* arg1);

	int m_version;

	XinxProject::ProjectPtrWeak m_self;
	QString m_fileName;
	QString m_projectName;
	QString m_projectRCS;
	QStringList m_activatedPlugin;
	QHash<QString,QVariant> m_properties;
	QHash<QString,QObject*> m_objects;

	QString m_projectPath;
	ContentView3::Cache * m_cache;
	ExternalFileResolver * _resolver;
	VersionControl::RCSProxy * m_rcsProxy;

	void load_rcsproxy_settings();
};


PrivateXinxProject::PrivateXinxProject(): m_version(XINX_PROJECT_VERSION), m_cache(0), _resolver(0), m_rcsProxy(0)
{
}

PrivateXinxProject::~PrivateXinxProject()
{
	delete m_rcsProxy;
	delete m_cache;
	delete _resolver;
}

void PrivateXinxProject::initialisation ()
{
	m_rcsProxy		= new VersionControl::RCSProxy(m_self);
	m_cache			= new ContentView3::Cache(m_self);
	_resolver		= new ExternalFileResolver(m_self);
}

QString PrivateXinxProject::tr(const char* arg1)
{
	return Project::tr(arg1);
}

QString PrivateXinxProject::getValue(const QDomDocument & document, const QString & node)
{
	QDomElement root = document.documentElement();
	QDomElement elt  = root.firstChildElement(node);
	return elt.text();
}

void PrivateXinxProject::setValue(QDomDocument & document, const QString & node, const QString & value)
{
	QDomElement root = document.documentElement();
	QDomElement elt  = root.firstChildElement(node);
	QDomText    text;

	if (elt.isNull())
	{
		elt = document.createElement(node);
		root.appendChild(elt);
	}
	else
	{
		QDomNode node = elt.firstChild();
		while (!(node.isNull() || node.isText())) node = node.nextSibling();
		text = node.toText();
	}

	if (text.isNull())
	{
		text = document.createTextNode(value);
		elt.appendChild(text);
	}
	else
		text.setData(value);
}

QStringList PrivateXinxProject::loadList(QDomDocument document, const QString & list, const QString & element)
{
	QDomElement root = document.documentElement();
	QDomElement dlist = root.firstChildElement(list);
	if (dlist.isNull()) return QStringList();

	QStringList result;
	QDomElement delement = dlist.firstChildElement(element);
	while (! delement.isNull())
	{
		result.append(delement.firstChild().toText().nodeValue());
		delement = delement.nextSiblingElement(element);
	}

	return result;
}

void PrivateXinxProject::saveList(QDomDocument & document, const QString & list, const QString & element, const QStringList & slist)
{
	QDomElement root = document.documentElement();
	QDomElement dlist = document.createElement(list);
	root.appendChild(dlist);

	foreach(const QString & s, slist)
	{
		QDomElement delement = document.createElement(element);
		dlist.appendChild(delement);

		QDomText text = document.createTextNode(s);
		delement.appendChild(text);
	}
}

void PrivateXinxProject::loadFromFile()
{
	QFile file(m_fileName);
	QDomDocument document;

	// Open the file
	if (! file.open(QFile::ReadOnly | QFile::Text))
		throw ProjectException(tr("Cannot read file %1:\n%2.").arg(m_fileName).arg(file.errorString()));

	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;
	if (! document.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
		throw ProjectException(tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));

	QDomElement root = document.documentElement();

	// Test if Project File
	if (root.tagName() != "XinxProject")
		throw ProjectException(tr("The file isn't a XINX Project. The root name must be \"XinxProject\", not \"%1\".").arg(root.tagName()));

	m_version = getValue(document, "xinx_version").isEmpty() ? 0 : getValue(document, "xinx_version").toInt();
	if (m_version > XINX_PROJECT_VERSION)
		throw ProjectException(tr("The file is a too recent XINX Project"));
	if (m_version < XINX_PROJECT_VERSION)
		throw ProjectException(tr("The project can't be opened. Please use the XINX Project Wizard."), true);


	// Load values
	m_projectName         = getValue(document, "name");
	m_projectRCS          = getValue(document, "rcs");
	m_activatedPlugin     = loadList(document, "activatedPlugin", "name");

	QDomElement propertiesElement = root.firstChildElement("properties");

	m_properties.clear();
	QDomElement node = propertiesElement.firstChildElement();
	while (! node.isNull())
	{
		QString type  = node.attribute("type", "QString"),
				name  = node.tagName(),
				value = node.text();

		if ((type == "QStringList") && (! value.isEmpty()))
		{
			m_properties[name] = QVariant::fromValue(value.split(";;;"));
		}
		else if (! value.isEmpty())
		{
			QVariant::Type variantType = QVariant::nameToType(qPrintable(type));
			QVariant p(variantType);
			p.setValue(value);
			m_properties[ name ] = p;
		}

		node = node.nextSiblingElement();
	}

	load_rcsproxy_settings();
}

void PrivateXinxProject::saveToFile()
{
	// Save attributes
	QDomDocument document("XinxProject");
	QDomElement root = document.createElement("XinxProject");
	document.appendChild(root);

	setValue(document, "xinx_version", QString("%1").arg(XINX_PROJECT_VERSION));
	setValue(document, "name", m_projectName);
	setValue(document, "rcs", m_projectRCS);
	saveList(document, "activatedPlugin", "name", m_activatedPlugin);

	QDomElement propertiesElement = document.createElement("properties");
	root.appendChild(propertiesElement);

	foreach(const QString & key, m_properties.keys())
	{
		const QString typeName = m_properties[ key ].typeName();
		const QString value = typeName == "QStringList" ? m_properties[ key ].toStringList().join(";;;") : m_properties[ key ].toString();

		QDomElement propertyElement = document.createElement(key);
		QDomText text = document.createTextNode(value);
		propertyElement.appendChild(text);
		propertyElement.setAttribute("type", typeName);
		propertiesElement.appendChild(propertyElement);
	}

	// Open the file
	static const int IndentSize = 3;
	QFile file(m_fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
		throw ProjectException(tr("Cannot write file %1:\n%2.").arg(m_fileName).arg(file.errorString()));

	QTextStream text(&file);
	document.save(text, IndentSize);
}

void PrivateXinxProject::load_rcsproxy_settings()
{
	m_rcsProxy->setCurrentRootPath(m_projectPath);
	m_rcsProxy->setCurrentRCS(m_projectRCS);
}

/* XinxProject */

/*!
 * \ingroup XinxProject
 * \class Project
 * \brief Class that contains project property.
 * \since 0.9.1.0
 *
 *
 */

Project::Project()
{
	d = new PrivateXinxProject;

	qDebug() << "Create a project with no name " << this;
}

Project::Project (const QString & path)
{
	Q_ASSERT_X(! path.isEmpty(), "XinxProject::Project::Project", "The path mustn't be empty.'");

	if (! QDir(path).exists())
	{
		throw ProjectException(tr("The project must be created in a existing directory"), false);
	}

	d = new PrivateXinxProject;

	d->m_projectPath = path;
	d->m_fileName    = QDir(projectPath()).absoluteFilePath(XINX_PROJECT_FILENAME);

	qDebug() << "Create the project " << path << " " << this;
}

/*!
 * \brief To create an empty project
 * \since 0.9.1.0
 */
XinxProject::ProjectPtr Project::create()
{
	XinxProject::ProjectPtr ptr(new Project());
	ptr->d->m_self = ptr.toWeakRef();
	ptr->d->initialisation();
	return ptr;
}

/*!
 * \brief To create a project base on a directory
 * \since 0.9.1.0
 *
 * \throw XinxProject::ProjectException When the application can't read a project file or can't
 * save it.
 *
 * Warning : The project must exists
 */
XinxProject::ProjectPtr Project::create(const QString & path, bool creation)
{
	XinxProject::ProjectPtr ptr(new Project(path));
	ptr->d->m_self = ptr.toWeakRef();
	ptr->d->initialisation();

	if (! creation)
	{
		ptr->d->loadFromFile();
	}

	return ptr;
}

/*!
 * \brief Destroy the project
 *
 * When the project is destroying, he must clear all reference to it-self
 * of the the ContentView2::Cache.
 */
Project::~Project()
{
	qDebug() << "Destroy the project " << d->m_fileName << " " << this;

	delete d;
}

/*!
 * \brief Return the curent project path
 * \since 0.9.1.0
 */
const QString & Project::projectPath() const
{
	return d->m_projectPath;
}

/*!
 * \brief This method return the cache of the project.
 * \since 0.9.1.0
 */
ContentView3::Cache* Project::cache()
{
	return d->m_cache;
}

/*!
 * \brief This method return the resolver that can be used to resolve import
 * \since 0.9.1.0
 */
ExternalFileResolver * Project::resolver()
{
	return d->_resolver;
}

/*!
 * \brief Return the RCS Proxy from the given project.
 * \since 0.9.1.0
 */
VersionControl::RCSProxy* Project::rcsProxy()
{
	return d->m_rcsProxy;
}


/*!
 * \brief The concurent file system used by XINX.
 * \return Return the name of the RCS.
 * \sa setProjectRCS()
 */
const QString & Project::projectRCS() const
{
	return d->m_projectRCS == "no" ? d->m_projectRCS = QString() : d->m_projectRCS;
}

/*!
 * \brief Set the concurent file system used by XINX.
 * \param value The new system used.
 * \sa projectRCS()
 */
void Project::setProjectRCS(const QString & value)
{
	if (d->m_projectRCS != value)
	{
		d->m_projectRCS = value;
		d->saveToFile();
		emit changed();
		d->load_rcsproxy_settings ();
	}
}

/*!
 * \brief Name of the project. The name is just an information, nothing else.
 * \return The name of the project.
 * \sa setProjectName();
 */
QString Project::projectName() const
{
	return d->m_projectName;
}

/*!
 * \brief Set the name of the project.
 * \param value The new name of the project.
 * \sa projectName()
 */
void Project::setProjectName(const QString & value)
{
	if (d->m_projectName != value)
	{
		d->m_projectName = value;
		d->saveToFile();
		emit changed();
	}
}

/*!
 * \brief Activated plugin for the project (the plugin must control this it-self, it's not a XINX control)
 */
QStringList Project::activatedPlugin() const
{
	return d->m_activatedPlugin;
}

/*!
 * \brief Set activated plugin for the project
 * \param value The list of plugin activated for the project
 */
void Project::setActivatedPlugin(const QStringList & value)
{
	if (d->m_activatedPlugin != value)
	{
		d->m_activatedPlugin = value;
		d->saveToFile();
		emit changed();
	}
}

/*!
 * \brief Write a property in the project file (used by the plugin).
 * \param key The property to write
 * \param value The value to write
 */
void Project::writeProperty(const QString & key, QVariant value)
{
	if (d->m_properties.value(key) != value)
	{
		d->m_properties[ key ] = value;
		d->saveToFile();
		emit changed();
	}
}

/*!
 * \brief Read a written property from the project file (used by the plugin).
 * \param key The Property to read
 * \return The value of the property
 */
QVariant Project::readProperty(const QString & key) const
{
	return d->m_properties.value(key);
}

void Project::addObject(const QString& name, QObject* object)
{
	d->m_objects.insert(name, object);
	object->setParent(this);
}

void Project::removeObject(const QString& name)
{
	QObject * object = d->m_objects.take(name);
	delete object;
}

bool Project::containsObject(const QString& name) const
{
	return d->m_objects.contains(name);
}

QObject* Project::getObject(const QString& name) const
{
	return d->m_objects.value(name);
}

} // namespace XinxProject
