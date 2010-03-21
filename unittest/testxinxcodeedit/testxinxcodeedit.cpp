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
#include <editors/xinxcodeedit.h>
#include <editors/xinxlanguagefactory.h>

// QCodeEdit header
#include <qlinemarksinfocenter.h>

// Qt header
//#include <QApplication>
#include <QtTest/QtTest>
#include <QRegExp>

class TestXinxCodeEdit: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void testGetSetPlainText();

	void testFindTextNormal_data();
	void testFindTextNormal();

	void testFindTextRegExp_data();
	void testFindTextRegExp();

	void testTextUnderCursor_data();
	void testTextUnderCursor();

	void testTabStop_data();
	void testTabStop();

	void testUpperCase();
	void testLowerCase();

	void testDuplicateLines_data();
	void testDuplicateLines();

	void testMoveLineUp_data();
	void testMoveLineUp();

	void testMoveLineDown_data();
	void testMoveLineDown();

	void testBookmark();
	void testError();

	void cleanupTestCase();
private:
	XinxCodeEdit * m_edit;
};

void TestXinxCodeEdit::initTestCase()
{
	QLineMarksInfoCenter::instance()->loadMarkTypes("../../libxinx/marks.qxm");
	QLineMarksInfoCenter::instance()->loadMarkTypes("../libxinx/marks.qxm");
	QLineMarksInfoCenter::instance()->loadMarkTypes("./libxinx/marks.qxm");

	XINXConfig::self()->languageFactory()->addDefinitionPath("../../libxinx");
	XINXConfig::self()->languageFactory()->addDefinitionPath("../libxinx");
	XINXConfig::self()->languageFactory()->addDefinitionPath("./libxinx");

	m_edit = new XinxCodeEdit;
//  m_edit->show();
	QVERIFY(m_edit != 0);
}

void TestXinxCodeEdit::testGetSetPlainText()
{
	QString textDeTest = "Hello, ceci est un texte. Trop cool :)";
	m_edit->setPlainText(textDeTest);
	QCOMPARE(m_edit->toPlainText().trimmed(), textDeTest);
}

void TestXinxCodeEdit::testFindTextNormal_data()
{
	QTest::addColumn<QString>("text");
	QTest::addColumn<QString>("searchStr");

	QTest::newRow("test 1") << "aaa abc abbb aaa acc add abc\n avc acc arc aaa aca aaa abb bbb ccc\n aaa abc abac aaax aee" << "aaa";
	QTest::newRow("test 2") << "aaa3 123 433 fff ppp cdc ZRZ AAA aaa RERE AAZ R aAa AZE EZA aaa" << "aaa";
	QTest::newRow("phrase") << "Ceci est un test, ce test est doit me compter 3 mots test" << "test";
	QTest::newRow("pas de mot") << "Dans ce texte le mot n'existe pas" << "test";
	QTest::newRow("espcace") << "Il etait une fois dans un petit chateau ..." << " ";
}

void TestXinxCodeEdit::testFindTextNormal()
{
	QFETCH(QString, text);
	QFETCH(QString, searchStr);

	int count = 0;
	m_edit->setPlainText(text);
	m_edit->moveCursor(QDocumentCursor::End, QDocumentCursor::MoveAnchor);

	QDocumentCursor cursor = m_edit->find(searchStr, m_edit->textCursor(), XinxCodeEdit::FindBackward);
	while (!cursor.isNull())
	{
		count++;

		cursor = m_edit->find(searchStr, cursor, XinxCodeEdit::FindBackward);
		if (count >= 999)
		{
			QWARN(qPrintable(QString("Position of cursor is %1").arg(cursor.position())));
			QFAIL("Infinit boucle");
		}
	}


	QCOMPARE(count, text.count(searchStr, Qt::CaseInsensitive));
}

