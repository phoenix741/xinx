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

#include "qmarker.h"
#include "qmarkerinterface.h"

/*!
	\file qmarker.cpp
	\brief Implementation of QMarker
	
	\see QMarker
*/

/*!
	\ingroup language
	@{
	
	\class QMarker
	\brief Wrapper class that "connect" a text editor to a QMarkerInterface.
	
	\see QMarkerInterface
*/

#include "qeditor.h"
//#include "qblockdata.h"

#include "qdocument.h"
#include "qdocumentline.h"
#include "qdocumentcursor.h"

#include "qlanguagedefinition.h"

#include "qlinemarksinfocenter.h"

#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QScrollBar>
#include <QActionGroup>

class QMarkerCommand
{
	public:
		enum Persistency
		{
			Move,		// disappear as soon as cursor moves
			OutOfLine,	// disappear when cursor goes out of line
			Manual		// disappear through menu only
		};
		
		QMarkerCommand(QDocumentLine l, int id, int p)
		 : m_id(id), m_persistency(p), m_line(l) {}
		
		inline int id() const { return m_id; }
		inline QDocumentLine& line() { return m_line; }
		inline int persistency() const { return m_persistency; }
		
	private:
		int m_id;
		int m_persistency;
		QDocumentLine m_line;
};

/*!
	\brief Constructor
	
	\param i interface used when matching triggered
	\param d language definition used to create the interface
*/
QMarker::QMarker(QMarkerInterface *i, QLanguageDefinition *d)
 : QObject(d), pInterface(i), pDefinition(d)
{
	
}

/*!
	\brief Destructor
*/
QMarker::~QMarker()
{
	
}

/*!
	\return the text editor connected to this matcher
*/
QEditor* QMarker::editor() const
{
	return pEdit;
}

/*!
	\brief Attach the marker to a text editor
	
	\param e target editor
	
	\note Previously attached editor is detached.
*/
void QMarker::setEditor(QEditor *e)
{
	if ( pEdit )
	{
		disconnect(	pEdit	, SIGNAL( cursorPositionChanged() ),
					this	, SLOT  ( cursorPositionChanged() ) );
		
	}
	
	pEdit = e;
	
	if ( pEdit )
	{
		connect(pEdit	, SIGNAL( cursorPositionChanged() ),
				this	, SLOT  ( cursorPositionChanged() ) );
		
	}
}

/*!
	\return the document used by this marker through editor() or
	editorinterface()
*/
QDocument* QMarker::document() const
{
	if ( editor() )
		return editor()->document();
	
	return 0;
}

/*!
	\return the language definition that created this marker
*/
QLanguageDefinition* QMarker::definition() const
{
	return pDefinition;
}

/*!

*/
QStringList QMarker::ids() const
{
	return pInterface ? pInterface->ids() : QStringList();
}

/*!

*/
QPixmap QMarker::icon(const QString& id) const
{
	return pInterface ? pInterface->icon(id) : QPixmap();
}

/*!

*/
QColor QMarker::color(const QString& id) const
{
	return pInterface ? pInterface->color(id) : QColor();
}

/*!

*/
bool QMarker::needFocus(const QString& id) const
{
	return pInterface ? pInterface->needFocus(id) : false;
}

/*!

*/
bool QMarker::isPrivate(const QString& id) const
{
	return pInterface ? pInterface->needFocus(id) : false;
}

/*!

*/
int QMarker::persistency(const QString& id) const
{
	return pInterface ? pInterface->persistency(id) : QMarkerCommand::Move;
}

/*!

*/
QString QMarker::priority(const QStringList& ids) const
{
	return pInterface ? pInterface->priority(ids) : QString();
}

/*!
	\internal
*/
void QMarker::menu(QDocumentLine& b, const QPoint& pos)
{
	if ( !b.isValid() || !pInterface )
		return;
	
	#if 0
	QBlockData *bd = QBlockData::data(b);
	
	if ( !bd )
	{
		bd = new QBlockData;
		bd->setToBlock(b);
	}
	
	QMenu *menu = new QMenu;
	QAction *aClear = new QAction(tr("&Clear"), menu);
	
	menu->addAction(aClear);
	
	bool bGroup = false;
	QList<QStringList> m = pInterface->items();
	
	foreach ( const QStringList& l, m )
	{
		if ( l.count() > 1 )
		{
			bGroup = true;
			menu->addSeparator();
			
			QActionGroup *g = new QActionGroup(menu);
			g->setExclusive(true);
			
			foreach ( const QString& s, l )
			{
				QAction *a = new QAction(s, menu);
				a->setCheckable(true);
				a->setChecked( bd->marks.contains(s) );
				a->setEnabled( !isPrivate(s) );
				
				g->addAction(a);
				menu->addAction(a);
			}
			
		} else if ( l.count() == 1 ) {
			
			if ( bGroup )
				menu->addSeparator();
			
			QAction *a = new QAction(l.at(0), menu);
			a->setCheckable(true);
			a->setChecked( bd->marks.contains(l.at(0)) );
			a->setEnabled( !isPrivate(l.at(0)) );
			
			menu->addAction(a);
		}
		
	}
	
	QAction *a = menu->exec(pos);
	
	if ( a )
	{
		if ( a == aClear )
		{
			bd->marks.clear();
			
			//document()->markContentsDirty(b.position(), b.length());
			
			QWidget *w = qobject_cast<QWidget*>(pEdit);
			
			if ( w )
			{
				w->clearFocus();
				w->setFocus();
			}
		} else {
			
			foreach ( QAction *a, menu->actions() )
				if ( a != aClear )
					toggleMark(b, a->text(), a->isChecked());
			
		}
	}
	
	delete menu;
	#else
	Q_UNUSED(pos);
	#endif
}

