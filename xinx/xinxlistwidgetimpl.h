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

#ifndef XINXLISTWIDGET_H_
#define XINXLISTWIDGET_H_

// Xinx header
#include "ui_xinxlistwidget.h"

class XinxListWidgetImpl : public QWidget, public Ui::XinxListWidget {
	Q_OBJECT
	Q_PROPERTY( bool defaultVisible READ defaultVisible WRITE setDefaultVisible )
	Q_PROPERTY( QString defaultValue READ defaultValue WRITE setDefaultValue )
public:
	XinxListWidgetImpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	virtual ~XinxListWidgetImpl();
	
	QString defaultValue() const;
	void setDefaultValue( const QString & value );
	
	bool defaultVisible() const;
	void setDefaultVisible( bool visible );

	QStringList values() const;
	void setValues( const QStringList & values );
	
	QString valueName() const;
	void setValueName( const QString & value );
	
	QString defaultProposedValue() const;
	void setDefaultProposedValue( const QString & value );
signals:
	void defaultValueChanged( QString value );
private slots:
	void on_m_lineEdit_textChanged( QString text );
	void on_m_btnDef_clicked();
	void on_m_btnAdd_clicked();
	void on_m_btnDel_clicked();
private:
	void updateDefault();
	QString m_defaultValue, m_valueName, m_defaultProposedValue;
};

#endif /*XINXLISTXIDGET_H_*/
