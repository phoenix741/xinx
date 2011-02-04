/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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
#include <core/xinxconfig.h>
#include <contentview2/contentview2project.h>
#include <contentview2/contentview2file.h>
#include <contentview2/contentview2node.h>
#include <contentview2/contentview2cache.h>
#include <contentview2/contentview2treemodel.h>
#include <contentview2/contentview2completionmodel.h>
#include <contentview2/contentview2manager.h>
#include <project/xinxproject.h>
#include <editors/models/xsl/xslcontentviewparser.h>
#include <editors/models/xsl/xslcompletionnodemodel.h>
#include <plugins/xinxpluginsloader.h>

// Qt header
#include <QApplication>
#include <QtTest/QtTest>
#include <QTreeView>
#include <QListView>
#include <modeltest.h>

Q_IMPORT_PLUGIN(coreplugin);

class TestContentView2 : public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();

	void testProject_data();
	void testProject();
	void testFile();
	void testNode();
	void testParser_data();
	void testParser();

	void cleanupTestCase();
private:
	int m_rootId;
	QString m_filename;

	ContentView2::Node    m_root;
	ContentView2::Project m_project;
	ContentView2::File    m_file;
};

void TestContentView2::initTestCase()
{
	qApp->setOrganizationName("Shadoware");
	qApp->setOrganizationDomain("Shadoware.Org");
	qApp->setApplicationName("XINX Testlib");

	const QString configDirectory    = QString(".config/%1/%2").arg(qApp->organizationDomain()).arg(qApp->applicationName());
	const QString homeDirectory      = QDir::home().absoluteFilePath(configDirectory);
	const QString datasDirectory     = QDir(homeDirectory).absoluteFilePath("datas");
	const QString pluginsDirectory   = QDir(homeDirectory).absoluteFilePath("plugins");

	QDir::home().mkpath(datasDirectory);
	QDir::home().mkpath(pluginsDirectory);

	// .. for datas ...
	QDir::addSearchPath("datas", datasDirectory);
	QDir::addSearchPath("datas", QDir(QApplication::applicationDirPath()).absoluteFilePath("../../datas"));

	// ... for plugins ...
	QDir::addSearchPath("plugins", pluginsDirectory);
	QDir::addSearchPath("plugins", QDir(QApplication::applicationDirPath()).absoluteFilePath("../../plugins"));

	QFile::remove("datas:contentview.db");

	// Init plugins
	XinxPluginsLoader::self()->loadPlugins();

	// Init Project
	XinxProject::Parameters * project = new XinxProject::Parameters("../doc/examples/imports1/imports.xinx_project");
	XinxProject::Manager::self()->setCurrentProject(project);

	m_filename = QFileInfo("../doc/examples/imports1/imports.xsl").canonicalFilePath();
}

void TestContentView2::testProject_data()
{
	QTest::newRow("No project");
	QTest::newRow("Have project");
}

void TestContentView2::testProject()
{
	try
	{
		try
		{
			m_project = ContentView2::Project(XinxProject::Manager::self()->project());
		}
		catch (ContentView2::ProjectException e)
		{
			m_project.setProjectName(XinxProject::Manager::self()->project()->projectName());
			m_project.setProjectPath(XinxProject::Manager::self()->project()->fileName());
			m_project.create();

			m_project.setProjectName(tr("My test project"));
			m_project.update();
		}
	}
	catch (ContentView2::ProjectException e)
	{
		QFAIL(qPrintable(e.getMessage()));
	}
}

void TestContentView2::testFile()
{
	if (! m_project.isValid())
		QSKIP("No project defined", SkipAll);

	ContentView2::FileContainer container(XinxProject::Manager::self()->project(), m_filename, true);
	try
	{
		QVERIFY(!container.isValid());

		m_file.setProject(m_project);
		m_file.setPath(m_filename);
		m_file.setIsCached(true);
		m_file.setType("XSL");
		m_file.create();

		QVERIFY(container.isValid());

		m_file.destroyNodes();
		m_file.destroyImports();

		m_file.setIsLoaded(false);
		m_file.setDatmod(QFileInfo(m_filename).lastModified());

		m_file.update();

		container.reload();
	}
	catch (ContentView2::FileException e)
	{
		QFAIL(qPrintable(e.getMessage()));
	}
}

void TestContentView2::testNode()
{
	if (! m_file.isValid())
		QSKIP("No file defined", SkipAll);

	try
	{
		m_root.setFile(m_file);
		m_root.setLine(15);
		m_root.setData("NAME", ContentView2::Node::NODE_NAME);
		m_root.setData("NAME", ContentView2::Node::NODE_DISPLAY_NAME);
		m_root.setData("TYPE", ContentView2::Node::NODE_TYPE);
		m_root.setData("UV01", ContentView2::Node::NODE_USER_VALUE);
		m_root.setData("UV02", ContentView2::Node::NODE_USER_VALUE + 1);
		m_root.setData("UV03", ContentView2::Node::NODE_USER_VALUE + 2);
		m_root.setData("UV04", ContentView2::Node::NODE_USER_VALUE + 3);
		m_root.setData("UV05", ContentView2::Node::NODE_USER_VALUE + 4);
		m_root.setData("UV06", ContentView2::Node::NODE_USER_VALUE + 5);
		m_root.setData("UV07", ContentView2::Node::NODE_USER_VALUE + 6);
		m_root.setData("UV08", ContentView2::Node::NODE_USER_VALUE + 7);
		m_root.setData("UV09", ContentView2::Node::NODE_USER_VALUE + 8);
		m_root.setData("UV10", ContentView2::Node::NODE_USER_VALUE + 9);
		m_root.setData("UV11", ContentView2::Node::NODE_USER_VALUE + 10);
		m_root.create();

		m_file.setRoot(m_root);
		m_file.update();
	}
	catch (ContentView2::NodeException e)
	{
		QFAIL(qPrintable(e.getMessage()));
	}
	catch (ContentView2::FileException e)
	{
		QFAIL(qPrintable(e.getMessage()));
	}
}

void TestContentView2::testParser_data()
{
	QTest::newRow("No project");
	QTest::newRow("Have project");
}

void TestContentView2::testParser()
{
	if (! m_root.isValid())
		QSKIP("No root defined", SkipAll);

	XslContentView2Parser * parser = 0;
	try
	{
		ContentView2::Manager::self()->startTransaction();

		parser = new XslContentView2Parser;
		parser->setRootNode(m_root);
		parser->openFilename(m_filename);
		parser->load();
		delete parser;
		parser = 0;

		ContentView2::Manager::self()->commitTransaction();

		m_root.reload();

		QList<int> childs = m_root.childs();
		QCOMPARE(childs.size(), 3);

		QStringList childsName;
		foreach(int childId, childs)
		{
			ContentView2::Node childNode(childId);
			childsName.append(childNode.data(ContentView2::Node::NODE_NAME).toString());
		}

		QVERIFY(childsName.contains("/"));

	}
	catch (ContentView2::ParserException e)
	{
		QFAIL(qPrintable(e.getMessage()));
		ContentView2::Manager::self()->rollbackTransaction();
	}
	catch (ContentView2::NodeException e)
	{
		QFAIL(qPrintable(e.getMessage()));
	}
	delete parser;
}

void TestContentView2::cleanupTestCase()
{
	XinxProject::Manager::self()->deleteProject();
}

QTEST_MAIN(TestContentView2)

#include "testcontentview2.moc"



