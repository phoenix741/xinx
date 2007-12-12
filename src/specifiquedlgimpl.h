/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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

#ifndef SPECIFIQUEDLGIMPL_H_
#define SPECIFIQUEDLGIMPL_H_

// Xinx header
#include "ui_specifiquedlg.h"
#include "globals.h"
#include "xinxconfig.h"

class SpecifiqueDialogImpl : public QDialog, public Ui::SpecifiqueDialog {
	Q_OBJECT
public:
	SpecifiqueDialogImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	
	static bool isSpecifique( const QString & filename );
	static bool canBeSaveAsSpecifique( const QString & filename );
	static bool canBeAddedToRepository( const QString & filename );
	
	void setFileName( const QString & filename );
	QString path() const;
	QString filename() const; 
	
	static QString saveFileAs( const QString & filename, const QString & suffix, QStringList & filesForRepository );
	static QString saveFileAsIfStandard( const QString & filename, QStringList & filesForRepository );
private slots:
private:
	static struct_extentions extentionOfFileName( const QString & name );
	
	QString m_filename, m_lastPlace, m_suffix;
};

#endif /*SPECIFIQUEDLGIMPL_H_*/
