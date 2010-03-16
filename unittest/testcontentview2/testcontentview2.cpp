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

	QString configDirectory = QString(".config/%1/%2").arg(qApp->organizationDomain()).arg(qApp->applicationName());
	QDir::home().mkpath(configDirectory);

	QDir    path            = QDir(QDir::home().absoluteFilePath(configDirectory));
	QFile::remove( path.absoluteFilePath("session.db") );

	// Init plugins
	QDir::addSearchPath("datas", QDir(QApplication::applicationDirPath()).absoluteFilePath("../../datas"));
	QDir::addSearchPath("plugins", QDir(QApplication::applicationDirPath()).absoluteFilePath("../../plugins"));
	XinxPluginsLoader::self()->loadPlugins();

	// Init Project
	XinxProject * project = new XinxProject("../doc/examples/imports1/imports.xinx_project");
	XINXProjectManager::self()->setCurrentProject(project);

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
			m_project = ContentView2::Project(ContentView2::Manager::self()->database(), XINXProjectManager::self()->project());
		}
		catch(ContentView2::ProjectException e)
		{
			m_project.setProjectName(XINXProjectManager::self()->project()->projectName());
			m_project.setProjectPath(XINXProjectManager::self()->project()->fileName());
			m_project.create(ContentView2::Manager::self()->database());

			m_project.setProjectName( tr("My test project") );
			m_project.update(ContentView2::Manager::self()->database());
		}
	}
	catch(ContentView2::ProjectException e)
	{
		QFAIL(qPrintable(e.getMessage()));
	}
}

void TestContentView2::testFile()
{
	if(! m_project.isValid())
		QSKIP("No project defined", SkipAll);

	ContentView2::FileContainer container(XINXProjectManager::self()->project(), m_filename, true);
	try
	{
		QVERIFY(!container.isValid(ContentView2::Manager::self()->database()));

		m_file.setProject(m_project);
		m_file.setPath(m_filename);
		m_file.setIsCached(true);
		m_file.setType("XSL");
		m_file.create(ContentView2::Manager::self()->database());

		QVERIFY(container.isValid(ContentView2::Manager::self()->database()));

		m_file.destroyNodes(ContentView2::Manager::self()->database());
		m_file.destroyImports(ContentView2::Manager::self()->database());

		m_file.setIsLoaded(false);
		m_file.setDatmod(QFileInfo(m_filename).lastModified());

		m_file.update(ContentView2::Manager::self()->database());

		container.reload(ContentView2::Manager::self()->database());
	}
	catch(ContentView2::FileException e)
	{
		QFAIL(qPrintable(e.getMessage()));
	}
}

void TestContentView2::testNode()
{
	if(! m_file.isValid())
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
		m_root.create(ContentView2::Manager::self()->database());

		m_file.setRoot(m_root);
		m_file.update(ContentView2::Manager::self()->database());
	}
	catch(ContentView2::NodeException e)
	{
		QFAIL(qPrintable(e.getMessage()));
	}
	catch(ContentView2::FileException e)
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
	if(! m_root.isValid())
		QSKIP("No root defined", SkipAll);

	QSqlDatabase db = ContentView2::Manager::self()->database();

	XslContentView2Parser * parser = 0;
	try
	{
		db.transaction();

		parser = new XslContentView2Parser;
		parser->setRootNode(m_root);
		parser->setFilename(m_filename);
		parser->setDatabase(db);
		parser->load();
		delete parser;
		parser = 0;

		db.commit();

		m_root.reload(db);

		QList<int> childs = m_root.childs( db );
		QCOMPARE( childs.size(), 3 );

		int childId = childs.at( 1 );
		ContentView2::Node childNode( db, childId );
		QCOMPARE( childNode.data( ContentView2::Node::NODE_NAME ).toString(), QString( "/" ) );
		QCOMPARE( childNode.childs( db ).size(), 0 );

	}
	catch(ContentView2::ParserException e)
	{
		QFAIL(qPrintable(e.getMessage()));
		db.rollback();
	}
	catch(ContentView2::NodeException e)
	{
		QFAIL(qPrintable(e.getMessage()));
	}
	delete parser;
}

void TestContentView2::cleanupTestCase()
{
	XINXProjectManager::self()->deleteProject();
}

QTEST_MAIN(TestContentView2)

#include "testcontentview2.moc"



