/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "templatedialogimpl.h"
#include <project/newprojecttemplate.h>

// Qt header
#include <QDir>
#include <QFileInfoList>

class TemplateWidgetItem : public QTreeWidgetItem {
public:
	TemplateWidgetItem( QTreeWidgetItem * parent ) : QTreeWidgetItem( parent ) {}

	NewProjectTemplate * m_projectTemplate;
};

/* TemplateDialogImpl */

TemplateDialogImpl::TemplateDialogImpl() {
	setupUi( this );

	loadDirectory( "templates:", m_template->invisibleRootItem() );

	connect( m_template, SIGNAL(itemSelectionChanged()), this, SIGNAL(completeChanged()) );
	m_template->expandAll();
}

TemplateDialogImpl::~TemplateDialogImpl() {
	m_template->clear();
	qDeleteAll( m_templates );
}

bool TemplateDialogImpl::isComplete() const {
	QTreeWidgetItem * currentItem = m_template->currentItem();
	return dynamic_cast<TemplateWidgetItem*>( currentItem );
}

QString TemplateDialogImpl::pagePluginId() const {
	return "StdTemplate";
}

bool TemplateDialogImpl::pageIsVisible() const {
	return true;
}

bool TemplateDialogImpl::saveSettingsDialog( XinxProject * project ) {
	Q_UNUSED( project );
	return true;
}

void TemplateDialogImpl::loadDirectory( const QString & directory, QTreeWidgetItem * item ) {
	QDir templateDirectory( directory );

	QFileInfoList files = templateDirectory.entryInfoList( QStringList() << "*.xml", QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot );
	foreach( const QFileInfo & info, files ) {
		if( info.isDir() ) {
			QTreeWidgetItem * dirItem = new QTreeWidgetItem( item );
			dirItem->setText( 0, info.baseName() );
			dirItem->setIcon( 0, style()->standardIcon( QStyle::SP_DirIcon ) );

			loadDirectory( info.absoluteFilePath(), dirItem );
		} else {
			NewProjectTemplate * projectTemplate = new NewProjectTemplate( info.absoluteFilePath() );

			TemplateWidgetItem * templateItem = new TemplateWidgetItem( item );
			templateItem->setIcon( 0, QIcon( ":/images/filenew.png" ) );
			templateItem->setText( 0, projectTemplate->projectName() );
			templateItem->m_projectTemplate = projectTemplate;

			m_templates.append( projectTemplate );
		}
	}
}

NewProjectTemplate * TemplateDialogImpl::currentTemplate() const {
	TemplateWidgetItem * currentItem = dynamic_cast<TemplateWidgetItem*>( m_template->currentItem() );
	if( currentItem )
		return currentItem->m_projectTemplate;

	return 0;
}

