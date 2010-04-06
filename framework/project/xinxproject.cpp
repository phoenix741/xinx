/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

// Qt header
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>

// Xinx header
#include "project/xinxproject.h"
#include "core/xinxcore.h"
#include "core/xinxconfig.h"
#include "plugins/xinxpluginsloader.h"
#include "contentview2/contentview2manager.h"
#include "contentview2/contentview2cache.h"

/* Static member */

XINXProjectManager * XINXProjectManager::s_self = 0;

/* XinxProjectException */

XinxProjectException::XinxProjectException(const QString & message, bool wizard) : XinxException(message), m_wizard(wizard)
{
}

bool XinxProjectException::startWizard() const
{
	return m_wizard;
}

/* XinxProjectSessionEditor */

XinxProjectSessionEditor::XinxProjectSessionEditor(XinxProjectSession * parent) : QObject(parent), m_parent(parent)
{
	parent->m_sessions.append(this);
}

XinxProjectSessionEditor::~XinxProjectSessionEditor()
{
	m_parent->m_sessions.removeAll(this);
}

void XinxProjectSessionEditor::loadFromNode(const QDomElement & element)
{
	m_properties.clear();
	QDomElement node = element.firstChildElement();
	while (! node.isNull())
	{
		QString name  = node.tagName(),
		                value = node.text();

		if (! value.isEmpty())
			m_properties[ name ] = value;

		node = node.nextSiblingElement();
	}
}

void XinxProjectSessionEditor::saveToNode(QDomDocument & document, QDomElement & element)
{
	foreach(const QString & key, m_properties.keys())
	{
		QDomElement propertyElement = element.firstChildElement(key);
		if (! propertyElement.isNull()) element.removeChild(propertyElement);

		QDomText text = document.createTextNode(m_properties[ key ].toString());

		propertyElement = document.createElement(key);
		propertyElement.appendChild(text);
		propertyElement.setAttribute("type", m_properties[ key ].typeName());
		element.appendChild(propertyElement);
	}
}

void XinxProjectSessionEditor::writeProperty(const QString & key, QVariant value)
{
	m_properties[ key ] = value;
}

QVariant XinxProjectSessionEditor::readProperty(const QString & key)
{
	return m_properties.value(key);
}

QString XinxProjectSessionEditor::projectPath() const
{
	return m_parent->project()->projectPath();
}

/* XinxProjectSession */

XinxProjectSession::XinxProjectSession(XinxProject * project) : m_project(project)
{

}

XinxProjectSession::XinxProjectSession(XinxProject * project, const QString & filename) : m_project(project)
{
	loadFromFile(filename);
}

XinxProjectSession::~XinxProjectSession()
{
	qDeleteAll(m_sessions);
}

XinxProject * XinxProjectSession::project() const
{
	return m_project;
}

void XinxProjectSession::loadFromFile(const QString & filename)
{
	m_filename = filename;
	QFile file(filename);

	m_lastOpenedFile.clear();
	qDeleteAll(m_sessions);

	QDomDocument document;

	// Open the file
	if (! file.open(QFile::ReadOnly | QFile::Text))
		throw XinxProjectException(tr("Can't open session file."));

	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;
	if (! document.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
		throw XinxProjectException(tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));

	QDomElement rootSession = document.documentElement();

	// Test if Project File
	if (rootSession.tagName() != "Session")
		throw XinxProjectException(tr("The file isn't a XINX Project Session file"));

	QDomElement sessionNode = rootSession.firstChildElement("Opened");
	if (! sessionNode.isNull())
	{
		QDomElement editor = sessionNode.firstChildElement("Editor");
		while (! editor.isNull())
		{
			(new XinxProjectSessionEditor(this))->loadFromNode(editor);
			editor = editor.nextSiblingElement("Editor");
		}
	}

	QDomElement lastOpenedFile = rootSession.firstChildElement("LastOpenedFile");
	if (! lastOpenedFile.isNull())
	{
		QDomElement file = lastOpenedFile.firstChildElement("File");
		while (! file.isNull())
		{
			m_lastOpenedFile.append(file.attribute("name"));

			file = file.nextSiblingElement("File");
		}
	}
}

