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

#ifndef SPECIFIQUEDLGIMPL_H_
#define SPECIFIQUEDLGIMPL_H_

// Xinx header
#include "ui_specifiquedlg.h"

// Qt header
#include <QPointer>

class ThreadedConfigurationFile;

class SpecifiqueDialogImpl : public QDialog, public Ui::SpecifiqueDialog {
	Q_OBJECT
public:
	virtual ~SpecifiqueDialogImpl();

	static void setLastPlace( const QString & pathname );
	static QString lastPlace();

	static QString saveFileAs( const QString & filename, const QString & defaultFileName, QStringList & filesForRepository );
	static QString saveFileAsIfStandard( const QString & filename, QStringList & filesForRepository );
private slots:
	void businessViewFinded( QStringList list );
private:
	SpecifiqueDialogImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );

	QString path() const;
	void setFileName( const QString & filename );
	QString filename() const;

	static bool isSpecifique( const QString & filename );
	static bool canBeSaveAsSpecifique( const QString & filename );
	static bool isInConfigurationFile( const QString & filename );
	static bool canBeAddedToRepository( const QString & filename );

	static QString m_lastPlace;
	QString m_filename, m_defaultFileName;
	QPointer<ThreadedConfigurationFile> m_instance;
};

#endif /*SPECIFIQUEDLGIMPL_H_*/
