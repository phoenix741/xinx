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

/*!
 * This widget is a list of value (exemple: list of path, list of urls, list of prefix) with
 * a button to add an value, a button to delete a value, and a button to make a value the 
 * default.
 */
class XinxListWidgetImpl : public QWidget, public Ui::XinxListWidget {
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("Licence", "GPL v2 or later")
	Q_PROPERTY( bool defaultVisible READ defaultVisible WRITE setDefaultVisible )
	Q_PROPERTY( QString defaultValue READ defaultValue WRITE setDefaultValue )
	Q_PROPERTY( QStringList values READ values WRITE setValues )
	Q_PROPERTY( QString defaultProposedValue READ defaultProposedValue WRITE setDefaultProposedValue )
public:
	/*! 
	 * Construct the widget with a default, add and remove button.
	 * \param parent The parent widget
	 * \param f Flags for the widget
	 */
	XinxListWidgetImpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	/*!
	 * Destroy the widget
	 */
	virtual ~XinxListWidgetImpl();
	
	/*!
	 * Retrieve the default value selected by the user.
	 */
	QString defaultValue() const;
	/*!
	 * Change the default value. 
	 * If \e value isn't in the list, the value is added.
	 */
	void setDefaultValue( const QString & value );
	
	/*! 
	 * Retrieve the visibility of the default button.
	 */
	bool defaultVisible() const;
	/*!
	 * Set the visibility of the default button. If \e visible is set to false,
	 * the default button is hidden and the user can't choose a default value.
	 */
	void setDefaultVisible( bool visible );

	/*! 
	 * Return the list of value used by the widget.
	 */
	QStringList values() const;
	/*!
	 * Change the list of value used by the widget. If the defaultValue isn't in
	 * the list, the default value is added.
	 */
	void setValues( const QStringList & values );
	
	/*!
	 * Retrieve the default value proposed to the user, when the add a value to the list.
	 */
	QString defaultProposedValue() const;
	/*!
	 * Set the default value proposed to the user.
	 */
	void setDefaultProposedValue( const QString & value );
signals:
	/*!
	 * This signal is emited when the user change the value \e value.
	 */
	void defaultValueChanged( QString value );
private slots:
	void on_m_btnDef_clicked();
	void on_m_btnAdd_clicked();
	void on_m_btnDel_clicked();
private:
	void updateDefault( const QString & def );
	int m_defaultValue;
	QString m_defaultProposedValue;
};

#endif /*XINXLISTXIDGET_H_*/