void TestXinxCodeEdit::testFindTextRegExp_data()
{
	QTest::addColumn<QString>("text");
	QTest::addColumn<QRegExp>("searchStr");

	QTest::newRow("phrase") << "Ceci est un test, ce test est doit me compter 3 mots test" << QRegExp("[^A-Za-z0-9_:\\-\\.]");
	QTest::newRow("test 2") << "aaa3 123 433 fff ppp cdc ZRZ AAA aaa RERE AAZ R aAa AZE EZA aaa" << QRegExp("aaa");
	QTest::newRow("test 1") << "aaa abc abbb aaa acc add abc\n avc acc arc aaa aca aaa abb bbb ccc\n aaa abc abac aaax aee" << QRegExp("aaa");
	QTest::newRow("pas de mot") << "Dans ce texte le mot n'existe pas" << QRegExp("[^A-Za-z0-9_:\\-]");
	QTest::newRow("espcace") << "Il etait une fois dans un petit chateau ..." << QRegExp("[^A-Za-z0-9_:\\-\\.]");
}

void TestXinxCodeEdit::testFindTextRegExp()
{
	QFETCH(QString, text);
	QFETCH(QRegExp, searchStr);

	int count = 0;
	m_edit->setPlainText(text);
	m_edit->moveCursor(QDocumentCursor::End, QDocumentCursor::MoveAnchor);

	QDocumentCursor cursor = m_edit->find(searchStr, m_edit->textCursor(), XinxCodeEdit::FindBackward);
	while (!cursor.isNull())
	{
		count++;

		cursor = m_edit->find(searchStr, cursor, XinxCodeEdit::FindBackward);
		if (count >= 999)
		{
			QWARN(qPrintable(QString("Position of cursor is %1").arg(cursor.position())));
			QFAIL("Infinit boucle");
		}
	}


	QCOMPARE(count, text.count(searchStr));
}

void TestXinxCodeEdit::testTextUnderCursor_data()
{
	QTest::addColumn<QStringList>("text");
	QTest::addColumn<int>("nextword");
	QTest::addColumn<int>("right");
	QTest::addColumn<QString>("result");

	QStringList list = QStringList() << "aaa" << "abc" << "abbb" << "aaa" << "acc" << "add" << "abc\n" << "avc" << "acc" << "arc" << "aaa" << "aca" << "aaa" << "abb" << "bbb" << "ccc\n" << "aaa" << "abc" << "abac" << "aaaa" << "aee";
	QTest::newRow("cas 1") << list << 2 << 1 << "abbb";
	QTest::newRow("cas 2") << list << 3 << 2 << "aaa";
	QTest::newRow("cas 3") << list << 6 << 0 << "abc";
	QTest::newRow("cas 4") << list << 9 << 1 << "acc"; // Attention la fin de ligne est considere comme un mot
	list.clear();
	list << "aaa" << "bbb" << "ccc" << "ddd" << "eee";
	QTest::newRow("cas 5") << list << 0 << 9 << "ccc";
	list.clear();
	list << "aaa" << "<bbb" << "ccc" << "ddd" << "eee";
	QTest::newRow("cas 6") << list << 1 << 2 << "bbb";
	QTest::newRow("cas 7") << list << 1 << 3 << "bbb";
}

void TestXinxCodeEdit::testTextUnderCursor()
{
	QFETCH(QStringList, text);
	QFETCH(int, nextword);
	QFETCH(int, right);
	QFETCH(QString, result);

	QString textDeTest = text.join(" ");
	m_edit->setPlainText(textDeTest);

	for (int i = 0 ; i < nextword; i++)
	{
		//      QWARN( qPrintable( m_edit->textUnderCursor( m_edit->textCursor() ) ) );
		m_edit->moveCursor(QDocumentCursor::NextWord);
	}

	for (int i = 0 ; i < right; i++)
		m_edit->moveCursor(QDocumentCursor::Right);

	QCOMPARE(m_edit->textUnderCursor(m_edit->textCursor()), result);
}