void XinxProjectSession::saveToFile(const QString & filename)
{
	QDomDocument document("Session");

	QDomElement root = document.createElement("Session");
	document.appendChild(root);

	QDomElement sessions = document.createElement("Opened");
	root.appendChild(sessions);

	foreach(XinxProjectSessionEditor* session, m_sessions)
	{
		QDomElement editor = document.createElement("Editor");
		sessions.appendChild(editor);

		session->saveToNode(document, editor);
	}

	QDomElement lastOpenedFile = document.createElement("LastOpenedFile");
	root.appendChild(lastOpenedFile);

	foreach(const QString & name, m_lastOpenedFile)
	{
		QDomElement file = document.createElement("File");
		file.setAttribute("name", name);
		lastOpenedFile.appendChild(file);
	}

	QDomElement signets = document.createElement("bookmarks");
	root.appendChild(signets);

	static const int IndentSize = 3;
	if (! filename.isEmpty()) m_filename = filename;
	QFile file(m_filename);

	// Open the file
	if (! file.open(QFile::WriteOnly | QFile::Text))
		throw XinxProjectException(tr("Cannot write file %1:\n%2.").arg(m_filename).arg(file.errorString()));

	// Save the content
	QTextStream text(&file);
	document.save(text, IndentSize);
}

QStringList & XinxProjectSession::lastOpenedFile()
{
	return m_lastOpenedFile;
}

QList<XinxProjectSessionEditor*> & XinxProjectSession::serializedEditors()
{
	return m_sessions;
}

/* XinxProjectSession2 */

XinxProjectSession2::XinxProjectSession2()
{
	m_filename.clear();
}

XinxProjectSession2::XinxProjectSession2(const QString & filename) : m_filename(filename)
{
}

XinxProjectSession2::~XinxProjectSession2()
{
}

void XinxProjectSession2::loadFromFile(const QString & filename)
{
	m_filename = filename;
}


/* PrivateXinxProject */

class PrivateXinxProject
{
public:
	PrivateXinxProject(XinxProject * parent);
	~PrivateXinxProject();

	static QString getValue(const QDomDocument & document, const QString & node);
	static void setValue(QDomDocument & document, const QString & node, const QString & value);

	static QStringList loadList(QDomDocument document, const QString & list, const QString & element);
	static void saveList(QDomDocument & document, const QString & list, const QString & element, const QStringList & slist);

	PrivateXinxProject& operator=(const PrivateXinxProject& p);

	XinxProjectSession * m_session;

	QString m_fileName;
	int m_version;

	QStringList m_activatedPlugin;
	QString m_projectName, m_projectPath;
	QString m_projectRCS;


	QHash<QString,QVariant> m_properties;
private:
	XinxProject * m_parent;
};

PrivateXinxProject::PrivateXinxProject(XinxProject * parent)
{
	m_parent = parent;

	m_session = new XinxProjectSession(parent);
}

PrivateXinxProject::~PrivateXinxProject()
{
	delete m_session;
}

