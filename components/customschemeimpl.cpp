/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

// Xinx header
#include "customschemeimpl.h"

// QCodeEdit header
#include <qcodeedit.h>
#include <qeditor.h>
#include <qformatscheme.h>
#include <qformat.h>

// Qt header
#include <QHBoxLayout>

CustomSchemeImpl::CustomSchemeImpl( QWidget * parent, Qt::WindowFlags f ) : QWidget( parent, f ), m_formats( 0 ), m_currentFormat( 0 ) {
	setupUi( this );

	m_exampleEditor = new QCodeEdit( false, this );
	QHBoxLayout * hb = new QHBoxLayout;
	hb->addWidget( m_exampleEditor->editor() );
	m_exampleGroupBox->setLayout( hb );
}

CustomSchemeImpl::~CustomSchemeImpl() {
}

void CustomSchemeImpl::setFormatScheme( QFormatScheme * formats ) {
	if( formats != m_formats ) {
		m_formats = formats;

		m_formatsListView->addItems( m_formats->formats() );
	}
}

QFormatScheme * CustomSchemeImpl::formatScheme() const {
	return m_formats;
}

void CustomSchemeImpl::setExample( const QString & value ) {
	if( m_example != value ) {
		m_example = value;
		m_exampleEditor->editor()->setText( m_example );
		m_exampleGroupBox->setVisible( ! m_example.isEmpty() );
	}
}

const QString & CustomSchemeImpl::example() const {
	return m_example;
}

void CustomSchemeImpl::on_m_formatsListView_currentTextChanged( const QString & currentText ) {
	m_currentFormat = &(m_formats->formatRef( currentText ));
	m_boldCheckBox->setChecked( m_currentFormat->weight != QFont::Normal );
	m_overLineCheckBox->setChecked( m_currentFormat->overline );
	m_strikeOutCheckBox->setChecked( m_currentFormat->strikeout );
	m_italicCheckBox->setChecked( m_currentFormat->italic );
	m_underLineCheckBox->setChecked( m_currentFormat->underline || m_currentFormat->waveUnderline );
	m_waveUnderLineCheckBox->setChecked( m_currentFormat->waveUnderline );
	m_foreGroundComboBox->setColor( m_currentFormat->foreground );
	m_backGroundComboBox->setColor( m_currentFormat->background );
}

void CustomSchemeImpl::on_m_boldCheckBox_stateChanged( int state ) {
	if( state == Qt::Checked )
		m_currentFormat->weight = QFont::Bold;
	else
		m_currentFormat->weight = QFont::Normal;
}

void CustomSchemeImpl::on_m_overLineCheckBox_stateChanged( int state ) {
	m_currentFormat->overline = state == Qt::Checked;
}

void CustomSchemeImpl::on_m_strikeOutCheckBox_stateChanged( int state ) {
	m_currentFormat->strikeout = state == Qt::Checked;
}

void CustomSchemeImpl::on_m_italicCheckBox_stateChanged( int state ) {
	m_currentFormat->italic = state == Qt::Checked;
}

void CustomSchemeImpl::on_m_underLineCheckBox_stateChanged( int state ) {
	m_currentFormat->underline = (state == Qt::Checked) && (m_waveUnderLineCheckBox->isChecked() == Qt::Unchecked);
	m_currentFormat->waveUnderline = (state == Qt::Checked) && (m_waveUnderLineCheckBox->isChecked() == Qt::Checked);
}

void CustomSchemeImpl::on_m_waveUnderLineCheckBox_stateChanged( int state ) {
	m_currentFormat->underline = (m_underLineCheckBox->isChecked() == Qt::Checked) && (state == Qt::Unchecked);
	m_currentFormat->waveUnderline = (m_underLineCheckBox->isChecked() == Qt::Checked) && (state == Qt::Checked);
}

void CustomSchemeImpl::on_m_foreGroundComboBox_activated( const QColor &col ) {
	m_currentFormat->foreground = col;
}

void CustomSchemeImpl::on_m_backGroundComboBox_activated( const QColor &col ) {
	m_currentFormat->background = col;
}