void TestXinxCodeEdit::testTabStop_data()
{
	QTest::addColumn<int>("tabulation");

	QTest::newRow("tabulation de 1") << 1;
	QTest::newRow("tabulation de 2") << 2;
	QTest::newRow("tabulation de 4") << 4;
	QTest::newRow("tabulation de 8") << 8;
}

void TestXinxCodeEdit::testTabStop()
{
	QFETCH(int, tabulation);

	m_edit->setTabStopWidth(tabulation);
	QCOMPARE(m_edit->tabStopWidth(), tabulation);
}

void TestXinxCodeEdit::testUpperCase()
{
	QString textOriginal    = "Il etait une fois, dans un petit pays, un editeur surper puissant pour editer un fichier texte.";
	QString textDestination = "Il etait une fois, danS UN PETIT PAYs, un editeur surper puissant pour editer un fichier texte.";

	m_edit->setPlainText(textOriginal);
	QDocumentCursor cursor(m_edit->textCursor());
	cursor.moveTo(0, 22);
	cursor.movePosition(14, QDocumentCursor::NextCharacter, QDocumentCursor::KeepAnchor);
	m_edit->setTextCursor(cursor);

	m_edit->upperSelectedText();

	QCOMPARE(m_edit->toPlainText().trimmed(), textDestination);
}

void TestXinxCodeEdit::testLowerCase()
{
	QString textOriginal    = "Il etait une fois, danS UN PETIT PAYs, un editeur surper puissant pour editer un fichier texte.";
	QString textDestination = "Il etait une fois, dans un petit pays, un editeur surper puissant pour editer un fichier texte.";

	m_edit->setPlainText(textOriginal);
	QDocumentCursor cursor(m_edit->textCursor());
	cursor.moveTo(0, 22);
	cursor.movePosition(14, QDocumentCursor::NextCharacter, QDocumentCursor::KeepAnchor);
	m_edit->setTextCursor(cursor);

	m_edit->lowerSelectedText();

	QCOMPARE(m_edit->toPlainText().trimmed(), textDestination);
}

void TestXinxCodeEdit::testDuplicateLines_data()
{
	QTest::addColumn<QString>("text");
	QTest::addColumn<int>("selectionStart");
	QTest::addColumn<int>("selectionLength");
	QTest::addColumn<QString>("result");

	QTest::newRow("Duplicate a line in the middle") << "aaa\nbbb\nccc\nddd\neee"<< 4 << 0 << "aaa\nbbb\nbbb\nccc\nddd\neee";
	QTest::newRow("Duplicate a selected part of line") << "aaa\nbxbb\nccc\nddd\neee"<< 5 << 1 << "aaa\nbxxbb\nccc\nddd\neee";
}

void TestXinxCodeEdit::testDuplicateLines()
{
	QFETCH(QString, text);
	QFETCH(int, selectionStart);
	QFETCH(int, selectionLength);
	QFETCH(QString, result);

	m_edit->setPlainText(text);
	QDocumentCursor cursor(m_edit->textCursor());
	cursor.movePosition(selectionStart, QDocumentCursor::NextCharacter);
	for (int i = 0 ; i < selectionLength ; i++)
	{
		cursor.movePosition(selectionLength, QDocumentCursor::NextCharacter, QDocumentCursor::KeepAnchor);
	}
	m_edit->setTextCursor(cursor);

	m_edit->duplicateLines();

	QCOMPARE(m_edit->toPlainText().trimmed(), result);
}

void TestXinxCodeEdit::testMoveLineUp_data()
{
	QTest::addColumn<QString>("text");
	QTest::addColumn<int>("selectionStart");
	QTest::addColumn<int>("selectionLength");
	QTest::addColumn<QString>("result");

	QTest::newRow("Move the third line") << "aaa\nbbb\nccc\nddd\neee"<< 9 << 0 << "aaa\nccc\nbbb\nddd\neee";
	QTest::newRow("Move the second line") << "aaa\nbbb\nccc\nddd\neee"<< 5 << 0 << "bbb\naaa\nccc\nddd\neee";
	QTest::newRow("Move the first line") << "aaa\nbbb\nccc\nddd\neee"<< 2 << 0 << "aaa\nbbb\nccc\nddd\neee";
	QTest::newRow("Move selected lines") << "aaa\nbbb\nccc\nddd\neee"<< 9 << 4 << "aaa\nccc\nddd\nbbb\neee";
}

