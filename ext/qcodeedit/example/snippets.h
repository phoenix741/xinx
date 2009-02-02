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

class Snippet
{
	public:
		virtual ~Snippet() {}
		
		virtual QString name() const = 0;
		virtual QString identifier() const = 0;
		virtual void insert(QEditor *e) const = 0;
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
		const Snippet* snippet(int i) const;
		
		void loadSnippetFromString(const QString& s, int type = Simple);
		void loadSnippetFromFile(const QString& file, int type = Simple);
		
	protected:
		virtual Snippet* loadFile(const QString& s, int type = Simple);
		virtual Snippet* loadString(const QString& s, int type = Simple);
		
	private:
		QList<Snippet*> m_snippets;
};

#endif // !_SNIPPETS_H_
