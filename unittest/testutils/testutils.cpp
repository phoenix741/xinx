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
#include <snipets/snipetitemmodel.h>
#include <utils/recursivesortfilterproxymodel.h>

// Qt header
#include <QtTest/QtTest>
#include <QTreeView>
#include <QApplication>
#include <modeltest.h>

class TestUtils : public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();

	void testEmptyRecursiveModel();
	void testFilterRecursiveModel_data();
	void testFilterRecursiveModel();

	void cleanupTestCase();
private:
	SnipetItemModel * m_srcModel;
	RecursiveSortFilterProxyModel * m_model;
	QTreeView * m_tree;
};

void TestUtils::initTestCase()
{
	QDir::addSearchPath("datas", QDir(QApplication::applicationDirPath()).absoluteFilePath("../../datas"));
	QDir::addSearchPath("datas", QDir(QApplication::applicationDirPath()).absoluteFilePath("../datas"));
	QDir::addSearchPath("datas", QDir(QApplication::applicationDirPath()).absoluteFilePath("../share/xinx/datas"));

	QVERIFY(SnipetManager::self()->database().isValid());
	QVERIFY(SnipetManager::self()->database().isOpen());

	m_model       = new RecursiveSortFilterProxyModel(this);
	m_srcModel = SnipetManager::self()->createSnipetItemModel(m_model);
	m_srcModel->select();

	m_model->setSourceModel(m_srcModel);

	m_tree = new QTreeView;
	m_tree->setModel(m_model);

	m_tree->setWindowTitle(QTreeView::tr("View"));
	m_tree->resize(640, 480);
	m_tree->show();
}

void TestUtils::testEmptyRecursiveModel()
{
	new ModelTest(m_model);
}

void TestUtils::testFilterRecursiveModel_data()
{
	QTest::addColumn<QString>("filter");
	QTest::addColumn<bool>("showAllChild");
	QTest::addColumn<bool>("dynamicSort");
	QTest::addColumn<bool>("insensitive");
	QTest::addColumn<bool>("includeIndex");

	QTest::newRow("no filter") << QString() << true << true << true << true;
	QTest::newRow("filter empty") << "ABCDEFGH" << true << true << true << true;
	QTest::newRow("filter find") << "Codzn" << true << true << true << true;
	QTest::newRow("no filter") << QString() << false << true << true << true;
	QTest::newRow("filter empty") << "ABCDEFGH" << false << true << true << true;
	QTest::newRow("filter find") << "Codzn" << false << true << true << true;
	QTest::newRow("no filter") << QString() << true << false << true << true;
	QTest::newRow("filter empty") << "ABCDEFGH" << true << false << true << true;
	QTest::newRow("filter find") << "Codzn" << true << false << true << true;
	QTest::newRow("no filter") << QString() << false << false << true << true;
	QTest::newRow("filter empty") << "ABCDEFGH" << false << false << true << true;
	QTest::newRow("filter find") << "Codzn" << false << false << true << true;
	QTest::newRow("no filter") << QString() << true << true << false << true;
	QTest::newRow("filter empty") << "ABCDEFGH" << true << true << false << true;
	QTest::newRow("filter find") << "Codzn" << true << true << false << true;
	QTest::newRow("no filter") << QString() << false << true << false << true;
	QTest::newRow("filter empty") << "ABCDEFGH" << false << true << false << true;
	QTest::newRow("filter find") << "Codzn" << false << true << false << true;
	QTest::newRow("no filter") << QString() << true << false << false << true;
	QTest::newRow("filter empty") << "ABCDEFGH" << true << false << false << true;
	QTest::newRow("filter find") << "Codzn" << true << false << false << true;
	QTest::newRow("no filter") << QString() << false << false << false << true;
	QTest::newRow("filter empty") << "ABCDEFGH" << false << false << false << true;
	QTest::newRow("filter find") << "Codzn" << false << false << false << true;
	QTest::newRow("no filter") << QString() << true << true << true << false;
	QTest::newRow("filter empty") << "ABCDEFGH" << true << true << true << false;
	QTest::newRow("filter find") << "Codzn" << true << true << true << false;
	QTest::newRow("no filter") << QString() << false << true << true << false;
	QTest::newRow("filter empty") << "ABCDEFGH" << false << true << true << false;
	QTest::newRow("filter find") << "Codzn" << false << true << true << false;
	QTest::newRow("no filter") << QString() << true << false << true << false;
	QTest::newRow("filter empty") << "ABCDEFGH" << true << false << true << false;
	QTest::newRow("filter find") << "Codzn" << true << false << true << false;
	QTest::newRow("no filter") << QString() << false << false << true << false;
	QTest::newRow("filter empty") << "ABCDEFGH" << false << false << true << false;
	QTest::newRow("filter find") << "Codzn" << false << false << true << false;
	QTest::newRow("no filter") << QString() << true << true << false << false;
	QTest::newRow("filter empty") << "ABCDEFGH" << true << true << false << false;
	QTest::newRow("filter find") << "Codzn" << true << true << false << false;
	QTest::newRow("no filter") << QString() << false << true << false << false;
	QTest::newRow("filter empty") << "ABCDEFGH" << false << true << false << false;
	QTest::newRow("filter find") << "Codzn" << false << true << false << false;
	QTest::newRow("no filter") << QString() << true << false << false << false;
	QTest::newRow("filter empty") << "ABCDEFGH" << true << false << false << false;
	QTest::newRow("filter find") << "Codzn" << true << false << false << false;
	QTest::newRow("no filter") << QString() << false << false << false << false;
	QTest::newRow("filter empty") << "ABCDEFGH" << false << false << false << false;
	QTest::newRow("filter find") << "Codzn" << false << false << false << false;
}

void TestUtils::testFilterRecursiveModel()
{
	QFETCH(QString, filter);
	QFETCH(bool, showAllChild);
	QFETCH(bool, dynamicSort);
	QFETCH(bool, insensitive);
	QFETCH(bool, includeIndex);

	m_model->setFilterRegExp(filter);
	m_model->setShowAllChild(showAllChild);
	m_model->setDynamicSortFilter(dynamicSort);
	m_model->setFilterCaseSensitivity(insensitive ? Qt::CaseInsensitive : Qt::CaseSensitive);
	m_model->setIncludeIndex(includeIndex ? QModelIndexList() << m_model->mapToSource(m_model->index(0, 0)) : QModelIndexList());

	m_tree->expandAll();
	qApp->processEvents();
}

void TestUtils::cleanupTestCase()
{

}

QTEST_MAIN(TestUtils)

#include "testutils.moc"