PrivateXinxProject& PrivateXinxProject::operator=(const PrivateXinxProject& p)
{
	m_projectName           = p.m_projectName;
	m_projectPath           = p.m_projectPath;
	m_projectRCS            = p.m_projectRCS;

	return *this;
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

/* XinxProject */

XinxProject::XinxProject() : QObject()
{
	d = new PrivateXinxProject(this);
}

XinxProject::XinxProject(const XinxProject & object) : QObject()
{
	d = new PrivateXinxProject(this);
	*d = *(object.d);
}

XinxProject::XinxProject(const QString & project) : QObject()
{
	d = new PrivateXinxProject(this);
	loadFromFile(project);
}

XinxProject::~XinxProject()
{
	delete d;
}

XinxProject& XinxProject::operator=(const XinxProject& p)
{
	*d = *(p.d);
	return *this;
}

void XinxProject::loadFromFile(const QString & filename)
{
	QFile file(filename);
	QDomDocument document;

	// Open the file
	if (! file.open(QFile::ReadOnly | QFile::Text))
		throw XinxProjectException(tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()));

	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;
	if (! document.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
		throw XinxProjectException(tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));

	QDomElement root = document.documentElement();

	// Test if Project File
	if (root.tagName() != "XinxProject")
		throw XinxProjectException(tr("The file isn't a XINX Project. The root name must be \"XinxProject\", not \"%1\".").arg(root.tagName()));

	d->m_version = PrivateXinxProject::getValue(document, "xinx_version").isEmpty() ? 0 : PrivateXinxProject::getValue(document, "xinx_version").toInt();
	if (d->m_version > XINX_PROJECT_VERSION)
		throw XinxProjectException(tr("The file is a too recent XINX Project"));
	if (d->m_version < XINX_PROJECT_VERSION)
		throw XinxProjectException(tr("The project can't be opened. Please use the XINX Project Wizard."), true);


	if (QFileInfo(filename).isRelative())
	{
		d->m_fileName = QDir(QDir::current().absoluteFilePath(filename)).canonicalPath();
	}
	else
	{
		d->m_fileName = QDir(filename).canonicalPath();
	}

	// Load values
	d->m_projectName         = PrivateXinxProject::getValue(document, "name");
	d->m_projectPath         = QFileInfo(d->m_fileName).absoluteDir().absoluteFilePath(PrivateXinxProject::getValue(document, "project"));
	d->m_projectRCS          = PrivateXinxProject::getValue(document, "rcs");
	d->m_activatedPlugin     = PrivateXinxProject::loadList(document, "activatedPlugin", "name");

	QDomElement propertiesElement = root.firstChildElement("properties");

	d->m_properties.clear();
	QDomElement node = propertiesElement.firstChildElement();
	while (! node.isNull())
	{
		QString type  = node.attribute("type", "QString"),
		                name  = node.tagName(),
		                        value = node.text();

		if ((type == "QStringList") && (! value.isEmpty()))
		{
			d->m_properties[ name ] = QVariant::fromValue(value.split(";;;"));
		}
		else if (! value.isEmpty())
		{
			QVariant::Type variantType = QVariant::nameToType(qPrintable(type));
			QVariant p(variantType);
			p.setValue(value);
			d->m_properties[ name ] = p;
		}

		node = node.nextSiblingElement();
	}

	if (! QDir(d->m_projectPath).exists())
		throw XinxProjectException(tr("Project path (%1) don't exists.").arg(projectPath()));

	try
	{
		QString sessionFileName = d->m_fileName;
		sessionFileName.replace(XINX_PROJECT_EXTENTION, XINX_SESSION_EXTENTION);
		d->m_session->loadFromFile(sessionFileName);
	}
	catch (XinxProjectException e)
	{
		qDebug(qPrintable(e.getMessage()));
	}

	emit changed();
}

void XinxProject::saveToFile(const QString & filename)
{
	if (! filename.isEmpty()) d->m_fileName = filename;
	if (d->m_fileName.isEmpty()) return;

	// Save the session file
	QString sessionFileName = d->m_fileName;
	sessionFileName.replace(XINX_PROJECT_EXTENTION, XINX_SESSION_EXTENTION);
	d->m_session->saveToFile(sessionFileName);

	// Save attributes
	QDomDocument document("XinxProject");
	QDomElement root = document.createElement("XinxProject");
	document.appendChild(root);

	PrivateXinxProject::setValue(document, "xinx_version", QString("%1").arg(XINX_PROJECT_VERSION));
	PrivateXinxProject::setValue(document, "name", d->m_projectName);
	PrivateXinxProject::setValue(document, "project", QFileInfo(d->m_fileName).absoluteDir().relativeFilePath(d->m_projectPath));
	PrivateXinxProject::setValue(document, "rcs", d->m_projectRCS);
	PrivateXinxProject::saveList(document, "activatedPlugin", "name", d->m_activatedPlugin);

	QDomElement propertiesElement = document.createElement("properties");
	root.appendChild(propertiesElement);

	foreach(const QString & key, d->m_properties.keys())
	{
		const QString typeName = d->m_properties[ key ].typeName();
		const QString value = typeName == "QStringList" ? d->m_properties[ key ].toStringList().join(";;;") : d->m_properties[ key ].toString();

		QDomElement propertyElement = document.createElement(key);
		QDomText text = document.createTextNode(value);
		propertyElement.appendChild(text);
		propertyElement.setAttribute("type", typeName);
		propertiesElement.appendChild(propertyElement);
	}

	// Open the file
	static const int IndentSize = 3;
	QFile file(d->m_fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text))
		throw XinxProjectException(QObject::tr("Cannot write file %1:\n%2.").arg(d->m_fileName).arg(file.errorString()));

	QTextStream text(&file);
	document.save(text, IndentSize);
}

