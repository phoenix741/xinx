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
#include <contentview2/contentview2node.h>
#include <contentview2/contentview2cache.h>
#include <contentview2/contentview2treemodel.h>
#include <project/xinxproject.h>
#include <editors/models/xsl/xslcv2parser.h>
#include <plugins/xinxpluginsloader.h>

// Qt header
#include <QApplication>
#include <QtTest/QtTest>
#include <QTreeWidget>
#include <modeltest.h>

Q_IMPORT_PLUGIN(coreplugin);

class TestContentView2 : public QObject {
	Q_OBJECT
private slots:
	void initTestCase();

	void createCache();
	void testParseRootFile_data();
	void testParseRootFile();
	void testParseImportsFile_data();
	void testParseImportsFile();

	void testTreeModel();

	void cleanupTestCase();
private:
	int m_rootId;
	ContentView2::Cache * m_cache;
	QStringList m_imports;
};

void TestContentView2::initTestCase() {
	// Init plugins
	QDir::addSearchPath( "plugins", QDir( QApplication::applicationDirPath() ).absoluteFilePath( "../../plugins" ) );
	XinxPluginsLoader::self()->loadPlugins();

	// Init Project
	QFile( "../doc/examples/imports1/imports.xinx_session2" ).remove();
	XinxProject * project = new XinxProject( "../doc/examples/imports1/imports.xinx_project" );
	XINXProjectManager::self()->setCurrentProject( project );

	m_rootId = -1;
}

void TestContentView2::createCache() {
	m_cache = new ContentView2::Cache( XINXProjectManager::self()->project() );
}

void TestContentView2::testParseRootFile_data() {
	QTest::newRow("Empty session file");
	QTest::newRow("Existing session file");
}

void TestContentView2::testParseRootFile() {
	QSqlDatabase db = XINXProjectManager::self()->session()->database();

	XslContentView2Parser * parser = 0;
	try {
		db.transaction();

		QString filename = QFileInfo( "../doc/examples/imports1/imports.xsl" ).canonicalFilePath();

		m_rootId = m_cache->createRootId( filename, true, false );

		parser = new XslContentView2Parser;
		parser->setPersistent( true );
		parser->setRootNode( ContentView2::Node( db, m_rootId ) );
		parser->setFilename( filename );

		db.commit();

		m_cache->addToCache( parser );
		m_cache->wait();
		m_imports = parser->imports();

		ContentView2::Node rootNode( db, m_rootId );
		QList<int> childs = rootNode.childs( db );

		QCOMPARE( childs.size(), 2 );

		int childId = childs.at( 0 );
		ContentView2::Node childNode( db, childId );
		QCOMPARE( childNode.data( ContentView2::Node::NODE_NAME ).toString(), QString( "/" ) );
		QCOMPARE( childNode.childs( db ).size(), 0 );

	} catch( ContentView2::ParserException e ) {
		QFAIL( qPrintable( e.getMessage() ) );
		db.rollback();
	}
	delete parser;
}

void TestContentView2::testParseImportsFile_data() {
	QTest::newRow("Empty session file");
	QTest::newRow("Existing session file");
}

void TestContentView2::testParseImportsFile() {
	if( m_imports.isEmpty() ) {
		QSKIP( "No import to test ...", SkipSingle );
	}

	foreach( const QString & import, m_imports ) {
		QVERIFY2( QFileInfo( import ).exists(), "Import must exists: probleme in resolution of import" );
	}
	m_cache->loadCache( m_imports );
	m_cache->wait();

	try {
		QSqlDatabase db = XINXProjectManager::self()->session()->database();
		db.transaction();

		QString filename = QFileInfo( "../doc/examples/imports1/patients.xsl" ).canonicalFilePath();
		uint rootId = m_cache->createRootId( filename, true );

		db.rollback();

		ContentView2::Node rootNode( db, rootId );
		QList<int> childs = rootNode.childs( db );

		QCOMPARE( childs.size(), 3 );

		foreach( int childId, childs ) {
			ContentView2::Node childNode( db, childId );
			QCOMPARE( childNode.childs( db ).size(), 0 );
		}
	} catch( ContentView2::NodeException e ) {
		QFAIL( qPrintable( e.getMessage() ) );
	}

}

void TestContentView2::testTreeModel() {
	if( m_rootId == -1 ) {
		QSKIP( "No root ID defined ...", SkipSingle );
	}

	QTreeView * tree = new QTreeView;
	tree->setWindowTitle( QTreeView::tr("Content View 2") );
	tree->resize(640, 480);
	tree->show();

	ContentView2::TreeModel * model = new ContentView2::TreeModel( XINXProjectManager::self()->session()->database(), m_rootId, this );
	tree->setModel( model );

	new ModelTest( model , model );
	model->select();

	tree->expandAll();
	qApp->processEvents();

	delete tree;
	delete model;
}

void TestContentView2::cleanupTestCase() {
	delete m_cache;
	XINXProjectManager::self()->deleteProject();
	QFile( "../doc/examples/imports1/imports.xinx_session2" ).remove();
}

QTEST_MAIN(TestContentView2)

#include "testcontentview2.moc"



