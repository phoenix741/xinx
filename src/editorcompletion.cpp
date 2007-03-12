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
 
#include "editorcompletion.h"

#include <QtGui>
#include <QtXml>

CplNodeList * completionNodeList = NULL;

CplNode::CplNode(QObject * parent, const QDomElement & node) : QObject(parent) {
	m_name = node.tagName();
	if( ! node.prefix().isEmpty() )
		m_name = node.prefix() + ":" + m_name;
	QDomNamedNodeMap attr = node.attributes();
	
	for(int i = 0; i < attr.count(); i++) {
		if( attr.item( i ).isAttr() ) 
			m_params.push_back( attr.item( i ).toAttr().name() );
	}
}

void CplNodeList::loadFiles() {
  m_list.clear();

  QFile file(m_filesPath);
  QDomDocument objectFile;
  
  // Open the file
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(qApp->activeWindow(), tr("Completion"), tr("Cannot read file %1:\n%2.")
                                                        .arg(m_filesPath)
														.arg(file.errorString()));
    return;
  }

  // Load XML Document
  QString errorStr;
  int errorLine;
  int errorColumn;  
  if (!objectFile.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
    QMessageBox::information(qApp->activeWindow(), tr("Completion"), tr("Parse error at line %1, column %2:\n%3")
																.arg(errorLine)
                              									.arg(errorColumn)
																.arg(errorStr));
    return;
  }  
  
  QDomElement root = objectFile.documentElement();
  
  // Test if Completion
  if(root.tagName() != "completion") throw ENotCompletionFile(tr("%1 is not auto completion file").arg(m_filesPath));
  	
  // HTML
  QDomElement html = root.firstChildElement("html");

  QDomNode child = html.firstChild();
  while ((! child.isNull()) && (child.isElement())) {
    m_list.append(new CplNode(this, child.toElement()));
    child = child.nextSibling();
  }	

  // XSL
  QDomElement xsl = root.firstChildElement("xsl");

  child = xsl.firstChild();
  while ((! child.isNull()) && (child.isElement())) {
    m_list.append(new CplNode(this, child.toElement()));
    child = child.nextSibling();
  }	
}

CplNodeList::CplNodeList(const QString & name) {
	m_filesPath = name;
}

CplNode* CplNodeList::node(const QString & name) const {
	for(int i = 0; i < count(); i++) {
		if(name == node(i)->name())
			return node(i);
	}
	return NULL;
}
