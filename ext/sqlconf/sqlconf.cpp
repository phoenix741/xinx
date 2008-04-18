/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "sqlconf.h"
#include "xmlconfigurationreader.h"

sqlconf::sqlconf(QWidget *parent)  : QWidget(parent) {
	ui.setupUi(this);
	
	connect( ui.m_loadPushButton, SIGNAL(clicked()), this, SLOT(loadConfiguration()) );
}

sqlconf::~sqlconf() {

}

bool sqlconf::createConnection( QString name ) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName( name );
    if( ! db.open()) {
        QMessageBox::warning(this, tr("QXmlStream Configuration"),
                             tr("Unable to establish a database connection."));
        return false;
    }
    QSqlQuery query;
    query.exec("PRAGMA synchronous = OFF");
    query.exec("create table businessview_def (name varchar(255) primary key, "
               "target varchar(255), viewstruct varchar(255))");
    query.exec("create table presentation (businessview varchar(255), "
               "fileRef varchar(255), lang varchar(3), support varchar(7), type varchar(10))");
    query.exec("create table documentrow_def (name varchar(255), "
               "class varchar(255), service varchar(255))");
    query.exec("create table document_def (name varchar(255), "
               "class varchar(255), viewobject varchar(255), applicationmodule varchar(255), documentrow varchar(255))" );
    query.exec("create table view_def (name varchar(255), "
               "document varchar(255))" );
    query.exec("create table viewstruct_def (name varchar(255), "
               "defaultRetrieve varchar(255), defaultSort varchar(255))" );
    
    return true;
}

void sqlconf::findElement( const QString & element ) {
	ui.m_logView->addItem( element );
}

void sqlconf::loadConfiguration() {
	QString fileName = QFileDialog::getOpenFileName( this, "Configuration" );
	if( fileName.isEmpty() ) return;

    QFile file(fileName);
    if( ! file.open( QFile::ReadOnly | QFile::Text ) ) {
        QMessageBox::warning(this, tr("QXmlStream Configuration"),
                             tr("Cannot load file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }
    if(! createConnection( fileName + ".db" ) ) return;
    XmlConfigurationReader reader;
    connect( &reader, SIGNAL(findElement(QString)), this, SLOT(findElement(QString)) );
    if (!reader.read(&file)) {
        QMessageBox::warning(this, tr("QXmlStream Configuration"),
                             tr("Parse error in file %1 at line %2, column %3:\n%4")
                             .arg(fileName)
                             .arg(reader.lineNumber())
                             .arg(reader.columnNumber())
                             .arg(reader.errorString()));
    }
    QSqlDatabase::database().commit();
    QSqlDatabase::database().close();
    QSqlDatabase::removeDatabase( QLatin1String( QSqlDatabase::defaultConnection ) );
}

