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
#include <contentview/contentviewnode.h>

// Qt header
//#include <QApplication>
#include <QtTest/QtTest>

class TestContentView : public QObject {
	Q_OBJECT
private slots:
	void initTestCase();

	void testCreateAttachNode();

	void cleanupTestCase();
private:
	ContentViewNode * m_nodes;
};

void TestContentView::initTestCase() {
	m_nodes = new ContentViewNode( "root", 0 );
	QVERIFY( m_nodes != 0 );
}

void TestContentView::testCreateAttachNode() {
	QList<ContentViewNode*> list;
	list.append( new ContentViewNode( "child1", list.size() ) );
	list.append( new ContentViewNode( "child2", list.size() ) );
	list.append( new ContentViewNode( "child3", list.size() ) );
	list.append( new ContentViewNode( "child4", list.size() ) );
	list.append( new ContentViewNode( "child5", list.size() ) );
	list.append( new ContentViewNode( "child6", list.size() ) );
	list.append( new ContentViewNode( "child7", list.size() ) );

	foreach( ContentViewNode * n, list ) {
		n->attach( m_nodes );
	}

	foreach( ContentViewNode * n, list ) {
		QVERIFY( m_nodes->childs().contains( n ) );
	}
}

void TestContentView::cleanupTestCase() {
	m_nodes->deleteInstance();
}

QTEST_MAIN(TestContentView)

#include "testcontentview.moc"
