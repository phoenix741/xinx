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

#include "qlanguagefactory.h"

/*!
	\file qlanguagefactory.cpp
	\brief Implementation of QLanguageFactory
	
	\see QLanguageFactory
*/

/*!
	\ingroup language
	@{
	
	\class QLanguageFactory
	\brief The "dispatcher" class.
	
	It stores language definitions, added programmatically or found in XML files,
	in specified locations and only if generic components are built-in. From 
	these definitions, QLanguageFactory generates matchers, indenters and
	highlighters for a text editor, according to a file name.
	
	\see QLanguageDefinition
	\see QMatcher
	\see QIndenter
	\see QHighlighter
*/

#include <QDir>
#include <QFileInfo>
#include <QStringList>

#ifdef _QCODE_EDIT_DEBUG_
#include <QtDebug>
#endif

#include "qlanguagedefinition.h"

#include "qhighlighter.h"
#include "qhighlighterinterface.h"

#include "qindenter.h"
#include "qindenterinterface.h"

#include "qmatcher.h"
#include "qmatcherinterface.h"

#include "qmarker.h"
#include "qmarkerinterface.h"

#include "qcodecompletionengine.h"

#include "qeditor.h"

#ifdef QXS_BUILD
#include "qxsdefinition.h"
#endif

#ifdef QNFA_BUILD
#include "qnfadefinition.h"
#endif

/*!
	\brief Empty constructor
*/
QLanguageFactory::QLanguageFactory(QFormatFactory *fmt, QObject *p)
 : QObject(p), pFormatFactory(fmt)
{
	
}

/*!
	\brief Empty destructor
*/
QLanguageFactory::~QLanguageFactory()
{
	
}

/*!
	\return a list of languages supported by this factory
	
	A supported language is a language that is claimed to be supported by one
	language definition at least...
*/
QStringList QLanguageFactory::languages() const
{
	QStringList l;
	
	foreach ( QLanguageDefinition *d, definitions )
		l << d->language();
	
	return l;
}

/*!
	\return a list of file filters supported by this factory
	
	\note This list is NEVER empty and the last item is always "All files (*)"
*/
QStringList QLanguageFactory::fileFilters() const
{
	QStringList l;
	
	foreach ( QLanguageDefinition *d, definitions )
		l << tr("%1 files (*.%2)").arg(d->language()).arg(d->extensions().join(" *."));
	
	l << tr("All files (*)");
	
	return l;
}

/*!
	\overload
	
	\param e target editor
	\param file filename displayed by the editor
	
	\note The filename is of no real importance, the only thing that matters is
	its suffix ( \see QFileInfo::completeSuffix() ) which must match one of those
	handled by the registered language definitions.
	
	\see setLanguage(QEditor*, QLanguageDefinition*, QCodeCompletionEngine*)
*/
void QLanguageFactory::setLanguage(QEditor *e, const QString& file)
{
	QFileInfo inf(file);
	const QString ext = inf.suffix(),
				cext = inf.completeSuffix();
	
	if ( cext.isEmpty() )
	{
		return;
	}
	
	QLanguageDefinition *d = definition(cext);
	
	if ( !d && (ext != cext) )
		d = definition(ext);
	
	if ( d == e->languageDefinition() )
		return;
	
	QCodeCompletionEngine *en = completionEngine(ext);
	
	if ( !en && (ext != cext) )
		en = completionEngine(ext);
	
	setLanguage(e, d, en);
}

/*!
	\brief Sets matcher, indenter and highlighter to a text editor
	
	\param e target editor
	\param d language definition to use
	
	\see setLanguage(QEditor*, const QString&)
*/
void QLanguageFactory::setLanguage( QEditor *e,
									QLanguageDefinition *d,
									QCodeCompletionEngine *en)
{
	QMarker *k = e->marker();
	QMatcher *m = e->matcher();
	QIndenter *i = e->indenter();
	QHighlighter *h = e->highlighter();
	
	if ( !d )
	{
		//qDebug("No syntax...");
		
		if ( k )
			k->pInterface = 0;
		
		if ( m )
			m->pInterface = 0;
		
		if ( i )
			i->pInterface = 0;
		
		if ( h )
			h->pInterface = 0;
		
		e->setCompletionEngine(0);
		return;
	}
	
	if ( !k )
	{
		k = new QMarker(d->marker(), d);
		e->setMarker(k);
	} else {
		k->pInterface = d->marker();
	}
	
	k->pFormatFactory = pFormatFactory;
	
	if ( !m )
	{
		m = new QMatcher(d->matcher(), d);
		e->setMatcher(m);
	} else {
		m->pInterface = d->matcher();
	}
	
	m->pFormatFactory = pFormatFactory;
	
	if ( !i )
	{
		i = new QIndenter(d->indenter(), d);
		e->setIndenter(i);
	} else {
		i->pInterface = d->indenter();
	}
	
	//i->pFormatFactory = pFormatFactory;
	
	if ( !h )
	{
		h = new QHighlighter(d->highlighter(), d);
		e->setHighlighter(h);
	} else {
		h->pInterface = d->highlighter();
	}
	
	h->pFormatFactory = pFormatFactory;
	
	e->setLanguageDefinition(d);
	
	e->setCompletionEngine(en ? en->clone() : 0);
}

/*!
	\brief Adds a language definition to the factory
	
	\param d language definition to add
	
	This function is mainly targeted for plugin-based application that allows
	plugins to bring new definitions, especially hard-coded ones. 
*/
void QLanguageFactory::addLanguageDefinition(QLanguageDefinition *d)
{
	definitions << d;
}

/*!
	\brief Adds a completion engine
*/
void QLanguageFactory::addCompletionEngine(QCodeCompletionEngine *e)
{
	m_engines << e;
}

/*!
	\return the language definition corresponding to the extension \a ext
*/
QLanguageDefinition* QLanguageFactory::definition(const QString& ext)
{
	#ifdef _QCODE_EDIT_DEBUG_
	qDebug("Getting language : %s", qPrintable(ext));
	#endif
	
	foreach ( QLanguageDefinition *d, definitions )
	{
		if ( d && d->extensions().contains(ext) )
		{
			//qDebug("found %s", qPrintable(d->language()));
			return d;
		}
	}
	
	#ifdef _QCODE_EDIT_DEBUG_
	qDebug("not found...");
	#endif
	
	return 0;
}

/*!
	\return the language definition corresponding to the extension \a ext
*/
QCodeCompletionEngine* QLanguageFactory::completionEngine(const QString& ext)
{
	#ifdef _QCODE_EDIT_DEBUG_
	qDebug("Getting completion engine : %s", qPrintable(ext));
	#endif
	
	foreach ( QCodeCompletionEngine *e, m_engines )
		if ( e && e->extensions().contains(ext) )
			return e;
	
	#ifdef _QCODE_EDIT_DEBUG_
	qDebug("not found...");
	#endif
	
	return 0;
}

/*!

*/
void QLanguageFactory::addDefinitionPath(const QString& path)
{
	QDir d(path);
	
	foreach ( QString f, d.entryList(QDir::Files | QDir::Readable) )
	{
		#ifdef QXS_BUILD
		if ( f.endsWith(".qxs") )
			addLanguageDefinition(new QXSDefinition(d.filePath(f), this));
		#endif
		
		#ifdef QNFA_BUILD
		if ( f.endsWith(".qnfa") )
			addLanguageDefinition(new QNFADefinition(d.filePath(f), this));
		#endif
	}
}

/*! @} */
