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
#include "xquerydialogimpl.h"
#include "xquerydialogimpl_p.h"
#include <plugins/xinxpluginsloader.h>

#include "editors/models/xq/xqmodelcompleter.h"

// QCodeEdit header
#include "qdocumentcursor.h"

// Qt header
#include <QStringList>
#include <QPushButton>
#include <QCompleter>
#include <QtXmlPatterns>

/* XQueryDialogMessageHandler */

XQueryDialogMessageHandler::XQueryDialogMessageHandler(QObject * parent) : QAbstractMessageHandler(parent)
{
}

XQueryDialogMessageHandler::~XQueryDialogMessageHandler() {
}

const QStringList & XQueryDialogMessageHandler::messages() const
{
	return m_messages;
}

void XQueryDialogMessageHandler::handleMessage(QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation)
{
	const bool hasLine = sourceLocation.line() != -1;

	switch (type)
	{
	case QtWarningMsg:
		if (hasLine)
		{
			m_messages += tr("Warning in %1, at line %2, column %3: %4").arg(QString::fromLatin1(sourceLocation.uri().toEncoded()),
							QString::number(sourceLocation.line()),
							QString::number(sourceLocation.column()),
							description);
		}
		else
		{
			m_messages += tr("Warning in %1: %2").arg(QString::fromLatin1(sourceLocation.uri().toEncoded()),
							description);
		}

		break;
	case QtFatalMsg:
	{
		Q_ASSERT(!sourceLocation.isNull());
		const QString errorCode(identifier.fragment());
		Q_ASSERT(!errorCode.isEmpty());
		QUrl uri(identifier);
		uri.setFragment(QString());

		QString errorId;
		if (uri.toString() == QLatin1String("http://www.w3.org/2005/xqt-errors"))
			errorId = errorCode;
		else
			errorId = QString::fromLatin1(identifier.toEncoded());

		if (hasLine)
		{
			m_messages += tr("Error %1 in %2, at line %3, column %4: %5").arg(errorId,
							QString::fromLatin1(sourceLocation.uri().toEncoded()),
							QString::number(sourceLocation.line()),
							QString::number(sourceLocation.column()),
							description);
		}
		else
		{
			m_messages += tr("Error %1 in %2: %3").arg(errorId,
							QString::fromLatin1(sourceLocation.uri().toEncoded()),
							description);
		}
		break;
	}
	case QtCriticalMsg:
	case QtDebugMsg:
		Q_ASSERT_X(false, Q_FUNC_INFO, "message() is not supposed to receive QtCriticalMsg or QtDebugMsg.");
		return;
	}
}

/* XQueryDialogImpl */

XQueryDialogImpl::XQueryDialogImpl(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);

	m_queryTextEdit->editor()->setFrameStyle(QFrame::StyledPanel);
	m_queryTextEdit->editor()->setFrameShadow(QFrame::Sunken);
	m_queryTextEdit->editor()->setLineWrapping(true);

	m_queryTextEdit->setHighlighter("XQuery");
	m_resultTextEdit->setHighlighter("XML");

	XQModelCompleter * completionModel = new XQModelCompleter(m_queryTextEdit);
	QCompleter * completer = new QCompleter(m_queryTextEdit);
	completer->setModel(completionModel);
	//FIXME: Ne fonctionne plus. Doit utiliser le nouveau système ! Obligatoire
//	m_queryTextEdit->setCompleter(completer);

	QPushButton * evaluateBtn = new QPushButton(tr("&Evaluate"));
	evaluateBtn->setDefault(true);
	m_buttonBox->addButton(evaluateBtn, QDialogButtonBox::ActionRole);
	connect(evaluateBtn, SIGNAL(clicked()), this, SLOT(evaluate()));
}

XQueryDialogImpl::~XQueryDialogImpl()
{

}

void XQueryDialogImpl::setFileName(const QString & filename)
{
	if (m_fileName != filename)
		m_fileName = filename;
}

void XQueryDialogImpl::setCurrentXPath(const QString & currentXPath)
{
	m_xpathTextEdit->setText(currentXPath);
}

void XQueryDialogImpl::evaluate()
{
	// Open the file to read it
	QFile sourceDocument;
	sourceDocument.setFileName(m_fileName);
	if (! sourceDocument.open(QIODevice::ReadOnly))
	{
		qWarning(qPrintable(tr("Error while opening presentation file : %1").arg(sourceDocument.errorString())));
		return;
	}

	// Get the XPath
	QString xpath = m_queryTextEdit->toPlainText();
	if (xpath.isEmpty()) return;
	QString xquery;
	xquery += "<result>\n";
	xquery += "{\n";
	xquery += "doc($d)" + m_xpathTextEdit->text() + "/\n";
	xquery += "(" + xpath + ")\n";
	xquery += "}\n";
	xquery += "</result>\n";

	// Prepare the query
	QBuffer result;
	result.open(QIODevice::ReadWrite);
	XQueryDialogMessageHandler handler;
	QXmlQuery query;
	query.setMessageHandler(&handler);
	QXmlFormatter serializer(query, &result);

	// Execute the query
	query.bindVariable("d", &sourceDocument);
	query.setQuery(xquery);

	QTime bench;
	bench.start();

	query.evaluateTo(&serializer);

	m_benchLabel->setText(tr("Resolved in %1 ms").arg(bench.elapsed()));

	QString r = result.data() + "\n" + handler.messages().join("\n");
	m_resultTextEdit->setPlainText(r);
}

void XQueryDialogImpl::addFunction()
{
	QPushButton * btn = qobject_cast<QPushButton*>(sender());
	if (btn)
	{
		QString functionName = btn->text();
		functionName.chop(1);

		if (m_queryTextEdit->textCursor().hasSelection())
		{
			QDocumentCursor c = m_queryTextEdit->textCursor(),
								s = c.selectionStart(),
									e = c.selectionEnd();
			e.movePosition(functionName.length(), QDocumentCursor::Right);

			s.insertText(functionName);
			e.insertText(")");
		}
		else
		{
			QDocumentCursor c = QDocumentCursor(m_queryTextEdit->document());
			c.movePosition(1, QDocumentCursor::Start);
			c.insertText(functionName);
			c.movePosition(1, QDocumentCursor::End);
			c.insertText(")");
		}
	}
}