QString QMarker::defaultMark() const
{
	return pInterface ? pInterface->defaultMark() : QString();
}

bool QMarker::isDefaultMarkToggled(QDocumentLine& b) const
{
	return isMarkToggled(b, defaultMark());
}

void QMarker::toggleDefaultMark(QDocumentLine& b, int state)
{
	if ( state != -1 )
		toggleMark(b, defaultMark(), (bool)state);
	else
		toggleMark(b, defaultMark(), !isDefaultMarkToggled(b));
}

bool QMarker::isMarkToggled(const QDocumentLine& b, const QString& id) const
{
	return b.marks().contains(QLineMarksInfoCenter::instance()->markTypeId(id));
}

/*!
	\overload
	
*/
void QMarker::toggleMark(QDocumentLine& b, const QString& id, bool on)
{
	#if 0
	if ( !b.isValid() || !document() || !pInterface )
		return;
	
	QBlockData *bd = QBlockData::data(b);
	
	if ( !bd )
	{
		bd = new QBlockData;
		bd->setToBlock(b);
	}
	
	if ( !ids().contains(id) )
	{
		qWarning("Unsupported mark : %s", qPrintable(id));
		return;
	}
	
	QStringList exclusive;
	QList<QStringList> m = pInterface->items();
	
	foreach ( QStringList l, m )
	{
		if ( l.contains(id) )
		{
			exclusive = l;
			exclusive.removeAll(id);
			break;
		}
	}
	
	if ( on )
	{
		if ( !bd->marks.contains(id) )
		{
			if ( needFocus(id) )
			{
				QDocumentCursor c(b);
				editor()->setCursor(c);
			}
			
			bd->marks << id;
			
			foreach ( QString concurrent, exclusive )
			{
				emit marked(b, concurrent, false);
				bd->marks.removeAll(concurrent);
				
				foreach ( QMarkerCommand *c, marks )
				{
					if ( c->id() == concurrent && c->data() == bd )
						marks.removeAll(c);
				}
			}
			
			marks << new QMarkerCommand(id, bd, persistency(id));
		}
	} else {
		bd->marks.removeAll(id);
		
		foreach ( QMarkerCommand *c, marks )
		{
			if ( c->id() == id && c->data() == bd )
				marks.removeAll(c);
		}
		
		QString concurrent = priority(exclusive);
		
		if ( concurrent.count() )
		{
			emit marked(b, concurrent, true);
			bd->marks << concurrent;
			
			marks << new QMarkerCommand(concurrent, bd, persistency(concurrent));
		}
	}
	
	//document()->markContentsDirty(b.position(), b.length());
	
	QWidget *w = qobject_cast<QWidget*>(pEdit);
	
	if ( w )
	{
		w->clearFocus();
		w->setFocus();
	}
	
	emit marked(b, id, on);
	#endif
	
	const int mid = QLineMarksInfoCenter::instance()->markTypeId(id);
	
	toggleMark(b, mid, on);
}

void QMarker::toggleMark(QDocumentLine& b, int mid, bool on)
{
	QList<int> prevmarks = b.marks();
	
	if ( on )
	{
		b.addMark(mid);
		
		marks << new QMarkerCommand(b,
									mid,
									QLineMarksInfoCenter::instance()->markType(mid).persistency
									);
		
	} else {
		b.removeMark(mid);
		
		QList<QMarkerCommand*>::iterator it = marks.begin();
		
		while ( it != marks.end() )
		{
			QMarkerCommand *c = *it;
			
			if ( (c->line() == b) && (c->id() == mid) )
			{
				it = marks.erase(it);
				delete c;
			} else {
				++it;
			}
		}
	}
}

/*!
	\internal
*/
void QMarker::cursorPositionChanged()
{
	if ( !document() || !pInterface )
		return;
	
	QList<QMarkerCommand*>::iterator it = marks.begin();
	
	while ( it != marks.end() )
	{
		QMarkerCommand *c = *it;
		
		if ( c->persistency() == QMarkerCommand::Move )
		{
			//qDebug("toggling out ephemere mark...");
			
			it = marks.erase(it);
			
			toggleMark(c->line(), c->id(), false);
			
			delete c;
			continue;
		} else if ( c->persistency() == QMarkerCommand::OutOfLine ) {
			if ( c->line() != editor()->cursor().line() )
			{
				it = marks.erase(it);
				
				toggleMark(c->line(), c->id(), false);
				
				delete c;
				continue;
			}
		}
		
		++it;
	}
}


/*! @} */
