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

#ifndef _CUSTOMSCHEMEIMPL_H_
#define _CUSTOMSCHEMEIMPL_H_
#pragma once

// Xinx header
#include "ui_customscheme.h"

class QCodeEdit;
class QFormatScheme;
class QFormat;
class QLanguageFactory;

/*!
 * Create a widget that permit to personalize the editor.
 */
class CustomSchemeImpl : public QWidget, public Ui::CustomScheme {
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v3 or later")

	Q_PROPERTY( QString example READ example WRITE setExample )
	Q_PROPERTY( QString languageDefinition READ languageDefinition WRITE setLanguageDefinition )
	Q_PROPERTY( QStringList hiddenFormat READ hiddenFormat WRITE setHiddenFormat )
public:
	/*!
	 * Construct the widget.
	 * \param parent The parent widget
	 * \param f Flags for the widget
	 */
	CustomSchemeImpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	//! Destroy the widget
	virtual ~CustomSchemeImpl();

	//! Set a new format scheme in the widget and configure it.
	void setFormatScheme( QFormatScheme * formats );

	//! Return the current format scheme.
	QFormatScheme * formatScheme() const;

	//! Set the exemple in the custom widget \e value
	void setExample( const QString & value );
	//! Return the exemple set
	const QString & example() const;

	//! Set the language definition to \e value
	void setLanguageDefinition( const QString & value );
	//! Return the language definition set.
	QString languageDefinition() const;

	//! Set the language factory used in the custom editor \e value
	void setLanguageFactory( QLanguageFactory * value );
	//! Return the language factory set.
	QLanguageFactory * languageFactory() const;

	//! Set the list of format that the widget musn't be show.
	void setHiddenFormat( const QStringList & value );
	//! Return the list of format to hide.
	const QStringList & hiddenFormat() const;
private slots:
	void on_m_formatsListView_currentItemChanged( QListWidgetItem * current, QListWidgetItem * previous );
	void on_m_boldCheckBox_stateChanged( int state );
	void on_m_overLineCheckBox_stateChanged( int state );
	void on_m_strikeOutCheckBox_stateChanged( int state );
	void on_m_italicCheckBox_stateChanged( int state );
	void on_m_underLineCheckBox_stateChanged( int state );
	void on_m_waveUnderLineCheckBox_stateChanged( int state );
	void on_m_foreGroundComboBox_activated( const QColor &col );
	void on_m_backGroundComboBox_activated( const QColor &col );
private:
	void updateFormatList();

	QLanguageFactory * m_languageFactory;
	QCodeEdit * m_exampleEditor;
	QFormatScheme * m_formats;
	QFormat * m_currentFormat;
	QListWidgetItem * m_currentItem;
	QString m_example;
	QStringList m_hiddenFormat;
};

#endif /* _CUSTOMSCHEMEIMPL_H_ */
