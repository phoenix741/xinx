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
#include <modeltest.h>

class TestSnipets : public QObject {
	Q_OBJECT
private slots:
	void initTestCase();

	void testConnection();

	void testEmptyCategoryItemModel();
	void testSelectCategoryItemModel();
	void testSearchCategoryId();

	void testEmptySnipetItemModel();
	void testSelectSnipetItemModel();

	void cleanupTestCase();
private:
};

void TestSnipets::initTestCase() {
	qsrand( time(NULL) );
	QDir::addSearchPath( "datas", QDir( QApplication::applicationDirPath() ).absoluteFilePath( "../../datas" ) );
	QDir::addSearchPath( "datas", QDir( QApplication::applicationDirPath() ).absoluteFilePath( "../../share/xinx/datas" ) );
}

void TestSnipets::testConnection() {
	SnipetManager::self();
	QVERIFY( SnipetManager::self()->database().isValid() );
	QVERIFY( SnipetManager::self()->database().isOpen() );

}

void TestSnipets::testEmptyCategoryItemModel() {
	CategoryItemModel * model = SnipetManager::self()->createCategoryItemModel( this );
	new ModelTest( model );
	QVERIFY( model != 0 );

	delete model;
}

void TestSnipets::testSelectCategoryItemModel() {
	CategoryItemModel * model = SnipetManager::self()->createCategoryItemModel( this );
	model->select();
	new ModelTest( model );
	QVERIFY( model != 0 );

	delete model;
}

void TestSnipets::testSearchCategoryId() {
	CategoryItemModel * model = SnipetManager::self()->createCategoryItemModel( this );
	model->select();
	new ModelTest( model );
	QVERIFY( model != 0 );

	QModelIndex rootIndex = model->index( 0 );
	QVERIFY( ! rootIndex.isValid() );

	if( model->sourceModel()->rowCount() == 0 ) {
		QSKIP( "Empty base", SkipSingle );
		delete model;
		return;
	}

	int row = ( qrand() % model->sourceModel()->rowCount() );
	int testId = model->sourceModel()->index( row, 0 ).data().toInt();
	QModelIndex index = model->index( testId );
	QVERIFY( index.isValid() );

	int id = index.data( CategoryItemModel::CategoryIdRole ).toInt();

	QVERIFY( id == testId );

	delete model;
}

void TestSnipets::testEmptySnipetItemModel() {
	SnipetItemModel * model = SnipetManager::self()->createSnipetItemModel( this );
	new ModelTest( model );
	QVERIFY( model != 0 );

	delete model;
}

void TestSnipets::testSelectSnipetItemModel() {
	SnipetItemModel * model = SnipetManager::self()->createSnipetItemModel( this );
	model->select();
	new ModelTest( model );
	QVERIFY( model != 0 );

	delete model;
}

void TestSnipets::cleanupTestCase() {
}

QTEST_MAIN(TestSnipets)

#include "testsnipets.moc"
