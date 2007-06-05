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

#include <assert.h>

#include "globals.h"
#include "xmlvisualstudio.h" 
#include "texteditor.h"
#include "fileeditor.h"
#include "xslproject.h"
#include "webservices.h"
#include "serviceresultdialogimpl.h"
#include "webserviceseditor.h"

void XMLVisualStudio::createWebServicesPart() {
	global.m_webServices = new WebServicesList();	
}

void XMLVisualStudio::setWebServicesView( bool enabled ) {
	if( enabled ) {
		refreshWebServicesList();
	} else {
		qDeleteAll( *(global.m_webServices) );
		global.m_webServices->clear();
	}
		
	m_webServicesMenu->setEnabled( enabled );
	m_webServicesToolBar->setEnabled( enabled );
}

void XMLVisualStudio::on_m_refreshWebServicesListAct_triggered() {
	refreshWebServicesList();
}

void XMLVisualStudio::refreshWebServicesList() {
	qDeleteAll( *(global.m_webServices) );
	global.m_webServices->clear();

	if( global.m_project ) {
		foreach( QString link, global.m_project->serveurWeb() ) {
			WebServices * ws = new WebServices( link, this );
			global.m_webServices->append( ws );
			ws->askWSDL( this );
			connect( ws, SIGNAL(soapResponse(QString,QString,QString,QString)), this, SLOT(webServicesReponse(QString,QString,QString,QString)) );
		}
		global.emitWebServicesChanged();
	}
}

void XMLVisualStudio::webServicesReponse( QString query, QString response, QString errorCode, QString errorString ) {
	ServiceResultDialogImpl * dlg = new ServiceResultDialogImpl( this );
	dlg->setInputStreamText( query );
	dlg->setOutputStreamText( response );
	dlg->show();
}

void XMLVisualStudio::on_m_callWebServicesAct_triggered() {
	assert( m_tabEditors->currentEditor() != NULL );
	assert( global.m_project );
	
	WebServicesEditor * editor = dynamic_cast<WebServicesEditor*>(m_tabEditors->currentEditor());
	if( editor ) {
		editor->service()->call( editor->operation(), editor->values() );
	}
	
	//if( TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) {
		//QTextEdit * ed = static_cast<WebServicesEditor*>( m_tabEditors->currentEditor() )->textEdit();
		//
		//NewWebServicesDialogImpl dlg;
		//if( dlg.exec() == QDialog::Accepted ) {
			//QStringList params;
			//params.append( ed->toPlainText() );
			//dlg.calledWebServices()->call( dlg.calledOperation(), params );
		//}
	//}
}

