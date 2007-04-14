/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#ifndef _EDITORCOMPLETION_H_
#define _EDITORCOMPLETION_H_

#include <QObject>
#include <QList>
#include <QStringList>

class QDomElement;

class ENotCompletionFile;

class CplNode : public QObject  {
	Q_OBJECT
public:
	CplNode(QObject * parent, const QDomElement & node);
	
	const QString & name() const { return m_name; };

	int count() const { return m_params.count(); };
	const QString & param(int i) const { return m_params.at(i); };
private:
	QString m_name;
	QList<QString> m_params;
};

class ENotCompletionFile {
public:
  ENotCompletionFile(const QString & message) {
    qDebug(message.toLatin1());
  }
};

class CplNodeList : public QObject {
	Q_OBJECT
public:
	CplNodeList(const QString & = "");

	void loadFiles();

	const QString & path() const { return m_filesPath; };
	void setPath(const QString & path) { m_filesPath = path; };

	int indexOf( CplNode * node, int from = 0 ) const { return m_list.indexOf( node, from ); };
	
	int count() const { return m_list.count(); };
	CplNode* node(int i) const { return m_list.at(i); };
	CplNode* node(const QString & name) const;
private:
	QList<CplNode*> m_list;
	QString m_filesPath;
};

extern CplNodeList * completionNodeList;

class CompletionXMLAttribute {
public:
	CompletionXMLAttribute( const QDomElement & node );
	~CompletionXMLAttribute();

	const QString & name() const { return m_name; };
	bool isDefault() const { return m_isDefault; };
	
	const QStringList & values() const { return m_values; };
	int defaultValue() const { return m_defaultValue; };
private:
	QString m_name;
	bool m_isDefault;
	
	QStringList m_values;
	int m_defaultValue;
};

class CompletionXMLBalise {
public:
	CompletionXMLBalise( const QString & category, const QDomElement & node );
	~CompletionXMLBalise();

	const QString & name() const { return m_name; };
	const QString & category() const { return m_category; };
	bool isDefault() const { return m_isDefault; };
	
	const QList<CompletionXMLAttribute*> & attributes() const { return m_attributes; };
	const QList<CompletionXMLBalise*> & balises() const { return m_balises; };
private:	
	QString m_name;
	bool m_isDefault;
	
	QString m_category;
	
	QList<CompletionXMLAttribute*> m_attributes;
	QList<CompletionXMLBalise*> m_balises;
};

class Completion {
public:
	Completion( const QString & name );
	~Completion();

	const QList<CompletionXMLBalise*> xmlBalises() { return m_xmlBalises; };
protected:
	void load();
private:
	QList<CompletionXMLBalise*> m_xmlBalises;
	QString m_name;
};

extern Completion * completionContents;

#endif
