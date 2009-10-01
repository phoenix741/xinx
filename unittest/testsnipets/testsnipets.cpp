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
#include <core/xinxconfig.h>
#include <snipets/snipetmanager.h>
#include <snipets/categoryitemmodel.h>
#include <snipets/snipetitemmodel.h>
#include <snipets/snipetdockitemmodel.h>
#include <snipets/categorytreeview.h>

// Qt header
#include <QtTest/QtTest>
#include <QTreeView>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <modeltest.h>

class TestSnipets : public QObject {
	Q_OBJECT
private slots:
	void initTestCase();

	void testConnection();

	void testEmptyCategoryItemModel();
	void testSelectCategoryItemModel();
	void testCategoryView();
	void testAddCategoryItemModel();
	void testSearchCategoryId();
	void testRemoveCategoryItemModel();

	void testEmptySnipetItemModel();
	void testSelectSnipetItemModel();
	void testSearchSnipetId();

	void testEmptySnipetDockItemModel();
	void testSelectSnipetDockItemModel();

	void testExport();
	void testImport();

	void testCategoryDeletion();

	void testDeleteAll();

	void cleanupTestCase();
private:
	CategoryItemModel * m_categoryModel;
	SnipetItemModel * m_snipetModel;
	SnipetDockItemModel * m_snipetDockModel;
	QTreeView * m_treeCategory, * m_treeSnipet, * m_treeDock;
};

void TestSnipets::initTestCase() {
	qsrand( time(NULL) );
	QDir::addSearchPath( "datas", QDir( QApplication::applicationDirPath() ).absoluteFilePath( "../datas" ) );
	QDir::addSearchPath( "datas", QDir( QApplication::applicationDirPath() ).absoluteFilePath( "../../datas" ) );
	QDir::addSearchPath( "datas", QDir( QApplication::applicationDirPath() ).absoluteFilePath( "../share/xinx/datas" ) );

	SnipetManager::self()->database().transaction();
	m_categoryModel = SnipetManager::self()->createCategoryItemModel( this );
	m_snipetModel = SnipetManager::self()->createSnipetItemModel( this );
	m_snipetDockModel = SnipetManager::self()->createSnipetDockItemModel( this );

	m_treeCategory = new QTreeView;
	m_treeCategory->setModel( m_categoryModel );

	m_treeCategory->setWindowTitle( QTreeView::tr("Category View") );
	m_treeCategory->resize(640, 480);
	m_treeCategory->show();

	m_treeSnipet = new QTreeView;
	m_treeSnipet->setModel( m_snipetModel );

	m_treeSnipet->setWindowTitle( QTreeView::tr("Snipet View") );
	m_treeSnipet->resize(640, 480);
	m_treeSnipet->show();

	m_treeDock = new QTreeView;
	m_treeDock->setModel( m_snipetDockModel );

	m_treeDock->setWindowTitle( QTreeView::tr("Dock View") );
	m_treeDock->resize(640, 480);
	m_treeDock->show();
}

void TestSnipets::testConnection() {
	SnipetManager::self();
	QVERIFY( SnipetManager::self()->database().isValid() );
	QVERIFY( SnipetManager::self()->database().isOpen() );
}

void TestSnipets::testEmptyCategoryItemModel() {
	new ModelTest( m_categoryModel );
}

void TestSnipets::testSelectCategoryItemModel() {
	m_categoryModel->select();
	m_treeCategory->expandAll();
	qApp->processEvents();
}

void TestSnipets::testCategoryView() {
	CategoryTreeView view;
	view.setModel( m_categoryModel );
	view.show();
	view.setCategoryId( 1 );

	int row = ( qrand() % m_categoryModel->sourceModel()->rowCount() );
	int testId = m_categoryModel->sourceModel()->index( row, 0 ).data().toInt();

	view.setCategoryId( testId );

	QCOMPARE( view.categoryId(), testId );
}

void TestSnipets::testAddCategoryItemModel() {
	int row = ( qrand() % m_categoryModel->sourceModel()->rowCount() );
	int testId = m_categoryModel->sourceModel()->index( row, 0 ).data().toInt();

	QSqlQuery query( "INSERT INTO categories(parent_id, name) VALUES(:parentCategory, :name)", SnipetManager::self()->database() );
	query.bindValue( ":parent_id", testId );
	query.bindValue( ":name", QString("testDoubleSelectCategoryItemModel(%1)").arg( testId ) );

	bool result = query.exec();
	QVERIFY( result );

	m_categoryModel->select();
	m_treeCategory->expandAll();
	qApp->processEvents();
}

void TestSnipets::testRemoveCategoryItemModel() {
	QSqlQuery query( "DELETE FROM categories WHERE id not in (select distinct parent_id from categories) and id not in (select distinct category_id from snipets)", SnipetManager::self()->database() );

	m_categoryModel->select();
	m_treeCategory->expandAll();
	qApp->processEvents();
}

