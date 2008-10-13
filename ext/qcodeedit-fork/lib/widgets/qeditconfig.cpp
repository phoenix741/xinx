/****************************************************************************
**
** Copyright (C) 2006-2008 fullmetalcoder <fullmetalcoder@hotmail.fr>
**
** This file is part of the Edyuk project <http://edyuk.org>
** 
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qeditconfig.h"

#include "qeditor.h"
#include "qdocument.h"
#include "qdocument_p.h"

#include <QDebug>

/*!
	\file qeditconfig.cpp
	\brief Implementation of the QEditConfig class.
	
	\see QEditConfig
*/

/*!
	\ingroup dialogs
	@{
	
	\class QEditConfig
	\brief A widget for embedding into a larger configuration dialog.
	
	The widget's values (that is its subcontrols) are set with setValues.
	The editted values are read out with extractValues().	
*/

QEditConfig::QEditConfig(QWidget *w)
 : QWidget(w), m_loading(false)
{
	setupUi(this);
	
	// disable not yet supported properties
	// remove these once the properties are supported
	//chkReplaceTabs->setEnabled(false);
	//chkAutoRemoveTrailingWhitespace->setEnabled(false);
}

/*!

*/
void QEditConfig::retranslate()
{
	retranslateUi(this);
}

/*!
	\brief Fills the widget subcontrols from settings map \a keys
	
	\param keys a key/value map that can be obtained from QSettings
*/
void QEditConfig::loadKeys(const QMap<QString, QVariant>& keys)
{
	//qDebug("loading %i keys", keys.count());
	
	// set current
	m_loading = true;
	
	if ( !QDocumentPrivate::m_font )
	{
		QDocument::setFont(QFont("Monospace", 12));
	}
	
	cbFont->setFont(QDocument::font());
	spnFontSize->setValue(QDocument::font().pointSize());
	
	spnTabWidth->setValue(QDocument::tabStop());
	chkShowTabsInText->setChecked(QDocument::showSpaces() & QDocument::ShowTabs);
	chkShowLeadingWhitespace->setChecked(QDocument::showSpaces() & QDocument::ShowLeading);
	chkShowTrailingWhitespace->setChecked(QDocument::showSpaces() & QDocument::ShowTrailing);
	
	m_loading = false;
	
	// load
	QMap<QString, QVariant>::const_iterator it = keys.constBegin();
	
	while ( it != keys.constEnd() )
	{
		if ( it.key() == "font" )
		{
			QFont f = qvariant_cast<QFont>(*it);
			
			cbFont->setCurrentFont(f);
			spnFontSize->setValue(f.pointSize());
			
			QDocument::setFont(f);
			lblSampleText->setFont(f);
			
		} else if ( it.key() == "tab_width" ) {
			on_spnTabWidth_valueChanged(it->toInt());
		} else if ( it.key() == "replace_tabs" ) {
			on_chkReplaceTabs_toggled(it->toBool());
		} else if ( it.key() == "remove_trailing" ) {
			on_chkAutoRemoveTrailingWhitespace_toggled(it->toBool());
		} else if ( it.key() == "preserve_trailing_indent" ) {
			on_chkPreserveTrailingIndent_toggled(it->toBool());
		} else if ( it.key() == "show_tabs_in_text" ) {
			on_chkShowTabsInText_toggled(it->toBool());
		} else if ( it.key() == "show_leading_whitespace" ) {
			on_chkShowLeadingWhitespace_toggled(it->toBool());
		} else if ( it.key() == "show_trailing_whitespace" ) {
			on_chkShowTrailingWhitespace_toggled(it->toBool());
		} else {
			/*
			qWarning("QEditConfig::loadKeys() fed with unsupported settings key : \"%s\" ",
					qPrintable(it.key())
					);
			*/
			
		}
		
		++it;
	}
}

/*!
	\brief Slot used to apply font size settings
*/
void QEditConfig::on_spnFontSize_valueChanged(int size)
{
	QFont font = cbFont->currentFont();
	font.setPointSize(size);
	
	lblSampleText->setFont(font);
	
	if ( !m_loading )
	{
		QDocument::setFont(font);
	}
	
	emit keyChanged("font", font);
}

/*!
	\brief Slot used to apply font family settings
*/
void QEditConfig::on_cbFont_currentFontChanged(QFont font)
{
	font.setPointSize(spnFontSize->value());
	lblSampleText->setFont(font);
	
	if ( !m_loading )
	{
		QDocument::setFont(font);
	}
	
	emit keyChanged("font", font);
}

/*!
	\brief Slot used to apply tab width settings
*/
void QEditConfig::on_spnTabWidth_valueChanged(int n)
{
	if ( !m_loading )
	{
		QDocument::setTabStop(n);
	}
	
	emit keyChanged("tab_width", n);
}

/*!
	\brief Slot used to apply tabs replacement settings
*/
void QEditConfig::on_chkReplaceTabs_toggled(bool y)
{
	if ( !m_loading )
	{
		foreach ( QEditor *e, QEditor::m_editors )
		{
			e->setFlag(QEditor::ReplaceTabs, y);
		}
	}
	
	emit keyChanged("replace_tabs", y);
}

/*!
	\brief Slot used to apply tabs replacement settings
*/
void QEditConfig::on_chkAutoRemoveTrailingWhitespace_toggled(bool y)
{
	if ( !m_loading )
	{
		foreach ( QEditor *e, QEditor::m_editors )
		{
			e->setFlag(QEditor::RemoveTrailing, y);
		}
	}
	
	chkPreserveTrailingIndent->setEnabled(true);
	
	emit keyChanged("remove_trailing", y);
}

/*!
	\brief Slot used to apply tabs replacement settings
*/
void QEditConfig::on_chkPreserveTrailingIndent_toggled(bool y)
{
	if ( !m_loading )
	{
		foreach ( QEditor *e, QEditor::m_editors )
		{
			e->setFlag(QEditor::PreserveTrailingIndent, y);
		}
	}
	
	emit keyChanged("preserve_trailing_indent", y);
}

/*!
	\brief Slot used to apply tabs display settings
*/
void QEditConfig::on_chkShowTabsInText_toggled(bool y)
{
	if ( !m_loading )
	{
		if ( y )
			QDocument::setShowSpaces(QDocument::showSpaces() | QDocument::ShowTabs);
		else
			QDocument::setShowSpaces(QDocument::showSpaces() & ~QDocument::ShowTabs);
		
	}
	
	emit keyChanged("show_tabs_in_text", y);
}

/*!
	\brief Slot used to apply trailing whitespace display settings
*/
void QEditConfig::on_chkShowLeadingWhitespace_toggled(bool y)
{
	if ( !m_loading )
	{
		if ( y )
			QDocument::setShowSpaces(QDocument::showSpaces() | QDocument::ShowLeading);
		else
			QDocument::setShowSpaces(QDocument::showSpaces() & ~QDocument::ShowLeading);
		
	}
	
	emit keyChanged("show_leading_whitespace", y);
}

/*!
	\brief Slot used to apply leading whitespace display settings
*/
void QEditConfig::on_chkShowTrailingWhitespace_toggled(bool y)
{
	if ( ! m_loading )
	{
		if ( y )
			QDocument::setShowSpaces(QDocument::showSpaces() | QDocument::ShowTrailing);
		else
			QDocument::setShowSpaces(QDocument::showSpaces() & ~QDocument::ShowTrailing);
		
	}
	
	emit keyChanged("show_trailing_whitespace", y);
}

/*! @} */
