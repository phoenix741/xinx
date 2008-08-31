/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke   *
 *   ulrich.vdh@free.fr   *
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

#ifndef PROJECTWIZARD_H_
#define PROJECTWIZARD_H_

// Qt header
#include <QWizard>

class QLabel;
class QProgressBar;
class QCheckBox;
class DirectoryEditWidget;
class ProjectConverter;

class ProjectWizard : public QWizard {
	Q_OBJECT
public:
	ProjectWizard( QString filname, QWidget * parent = 0 );

	ProjectConverter * converter() const;
	void setConverter( ProjectConverter * c );

	virtual void accept();
private:
	ProjectConverter * m_converter;
	QString m_filename;
};

class FileWizardPage : public QWizardPage {
	Q_OBJECT
public:
	FileWizardPage( QString filename, QWidget * parent = 0 );

	virtual void initializePage();
	virtual bool validatePage();
private:
	QString m_filename;
	DirectoryEditWidget * m_projectEdit;
};

class VersionWizardPage : public QWizardPage {
	Q_OBJECT
public:
	VersionWizardPage( QWidget * parent = 0 );

	virtual void initializePage();
private:
	QLabel * m_resume;
};

class ProgressWizardPage : public QWizardPage {
	Q_OBJECT
public:
	ProgressWizardPage( QWidget * parent = 0 );

	virtual void initializePage ();
private slots:
	void processMessages();
private:
	QProgressBar * m_progressBar;
};

class ConclusionWizardPage : public QWizardPage {
	Q_OBJECT
public:
	ConclusionWizardPage( QWidget * parent = 0 );
private:
	QCheckBox * m_openCheck;
};

#endif /*PROJECTWIZARD_H_*/
