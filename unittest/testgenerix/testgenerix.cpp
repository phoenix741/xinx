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
#include <plugins/xinxpluginsloader.h>
#include <dictionaryparser.h>
#include <gceconfigurationxmlparser.h>
#include <gceconfigurationxmlparser2.h>
#include <contentview2/contentview2file.h>

// Qt header
#include <QApplication>
#include <QtTest/QtTest>
#include <QSqlError>

Q_IMPORT_PLUGIN(coreplugin);

class TestGenerix : public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();

	void testDictionaryNoBase();
	void testDictionaryBase();

	void testInsert();

	void testConfiguration140_1();
	void testConfiguration140_2();
	void testConfiguration150_1();
	void testConfiguration150_2();

	void cleanupTestCase();
private:
};

void TestGenerix::initTestCase()
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
}

void TestGenerix::testDictionaryNoBase()
{
	if(!QFileInfo("testgenerix/dico_fra_gce140.xml").exists())
		QSKIP("No dictionary", SkipSingle);

	QBENCHMARK
	{
		ContentView2::Manager::self()->database().transaction();

		const QString uuid = QUuid::createUuid().toString();

		ContentView2::File testFile;
		testFile.setPath(uuid);
		testFile.setIsCached(false);
		testFile.setIsLoaded(false);
		testFile.setProjectId(0);
		testFile.setSelection("*");
		testFile.create (ContentView2::Manager::self()->database());

		ContentView2::Node rootNode;
		rootNode.setFile(testFile);
		rootNode.setData("ROOT_NODE", ContentView2::Node::NODE_NAME);
		rootNode.setData("ROOT", ContentView2::Node::NODE_DISPLAY_NAME);
		rootNode.setData("ROOT", ContentView2::Node::NODE_TYPE);
		rootNode.create (ContentView2::Manager::self()->database());

		testFile.setRoot(rootNode);
		testFile.update (ContentView2::Manager::self()->database());

		DictionaryParser * parser = new DictionaryParser();
		parser->setFilename("testgenerix/dico_fra_gce140.xml");

		try
		{
			parser->load();
			ContentView2::Manager::self()->database().commit();
		}
		catch(const ContentView2::DatabaseException & e)
		{
			ContentView2::Manager::self()->database().rollback();
			QFAIL(e.what());
		}
	}
}

void TestGenerix::testDictionaryBase()
{
	if(!QFileInfo("testgenerix/dico_fra_gce140.xml").exists())
		QSKIP("No dictionary", SkipSingle);

	QBENCHMARK
	{
		ContentView2::Manager::self()->database().transaction();

		const QString uuid = QUuid::createUuid().toString();

		ContentView2::File testFile;
		testFile.setPath(uuid);
		testFile.setIsCached(false);
		testFile.setIsLoaded(false);
		testFile.setProjectId(0);
		testFile.setSelection("*");
		testFile.create (ContentView2::Manager::self()->database());

		ContentView2::Node rootNode;
		rootNode.setFile(testFile);
		rootNode.setData("ROOT_NODE", ContentView2::Node::NODE_NAME);
		rootNode.setData("ROOT", ContentView2::Node::NODE_DISPLAY_NAME);
		rootNode.setData("ROOT", ContentView2::Node::NODE_TYPE);
		rootNode.create (ContentView2::Manager::self()->database());

		testFile.setRoot(rootNode);
		testFile.update (ContentView2::Manager::self()->database());

		DictionaryParser * parser = new DictionaryParser();
		parser->setRootNode(rootNode);
		parser->setDatabase(ContentView2::Manager::self()->database());
		parser->setFilename("testgenerix/dico_fra_gce140.xml");

		try
		{
			parser->load();
			ContentView2::Manager::self()->database().commit();
		}
		catch(const ContentView2::DatabaseException & e)
		{
			ContentView2::Manager::self()->database().rollback();
			QFAIL(e.what());
		}
	}
}

void TestGenerix::testInsert()
{
	QBENCHMARK
	{
		const QString uuid = QUuid::createUuid().toString();

		ContentView2::File testFile;
		testFile.setPath(uuid);
		testFile.setIsCached(false);
		testFile.setIsLoaded(false);
		testFile.setProjectId(0);
		testFile.setSelection("*");
		testFile.create (ContentView2::Manager::self()->database());

		ContentView2::Node rootNode;
		rootNode.setFile(testFile);
		rootNode.setData("ROOT_NODE", ContentView2::Node::NODE_NAME);
		rootNode.setData("ROOT", ContentView2::Node::NODE_DISPLAY_NAME);
		rootNode.setData("ROOT", ContentView2::Node::NODE_TYPE);
		rootNode.create (ContentView2::Manager::self()->database());

		testFile.setRoot(rootNode);
		testFile.update (ContentView2::Manager::self()->database());

		ContentView2::Manager::self()->database().transaction();

		QSqlQuery insertQuery(ContentView2::Manager::self()->database());

		insertQuery.prepare("INSERT INTO cv_node(name, type, display_name, file_id, hash) "
							"VALUES(:name, :type, :display_name, :file_id, :hash)");

		for(int i = 0; i < 15000; i++)
		{

			insertQuery.bindValue(":name", QString("%1").arg(i));
			insertQuery.bindValue(":type", "NODE_TEST");
			insertQuery.bindValue(":display_name", QString("%1").arg(i));
			insertQuery.bindValue(":file_id", testFile.fileId());
			insertQuery.bindValue(":hash", "1");

			bool result = insertQuery.exec();
			if(!result)
				QFAIL(qPrintable(insertQuery.lastError().text()));
		}


		ContentView2::Manager::self()->database().commit();
	}
}

void TestGenerix::testConfiguration140_1()
{
	if(!QFileInfo("testgenerix/configuration140.xml").exists())
		QSKIP("No dictionary", SkipSingle);

	QBENCHMARK
	{
		GceConfigurationXmlParser parser;
		parser.m_parent = 0;
		parser.loadFromFile("testgenerix/configuration140.xml");
	}
}

void TestGenerix::testConfiguration140_2()
{
	if(!QFileInfo("testgenerix/configuration140.xml").exists())
		QSKIP("No dictionary", SkipSingle);

	QBENCHMARK
	{
		GceConfigurationXmlParser2 parser;
		parser.m_parent = 0;
		parser.loadFromFile("testgenerix/configuration140.xml");
	}
}

void TestGenerix::testConfiguration150_1()
{
	if(!QFileInfo("testgenerix/configuration150.xml").exists())
		QSKIP("No dictionary", SkipSingle);

	QBENCHMARK
	{
		GceConfigurationXmlParser parser;
		parser.m_parent = 0;
		parser.loadFromFile("testgenerix/configuration150.xml");
	}
}

void TestGenerix::testConfiguration150_2()
{
	if(!QFileInfo("testgenerix/configuration150.xml").exists())
		QSKIP("No dictionary", SkipSingle);

	QBENCHMARK
	{
		GceConfigurationXmlParser2 parser;
		parser.m_parent = 0;
		parser.loadFromFile("testgenerix/configuration150.xml");
	}
}

void TestGenerix::cleanupTestCase()
{

}

QTEST_MAIN(TestGenerix)

#include "testgenerix.moc"