void XinxProject::saveOnlySession()
{
	if (d->m_fileName.isEmpty()) return;
	d->m_session->saveToFile();
}

const QString & XinxProject::projectRCS() const
{
	return d->m_projectRCS == "no" ? d->m_projectRCS = QString() : d->m_projectRCS;
}

void XinxProject::setProjectRCS(const QString & value)
{
	if (d->m_projectRCS != value)
	{
		d->m_projectRCS = value;
		emit changed();
	}
}

QString XinxProject::projectName() const
{
	return d->m_projectName;
}

void XinxProject::setProjectName(const QString & value)
{
	if (d->m_projectName != value)
	{
		d->m_projectName = value;
		emit changed();
	}
}

QString XinxProject::projectPath() const
{
	return QDir(d->m_projectPath).absolutePath();
}

void XinxProject::setProjectPath(const QString & value)
{
	if (d->m_projectPath != value)
	{
		d->m_projectPath = value;
		emit changed();
	}
}

QStringList XinxProject::activatedPlugin() const
{
	return d->m_activatedPlugin;
}

void XinxProject::setActivatedPlugin(const QStringList & value)
{
	d->m_activatedPlugin = value;
}

const QString & XinxProject::fileName() const
{
	return d->m_fileName;
}


XinxProjectSession * XinxProject::session() const
{
	return d->m_session;
}

void XinxProject::writeProperty(const QString & key, QVariant value)
{
	if (d->m_properties.value(key) != value)
	{
		d->m_properties[ key ] = value;
		emit changed();
	}
}

QVariant XinxProject::readProperty(const QString & key) const
{
	return d->m_properties.value(key);
}

/* XINXProjectManager */

XINXProjectManager::XINXProjectManager() : m_project(0)
{
	m_session = new XinxProjectSession2;
}

XINXProjectManager::~XINXProjectManager()
{
	if (s_self == this)
		s_self = 0;
	delete m_project;
	delete m_session;
}

XINXProjectManager * XINXProjectManager::self()
{
	if (s_self == 0)
	{
		s_self = new XINXProjectManager();
		XINXStaticDeleter::self()->addObject(s_self);
	}
	return s_self;
}

void XINXProjectManager::setCurrentProject(XinxProject * project)
{
	if (m_project)
		m_project->disconnect(this);

	m_project = project;
	XINXConfig::self()->config().project.lastOpenedProject = project->fileName();
	m_session->loadFromFile(QString(project->fileName()).replace(XINX_PROJECT_EXTENTION, XINX_SESSION_EXTENTION2));
	ContentView2::Manager::self()->initializeDatabase();

	connect(m_project, SIGNAL(changed()), this, SIGNAL(changed()));
	emit changed();
}

QPointer<XinxProject> XINXProjectManager::project() const
{
	return m_project;
}

XinxProjectSession2 * XINXProjectManager::session() const
{
	return m_session;
}

void XINXProjectManager::deleteProject()
{
	if (! m_project) return;

	XinxProject * backup = m_project;

	m_project->disconnect(this);
	m_project = NULL;
	m_session->loadFromFile(QString());
	ContentView2::Manager::self()->initializeDatabase();
	emit changed();

	delete backup;
	XINXConfig::self()->config().project.lastOpenedProject = QString();
}
