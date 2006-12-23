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
  if(root.tagName() != "completion") throw ENotCompletionFile(tr("%1 is not for auto completion").arg(m_filesPath));
  	
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


/* CplModel */

/* Plutot raté ;) 

CplModel::CplModel(CplNodeList * nodeList, QObject *parent) : QAbstractItemModel(parent) {
	m_rootItem = nodeList;
}

QModelIndex CplModel::index(int row, int column, const QModelIndex &parent) const {
	qDebug() << "index (" << row << ", " << column << ", " << parent.isValid() << ", " << parent.row() << ")";
	if (!parent.isValid()) {
		return createIndex(row, column);
	} else {
		if(parent.parent().isValid()) {
			return createIndex( row, column, parent.parent().internalPointer() );
		} else {
			CplNode * nodeName = m_rootItem->node( parent.row() );
			return createIndex( row, column, nodeName );
		}
	}
}

QModelIndex CplModel::parent ( const QModelIndex & index ) const {
	qDebug() << "parent (" << index.isValid() << ", " << index.row() << ", " << index.internalPointer() << ")";
	if(index.isValid() && index.internalPointer()) {
		qDebug() << index.internalPointer();
		qDebug() << m_rootItem->indexOf( static_cast<CplNode*>( index.internalPointer() ) );
		qDebug() << m_rootItem->count();
		return createIndex( m_rootItem->indexOf( static_cast<CplNode*>( index.internalPointer() ) ), 0 );
	} else
		return QModelIndex();
}

int CplModel::rowCount(const QModelIndex &parent) const {
	qDebug() << "rowCount (" << parent.isValid() << ", " << parent.row() << ", " << parent.internalPointer() << ")";
	if(!parent.isValid())
		return m_rootItem->count();
	else {
		if( parent.internalPointer() ) {
			CplNode * nodeName = static_cast<CplNode*>(parent.internalPointer());
			return nodeName->count();
//			return 0;
		} else {
			CplNode * nodeName 	= m_rootItem->node( parent.row() );
			return nodeName->count();
		}
	}
}

int CplModel::columnCount(const QModelIndex &parent) const {
	qDebug() << "columnCount (" << parent.isValid() << ", " << parent.row() << ", " << parent.internalPointer() << ")";
	return 1;
}

QVariant CplModel::data(const QModelIndex &index, int role) const {
	qDebug() << "data (" << index.isValid() << ", " << index.row() << ", " << index.internalPointer() << ")";
	if (!index.isValid()) return QVariant();

	if (role != Qt::DisplayRole) return QVariant();

	qDebug() << "data parent valid (" << index.parent().isValid() << ")";
	if(index.parent().isValid()) {
		CplNode * node = static_cast<CplNode*>( index.internalPointer() );
		qDebug() << "Index Param : " << index.row();
		qDebug() << "Count Param : " << node->count();
		return node->param( index.row() );
	} else {
		qDebug() << "Index Node : " << index.row();
		qDebug() << "Count Node : " << m_rootItem->count();
		return m_rootItem->node ( index.row() )->name();
	}
}

Qt::ItemFlags CplModel::flags(const QModelIndex &index) const {
     return QAbstractItemModel::flags(index);	
}

QVariant CplModel::headerData(int section, Qt::Orientation orientation, int role) const {
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
     	switch(section) {
     		case 0 : return tr("Node");
			default : return QVariant();
   		}

     return QVariant();
}
*/