void TestXinxCodeEdit::testMoveLineUp()
{
	QFETCH(QString, text);
	QFETCH(int, selectionStart);
	QFETCH(int, selectionLength);
	QFETCH(QString, result);

	m_edit->setPlainText(text);
	QDocumentCursor cursor(m_edit->textCursor());
	cursor.movePosition(selectionStart, QDocumentCursor::NextCharacter);
	cursor.movePosition(selectionLength, QDocumentCursor::NextCharacter, QDocumentCursor::KeepAnchor);
	m_edit->setTextCursor(cursor);

	m_edit->moveLineUp();

	QCOMPARE(m_edit->toPlainText().trimmed(), result);
}

void TestXinxCodeEdit::testMoveLineDown_data()
{
	QTest::addColumn<QString>("text");
	QTest::addColumn<int>("selectionStart");
	QTest::addColumn<int>("selectionLength");
	QTest::addColumn<QString>("result");

	QTest::newRow("Move the 3rd line") << "aaa\nbbb\nccc\nddd\neee"<< 9 << 0 << "aaa\nbbb\nddd\nccc\neee";
	QTest::newRow("Move the 4th line") << "aaa\nbbb\nccc\nddd\neee"<< 13 << 0 << "aaa\nbbb\nccc\neee\nddd";
	QTest::newRow("Move the 5th line") << "aaa\nbbb\nccc\nddd\neee"<< 17 << 0 << "aaa\nbbb\nccc\nddd\neee";
	QTest::newRow("Move selected lines") << "aaa\nbbb\nccc\nddd\neee"<< 9 << 4 << "aaa\nbbb\neee\nccc\nddd";
}

void TestXinxCodeEdit::testMoveLineDown()
{
	QFETCH(QString, text);
	QFETCH(int, selectionStart);
	QFETCH(int, selectionLength);
	QFETCH(QString, result);

	m_edit->setPlainText(text);
	QDocumentCursor cursor(m_edit->textCursor());
	cursor.movePosition(selectionStart, QDocumentCursor::NextCharacter);
	cursor.movePosition(selectionLength, QDocumentCursor::NextCharacter, QDocumentCursor::KeepAnchor);
	m_edit->setTextCursor(cursor);

	m_edit->moveLineDown();

	QCOMPARE(m_edit->toPlainText().trimmed(), result);
}

void TestXinxCodeEdit::testBookmark()
{
	m_edit->setPlainText("aaa\nbbb\nccc\nddd\neee\nfff\nggg\nhhh\niii\njjj\nkkk\nlll\nmmm\nnnn\nooo\nppp");

	m_edit->setBookmark(2, true);
	m_edit->setBookmark(6, true);
	m_edit->setBookmark(6, false);
	m_edit->setBookmark(8, true);
	m_edit->setBookmark(11, true);
	m_edit->setBookmark(13, true);

	QCOMPARE(m_edit->listOfBookmark(), QList<int>() << 2 << 8 << 11 << 13);
}

void TestXinxCodeEdit::testError()
{
	m_edit->setPlainText("aaa\nbbb\nccc\nddd\neee\nfff\nggg\nhhh\niii\njjj\nkkk\nlll\nmmm\nnnn\nooo\nppp");

	m_edit->setErrors(QList<int>() << 5 << 12 << 44);
}

void TestXinxCodeEdit::cleanupTestCase()
{
	delete m_edit;
}

QTEST_MAIN(TestXinxCodeEdit)
#include "testxinxcodeedit.moc"
