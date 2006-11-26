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
 
#include <QtGui>
 
#include "javaobjectfileimpl.h"

//
JavaObjectFileImpl::JavaObjectFileImpl( ObjectsView * objects, QWidget * parent, Qt::WFlags f) : QDialog(parent, f) {
	setupUi(this);

    m_appObjects = objects;
    m_javaObjectPathEdit->setText(m_appObjects->path());
    	
    fillList(objects);
}
//

void JavaObjectFileImpl::on_m_javaObjectPathChange_clicked() {
  QString dir = QFileDialog::getExistingDirectory(this, tr("Java Object Directory"), m_javaObjectPathEdit->text());
  if(! dir.isEmpty())
	m_javaObjectPathEdit->setText(dir);
}

//
void JavaObjectFileImpl::on_m_javaObjectRefreshButton_clicked() {
  m_javaObjectList->clear();
  
  QDir dir(m_javaObjectPathEdit->text());
  
  if(dir.exists()) {
    ObjectsView m_tmpObjects(dir.path());
    connect(&m_tmpObjects, SIGNAL(objectLoaded(QString)), this, SLOT(addItem(QString)));
  	m_tmpObjects.loadFiles();
  } else {
    QMessageBox::information(qApp->activeWindow(), tr("Java Object Directory"), tr("Directory %1 don't exist").arg(dir.path()));  	 	
  }
}

//
void JavaObjectFileImpl::on_okButton_clicked() {
  QDir dir(m_javaObjectPathEdit->text());
  
  if(!dir.exists()) {
    QMessageBox::information(qApp->activeWindow(), tr("Java Object Directory"), tr("Directory %1 don't exist").arg(dir.path()));  	 	
  }
  
  m_appObjects->setPath(dir.path());
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_appObjects->loadFiles();
  QApplication::restoreOverrideCursor();
}
//

void JavaObjectFileImpl::on_m_javaObjectPathEdit_textChanged(QString text) {
  okButton->setEnabled(validDir(text));
}


void JavaObjectFileImpl::addItem(const QString & name) {
  m_javaObjectList->addItem(name);
}

void JavaObjectFileImpl::fillList(ObjectsView* objs) {
  m_javaObjectList->clear();

  for(int i = 0; i < objs->count(); i++) {
    m_javaObjectList->addItem(objs->objectViews(i)->name());
  }
}

bool JavaObjectFileImpl::validDir(const QString & name) {
  QDir dir(name);
  return (!name.isEmpty()) && (dir.exists());
}

