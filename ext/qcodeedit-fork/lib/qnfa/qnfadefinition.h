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

#ifndef _QNFA_DEFINITION_H_
#define _QNFA_DEFINITION_H_

/*!
	\file qnfadefinition.h
	
	\brief Definition of the QNFADefinition class.
*/

#include "qlanguagedefinition.h"

#include <QHash>
#include <QStringList>

struct QNFA;

class QNFAMarker;
class QNFAMatcher;
class QNFAIndenter;
class QNFAHighlighter;

class QNFAFactory;

class QNFAAction
{
	public:
		enum
		{
			NoAction	= 0,
			
			FormatMask	= 0x000000ff,
			ParenMask	= 0x0000ff00,
			
			Highlight	= 0x01000000,
			Indent		= 0x02000000,
			ParenOpen	= 0x04000000,
			ParenClose	= 0x08000000,
			MatchParen	= 0x10000000,
			Fold		= 0x20000000,
			
			Content		= 0x80000000
		};
		
		inline static int format(int id)
		{ return id & FormatMask; }
		
		inline static int parenthesis(int id)
		{ return id & ParenMask; }
};

class QCE_EXPORT QNFADefinition : public QLanguageDefinition
{
	friend class QNFAMarker;
	friend class QNFAMatcher;
	friend class QNFAIndenter;
	friend class QNFAHighlighter;
	
	public:
		QNFADefinition(const QString& fn, QLanguageFactory *f);
		virtual ~QNFADefinition();
		
		virtual const QString& language() const;
		virtual const QStringList& extensions() const;
		
		virtual QMarkerInterface* marker();
		virtual QMatcherInterface* matcher();
		virtual QIndenterInterface* indenter();
		virtual QHighlighterInterface* highlighter();
		
		static void addContext(const QString& id, QNFA *nfa);
		
		static void addEmbedRequest(const QString& lang, QNFA *dest);
		
		static void shareEmbedRequests(QNFA *src, QNFA *dest);
		
	private:
		bool m_indentFold;
		QString m_language,
				m_defaultMark,
				m_singleLineComment;
		
		QStringList m_extensions;
		
		QNFA *m_root;
		
		QNFAMarker *m_marker;
		QNFAMatcher *m_matcher;
		QNFAIndenter *m_indenter;
		QNFAHighlighter *m_highlighter;
		
		static QHash<QString, int> m_paren;
		static QHash<QString, QNFA*> m_contexts;
		
		static void flushEmbedRequests(const QString& lang);
		
		struct EmbedRequest
		{
			inline EmbedRequest(QNFA *nfa, int idx) : index(idx), target(nfa) {}
			
			int index;
			QNFA *target;
		};
		
		typedef QList<EmbedRequest> EmbedRequestList;
		
		static QHash<QString, EmbedRequestList> m_pendingEmbeds;
};

#endif // !_QNFA_DEFINITION_H_