void TestSnipets::testSearchCategoryId() {
	QModelIndex rootIndex = m_categoryModel->index( 0 );
	QVERIFY( ! rootIndex.isValid() );

	if( m_categoryModel->sourceModel()->rowCount() == 0 ) {
		QSKIP( "Empty base", SkipSingle );
		return;
	}

	int row = ( qrand() % m_categoryModel->sourceModel()->rowCount() );
	int testId = m_categoryModel->sourceModel()->index( row, 0 ).data().toInt();
	QModelIndex index = m_categoryModel->index( testId );
	QVERIFY( index.isValid() );

	int id = index.data( CategoryItemModel::CategoryIdRole ).toInt();

	QCOMPARE( id, testId );
}

void TestSnipets::testEmptySnipetItemModel() {
	new ModelTest( m_snipetModel );
}

void TestSnipets::testSelectSnipetItemModel() {
	m_snipetModel->select();
	m_treeSnipet->expandAll();
	qApp->processEvents();
}

void TestSnipets::testSearchSnipetId() {
	QModelIndex rootIndex = m_snipetModel->index( true, 0 );
	QVERIFY( ! rootIndex.isValid() );

	if( m_snipetModel->sourceModel()->rowCount() == 0 ) {
		QSKIP( "Empty base", SkipSingle );
		return;
	}

	int row = ( qrand() % m_snipetModel->sourceModel()->rowCount() );
	bool isCategory = m_snipetModel->sourceModel()->index( row, 6 ).data().toString() == "CATEGORY";
	int testId = m_snipetModel->sourceModel()->index( row, 0 ).data().toInt();
	QModelIndex index = m_snipetModel->index( isCategory, testId );
	QVERIFY( index.isValid() );

	int id = index.data( SnipetItemModel::SnipetIdRole ).toInt();

	QCOMPARE( id, testId );
}

void TestSnipets::testEmptySnipetDockItemModel() {
	new ModelTest( m_snipetDockModel );
}

void TestSnipets::testSelectSnipetDockItemModel() {
	m_snipetDockModel->select();
	m_treeDock->expandAll();
	qApp->processEvents();
}

void TestSnipets::testExport() {
	QString filename = QDir::temp().absoluteFilePath( "testImportExport.xml" );

	QList<int> ids = SnipetManager::self()->snipets();

	SnipetList list;
	QVERIFY( SnipetManager::self()->exportSnipetList( ids, &list ) );
	list.saveToFile( filename );

	m_snipetModel->select();
	m_treeSnipet->expandAll();
	qApp->processEvents();
}

void TestSnipets::testImport() {
	QString filename = QDir::temp().absoluteFilePath( "testImportExport.xml" );

	SnipetList list;
	list.loadFromFile( filename );
	QVERIFY( SnipetManager::self()->importSnipetList( list ) );

	m_snipetModel->select();
	m_treeSnipet->expandAll();
	qApp->processEvents();
}

void TestSnipets::testCategoryDeletion() {
	int firstCat, secondCat;
	QSqlQuery insertQuery( "INSERT INTO categories(parent_id, name) VALUES(:parentCategory, :name)", SnipetManager::self()->database() );
	insertQuery.bindValue( ":parentCategory", 1 );
	insertQuery.bindValue( ":name", "A" );

	bool result = insertQuery.exec();
	QVERIFY( result );

	firstCat = insertQuery.lastInsertId().toInt();

	insertQuery.bindValue( ":parentCategory", firstCat );
	insertQuery.bindValue( ":name", "B" );

	result = insertQuery.exec();
	QVERIFY( result );

	secondCat = insertQuery.lastInsertId().toInt();

	m_snipetModel->select();

	QSqlQuery removeQuery( "DELETE FROM categories WHERE id=:id", SnipetManager::self()->database() );
	removeQuery.bindValue( ":id", secondCat );
	result = removeQuery.exec();
	QVERIFY( result );
	removeQuery.bindValue( ":id", firstCat );
	result = removeQuery.exec();
	QVERIFY( result );

	m_snipetModel->select();
}

void TestSnipets::testDeleteAll() {
	QList<int> ids = SnipetManager::self()->snipets();

	foreach( int id, ids ) {
		SnipetManager::self()->removeSnipet( id );
		m_snipetModel->select();
		m_treeSnipet->expandAll();
		qApp->processEvents();
	}

	ids = SnipetManager::self()->snipets();

	QCOMPARE( ids.size(), 0 );
}

void TestSnipets::cleanupTestCase() {
	SnipetManager::self()->database().rollback();
}

QTEST_MAIN(TestSnipets)

#include "testsnipets.moc"
