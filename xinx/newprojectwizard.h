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

#ifndef _NEWPROJECTWIZARD_H_
#define _NEWPROJECTWIZARD_H_

// Qt header
#include <QWizard>

// Xinx header
#include "ui_newprojectwizard_project.h"
#include "ui_newprojectwizard_specifique.h"

class XSLProject;
class NewProjectWizard;
class IXinxPluginProjectConfiguration;

class ProjectPageImpl : public QWizardPage, private Ui::ProjectPage {
	Q_OBJECT
public:
	ProjectPageImpl( QWidget * parent = 0 );

	int nextId() const;

    virtual void setVisible( bool visible );
	virtual void initializePage();
	virtual bool isComplete () const;
private slots:
    void on_m_ASPathBtn_clicked();
    void on_m_logPathBtn_clicked();
    void on_m_projectPathBtn_clicked();

    void on_m_projectNameEdit_textChanged( const QString & value );
    void on_m_ASPathEdit_textChanged( const QString & value );
};

class SpecifiquePageImpl : public QWizardPage, private Ui::SpecifiquePage {
	Q_OBJECT
public:
	SpecifiquePageImpl( QWidget * parent = 0 );

	int nextId() const;
	virtual void initializePage();
};

class VersionsPageImpl : public QWizardPage {
	Q_OBJECT
public:
	VersionsPageImpl( QWidget * parent = 0 );

	int nextId() const;
private:
	QRadioButton * m_noRevisionControl;
	QList< QPair<QRadioButton*, QString> > m_revisionBtn;

	friend class NewProjectWizard;
};

class LastPageImpl : public QWizardPage {
	Q_OBJECT
public:
	LastPageImpl( QWidget * parent = 0 );

	int nextId() const;
private:
};

class NewProjectWizard : public QWizard {
	Q_OBJECT
public:
	enum { Page_Projet = 1, Page_Specifique = 2, Page_Versions = 3 };

	NewProjectWizard( QWidget * widget = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );

	XSLProject * createProject();
private slots:
	void on_customButton1_clicked();
private:
	XSLProject * m_project;
	QList< QPair<IXinxPluginProjectConfiguration*,QWizardPage*> > m_wizardPages;
	int m_lastPage;

	VersionsPageImpl * m_versions;
};



#endif
