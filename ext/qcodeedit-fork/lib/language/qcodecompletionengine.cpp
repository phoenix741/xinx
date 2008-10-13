/****************************************************************************
**
** Copyright (C) 2006-2008 fullmetalcoder <fullmetalcoder@hotmail.fr>
**
** This file is part of the Edyuk project <http://edyuk.org>
** 
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qcodecompletionengine.h"

/*!
	\file qcompletionengine.cpp
	\brief Implementation of the QCodeCompletionEngine class.
*/

#include "qeditor.h"

#include <QAction>
#include <QKeyEvent>
#include <QTextCursor>

#ifdef _QCODE_MODEL_
#include "qcodebuffer.h"
#endif

/*!

*/
QCodeCompletionEngine::QCodeCompletionEngine(QObject *p)
 : QObject(p), m_max(0)
{
	pForcedTrigger = new QAction(tr("&Trigger completion"), this);
	
	connect(pForcedTrigger	, SIGNAL( triggered() ),
			this			, SLOT  ( complete() ) );
	
}

/*!

*/
QCodeCompletionEngine::~QCodeCompletionEngine()
{
	
}

/*!
	\return 
*/
QAction* QCodeCompletionEngine::triggerAction() const
{
	return pForcedTrigger;
}

/*!

*/
void QCodeCompletionEngine::retranslate()
{
	pForcedTrigger->setText(tr("&Trigger completion"));
}

/*!

*/
QStringList QCodeCompletionEngine::triggers() const
{
	return m_triggers;
}

/*!

*/
void QCodeCompletionEngine::addTrigger(const QString& s)
{
	if ( m_triggers.contains(s) )
		return;
	
	if ( s.count() > m_max )
		m_max = s.count();
	
	m_triggers << s;
}

/*!

*/
void QCodeCompletionEngine::removeTrigger(const QString& s)
{
	m_triggers.removeAll(s);
}

/*!

*/
void QCodeCompletionEngine::setCodeModel(QCodeModel *m)
{
	Q_UNUSED(m)
}

/*!

*/
QEditor* QCodeCompletionEngine::editor() const
{
	return pEdit;
}

/*!
	\brief Attach the completion engine instance to a new editor object
*/
void QCodeCompletionEngine::setEditor(QEditor *e)
{
	if ( pEdit )
	{
		pEdit->removeAction(pForcedTrigger, "&Edit");
		pEdit->removeEventFilter(this);
	}
	
	pEdit = e;
	
	if ( pEdit )
	{
		pEdit->installEventFilter(this);
		pEdit->addAction(pForcedTrigger, "&Edit");
		//e->addAction(pForcedTrigger);
	}
}

/*!
	\internal
*/
void QCodeCompletionEngine::run()
{
	if ( m_cur.isNull() )
		return;
	
	//qDebug("complete!");
	
	complete(m_cur, m_trig);
	
	m_cur = QDocumentCursor();
	m_trig.clear();
}

void QCodeCompletionEngine::complete()
{
	complete(editor()->cursor(), QString());
}

/*!
	\internal
*/
bool QCodeCompletionEngine::eventFilter(QObject *o, QEvent *e)
{
	if ( !e || !o || (e->type() != QEvent::KeyPress) || (o != pEdit) )
		return false;
	
	//qDebug("should trigger completion?");
	
	QDocumentCursor cur = editor()->cursor();
	QKeyEvent *k = static_cast<QKeyEvent*>(e);
	
	QString s, txt = s = k->text();
	
	int count = txt.count();
	
	if ( txt.isEmpty() || m_triggers.isEmpty() )
		return false; // QThread::eventFilter(o, e);
	
	//qDebug("should trigger completion? (bis)");
	
	if ( count > m_max )
	{
		txt = txt.right(m_max);
		
	} else if ( count < m_max ) {
		
		QDocumentCursor c(cur);
		c.movePosition(m_max - count, QDocumentCursor::Left, QDocumentCursor::KeepAnchor);
		
		//qDebug("prev text : %s", qPrintable(c.selectedText()));
		
		txt.prepend(c.selectedText());
	}
	
	//qDebug("text : %s", qPrintable(txt));
	
	foreach ( QString trig, m_triggers )
	{
		if ( txt.endsWith(trig) )
		{
			editor()->write(s);
			
			cur = editor()->cursor();
			cur.movePosition(trig.count(), QDocumentCursor::PreviousCharacter);
			
			// notify completion trigger
			emit completionTriggered(trig);
			
			//get rid of previous calltips/completions
			editor()->setFocus();
			
			// trigger completion
			complete(cur, trig);
			
			return true;
		}
	}
	
	return false;
}

/*!
	\brief Completion callback
*/
void QCodeCompletionEngine::complete(const QDocumentCursor& c, const QString& trigger)
{
	#ifdef _QCODE_MODEL_
	QDocumentCursor cc = c;
	cc.movePosition(1, QDocumentCursor::Start, QDocumentCursor::KeepAnchor);
	
	//qDebug("%s", qPrintable(cc.selectedText()));
	
	QCodeBuffer *buffer = new QCodeBuffer(cc.selectedText());
	
	complete(buffer, trigger);
	#else
	qWarning("From complete(QDocumentCursor, QString)");
	qWarning("QCodeCompletionEngine is not self-sufficient : subclasses should "
			"reimplement at least on of the complete() method...");
	#endif
}

/*!
	\overload
	\brief Overloaded completion callback
*/
void QCodeCompletionEngine::complete(QCodeStream *s, const QString& trigger)
{
	qWarning("From complete(QCodeStream*, QString)");
	qWarning("QCodeCompletionEngine is not self-sufficient : subclasses should"
			"reimplement at least on of the complete() method...");
}
