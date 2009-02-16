/****************************************************************************
**
** Copyright (C) 2006-2009 fullmetalcoder <fullmetalcoder@hotmail.fr>
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

#ifndef _SNIPPETS_H_
#define _SNIPPETS_H_

/*!
	\file snippets.h
	\brief
*/

#include <QStringList>

class QEditor;

class Snippet
{
	public:
		virtual ~Snippet() {}
		
		inline QString name() const { return m_name; }
		inline void setName(const QString& n) { m_name = n; }
		
		inline QStringList contexts() const { return m_contexts; }
		inline void setContexts(const QStringList& l) { m_contexts = l; }
		
		virtual void insert(QEditor *e) const = 0;
		
	private:
		QString m_name;
		QStringList m_contexts;
};

class SnippetManager
{
	public:
		enum SnippetType
		{
			Guess,
			Plain,
			Simple,
			Scripted
		};
		
		SnippetManager();
		virtual ~SnippetManager();
		
		int snippetCount() const;
		Snippet* snippet(int i) const;
		void removeSnippet(int i, bool cleanup = true);
		
		void addSnippet(Snippet *s);
		
		void reloadSnippetFromString(int i, const QString& s, int type = Simple);
		void reloadSnippetFromFile(int i, const QString& file, int type = Simple);
		
		Snippet* loadSnippetFromString(const QString& s, int type = Simple);
		Snippet* loadSnippetFromFile(const QString& file, int type = Simple);
		
	protected:
		virtual Snippet* loadFile(const QString& s, int type = Simple);
		virtual Snippet* loadString(const QString& s, int type = Simple);
		
	private:
		QList<Snippet*> m_snippets;
};

#endif // !_SNIPPETS_H_
