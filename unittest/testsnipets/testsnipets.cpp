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

// Qt header
#include <QtTest/QtTest>
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
	void testAddCategoryItemModel();
	void testSearchCategoryId();
	void testRemoveCategoryItemModel();

	/*
	void testEmptySnipetItemModel();
	void testSelectSnipetItemModel();
	*/

	void cleanupTestCase();
private:
	CategoryItemModel * m_categoryModel;
	SnipetItemModel * m_snipetModel;
};

void TestSnipets::initTestCase() {
	qsrand( time(NULL) );
	QDir::addSearchPath( "datas", QDir( QApplication::applicationDirPath() ).absoluteFilePath( "../../datas" ) );
	QDir::addSearchPath( "datas", QDir( QApplication::applicationDirPath() ).absoluteFilePath( "../../share/xinx/datas" ) );

	SnipetManager::self()->database().transaction();
	m_categoryModel = SnipetManager::self()->createCategoryItemModel( this );
	m_snipetModel = SnipetManager::self()->createSnipetItemModel( this );
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
}

void TestSnipets::testRemoveCategoryItemModel() {
	QSqlQuery query( "DELETE FROM categories WHERE id not in (select parent_id from categories)", SnipetManager::self()->database() );

	m_categoryModel->select();
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

	QVERIFY( id == testId );
}

/*
void TestSnipets::testEmptySnipetItemModel() {
	new ModelTest( m_snipetModel );
}

void TestSnipets::testSelectSnipetItemModel() {
	m_snipetModel->select();
}
*/

void TestSnipets::cleanupTestCase() {
	delete m_snipetModel;
	delete m_categoryModel;
	SnipetManager::self()->database().rollback();
}

QTEST_MAIN(TestSnipets)

#include "testsnipets.moc"