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

#include "objectview.h"

#include <QDomElement>
#include <QDomDocument>
#include <QMessageBox>
#include <QApplication>
#include <QDir>

/****************** ObjectFields *********************/

ObjectFields::ObjectFields(QObject * parent, const QDomElement & node) : QObject(parent) {
	m_name = node.attribute( "Name", "" );
	if ( node.attribute( "IsUpdateable", "true" ) == "false" )
		m_isUpdateable = false;
	else
		m_isUpdateable = true;
		
	if ( node.attribute( "IsNotNull", "true" ) == "false" )
		m_isNotNull = false;
	else
		m_isNotNull = true;
		
	if ( node.attribute( "IsQueriable", "true" ) == "false" )
		m_isQueriable = false;
	else
		m_isQueriable = true;
}
  
const QString & ObjectFields::name() const {
	return m_name;
}

bool ObjectFields::isUpdateable() const {
	return m_isUpdateable;
}

bool ObjectFields::isNotNull() const {
	return m_isNotNull;
}

bool ObjectFields::isQueriable() const {
	return m_isQueriable;
}

/****************** ObjectView *********************/

ObjectView::ObjectView( QObject* parent, const QString & fileName ) : QObject(parent) {
	QFile file(fileName);
	QDomDocument objectFile;
  
	// Open the file
	if ( !file.open( QFile::ReadOnly | QFile::Text ) ) {
		QMessageBox::warning(qApp->activeWindow(), 
			tr("XSL Stylesheet"), 
			tr("Cannot read file %1:\n%2.")
				.arg(fileName)
				.arg(file.errorString()));
		return;
	}

	// Load XML Document
	QString errorStr;
	int errorLine = 0;
	int errorColumn = 0;  
	if (!objectFile.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
		QMessageBox::information(qApp->activeWindow(), 
			tr("XSL Stylesheet"), 
			tr("Parse error at line %1, column %2:\n%3")
				.arg(errorLine)
                .arg(errorColumn)
				.arg(errorStr));
		return;
	}  
  
	QDomElement root = objectFile.documentElement();
  
	// Test if ViewObject
	if(root.tagName() != "ViewObject") throw ENotViewObject(tr("%1 not a ViewObject").arg(fileName));

	// Update attribute  	
	m_name = root.attribute("Name");

	// Update ObjectFields
	QDomElement child = root.firstChildElement("ViewAttribute");
	while (! child.isNull()) {
		m_fields.append(new ObjectFields(this, child));
		child = child.nextSiblingElement("ViewAttribute");
	}    
}

ObjectView::~ObjectView() {
	// QObject free is child
}
 
const QString & ObjectView::name() const {
	return m_name;
}

ObjectFields * ObjectView::fields(int index) const {
	return m_fields.at(index);
}

int ObjectView::count() const {
	return m_fields.size();
}

/****************** ObjectsView *********************/

ObjectsView::ObjectsView(const QString & filesPath) {
	m_filesPath = filesPath;
}

ObjectsView::~ObjectsView() {
	// Auto destroy
}
  
void ObjectsView::loadFiles() {
	QDir path(m_filesPath);

	m_objectsview.clear();	
	if((m_filesPath.isEmpty()) || (! path.exists())) return ;
  
	path.setFilter(QDir::Files | QDir::NoSymLinks);
  
	QStringList list = path.entryList(QStringList("*.xml"));
	for (int i = 0; i < list.size(); i++) {
		QString fileInfo = list.at(i);
		try {
			ObjectView * o = new ObjectView(this, path.absoluteFilePath(fileInfo));
			m_objectsview.insert(o->name(), o);
			emit objectLoaded(o->name());
		} catch(ENotViewObject) { /* Continue */ }
	}
}
  
 
ObjectView * ObjectsView::objectViews( const QString & name ) const {
	return m_objectsview.value(name);
}

ObjectView * ObjectsView::objectViews(int i) const {
	return m_objectsview.values().at(i);
}

int ObjectsView::count() const {
	return m_objectsview.size();
}
