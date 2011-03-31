/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Xinx header
#include "customschemeimpl_p.h"

// QCodeEdit header
#include <qcodeedit.h>
#include <qeditor.h>
#include <qformatscheme.h>
#include <qformat.h>
#include <qdocument.h>
#include <qlanguagefactory.h>
#include <qlanguagedefinition.h>

// Qt header
#include <QHBoxLayout>

/* CustomSchemeImplPrivate */

CustomSchemeImplPrivate::CustomSchemeImplPrivate() : _ui(new Ui::CustomScheme), m_languageFactory(0), m_formats(0), m_currentFormat(0), m_currentItem(0)
{

}

CustomSchemeImplPrivate::~CustomSchemeImplPrivate()
{
}

void CustomSchemeImplPrivate::on_m_formatsListView_currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous)
{
	if (current != m_currentItem)
	{
		m_currentItem = current;

		if (m_currentItem && m_formats)
		{
			m_currentFormat = &(m_formats->formatRef(m_currentItem->text()));

			_ui->m_boldCheckBox->setChecked(m_currentFormat->weight != QFont::Normal);
			_ui->m_overLineCheckBox->setChecked(m_currentFormat->overline);
			_ui->m_strikeOutCheckBox->setChecked(m_currentFormat->strikeout);
			_ui->m_italicCheckBox->setChecked(m_currentFormat->italic);
			_ui->m_underLineCheckBox->setChecked(m_currentFormat->underline || m_currentFormat->waveUnderline);
			_ui->m_waveUnderLineCheckBox->setChecked(m_currentFormat->waveUnderline);
			_ui->m_foreGroundComboBox->setColor(m_currentFormat->foreground);
			_ui->m_backGroundComboBox->setColor(m_currentFormat->background);
		}
		_ui->m_formatGroupBox->setEnabled(m_currentItem);
		_ui->m_colorGroupBox->setEnabled(m_currentItem);
		_ui->m_exampleGroupBox->setEnabled(m_currentItem);

	}
}

void CustomSchemeImplPrivate::on_m_boldCheckBox_stateChanged(int state)
{
	if (state == Qt::Checked)
		m_currentFormat->weight = QFont::Bold;
	else
		m_currentFormat->weight = QFont::Normal;
	m_currentItem->setFont(m_currentFormat->toTextCharFormat().font());
	m_exampleEditor->editor()->document()->setFormatScheme(m_formats);
}

void CustomSchemeImplPrivate::on_m_overLineCheckBox_stateChanged(int state)
{
	m_currentFormat->overline = state == Qt::Checked;
	m_currentItem->setFont(m_currentFormat->toTextCharFormat().font());
	m_exampleEditor->editor()->document()->setFormatScheme(m_formats);
}

void CustomSchemeImplPrivate::on_m_strikeOutCheckBox_stateChanged(int state)
{
	m_currentFormat->strikeout = state == Qt::Checked;
	m_currentItem->setFont(m_currentFormat->toTextCharFormat().font());
	m_exampleEditor->editor()->document()->setFormatScheme(m_formats);
}

void CustomSchemeImplPrivate::on_m_italicCheckBox_stateChanged(int state)
{
	m_currentFormat->italic = state == Qt::Checked;
	m_currentItem->setFont(m_currentFormat->toTextCharFormat().font());
	m_exampleEditor->editor()->document()->setFormatScheme(m_formats);
}

void CustomSchemeImplPrivate::on_m_underLineCheckBox_stateChanged(int state)
{
	m_currentFormat->underline = (state == Qt::Checked) && (_ui->m_waveUnderLineCheckBox->isChecked() == Qt::Unchecked);
	m_currentFormat->waveUnderline = (state == Qt::Checked) && (_ui->m_waveUnderLineCheckBox->isChecked() == Qt::Checked);
	m_currentItem->setFont(m_currentFormat->toTextCharFormat().font());
	m_exampleEditor->editor()->document()->setFormatScheme(m_formats);
}

void CustomSchemeImplPrivate::on_m_waveUnderLineCheckBox_stateChanged(int state)
{
	m_currentFormat->underline = _ui->m_underLineCheckBox->isChecked() && (state == Qt::Unchecked);
	m_currentFormat->waveUnderline = _ui->m_underLineCheckBox->isChecked() && (state == Qt::Checked);
	m_currentItem->setFont(m_currentFormat->toTextCharFormat().font());
	m_exampleEditor->editor()->document()->setFormatScheme(m_formats);
}

void CustomSchemeImplPrivate::on_m_foreGroundComboBox_activated(const QColor &col)
{
	m_currentFormat->foreground = col;
	if (m_currentFormat->background.isValid()) m_currentItem->setBackground(m_currentFormat->background);
	m_exampleEditor->editor()->document()->setFormatScheme(m_formats);
}

void CustomSchemeImplPrivate::on_m_backGroundComboBox_activated(const QColor &col)
{
	m_currentFormat->background = col;
	if (m_currentFormat->foreground.isValid()) m_currentItem->setForeground(m_currentFormat->foreground);
	m_exampleEditor->editor()->document()->setFormatScheme(m_formats);
}

void CustomSchemeImplPrivate::updateFormatList()
{
	_ui->m_formatsListView->clear();

	if (m_formats)
	{
		foreach(const QString & f, m_formats->formats())
		{
			QFormat format = m_formats->format(f);
			QListWidgetItem * item = new QListWidgetItem(f, _ui->m_formatsListView);
			if (format.background.isValid()) item->setBackground(format.background);
			if (format.foreground.isValid()) item->setForeground(format.foreground);
			item->setFont(format.toTextCharFormat().font());
			item->setHidden(m_hiddenFormat.contains(f));
		}
	}

	if (_ui->m_formatsListView->count())
		_ui->m_formatsListView->setCurrentRow(0);
}


/* CustomSchemeImpl */

/*!
 * \ingroup Components
 * \class CustomSchemeImpl
 * \since 0.8.0.0
 *
 * \brief Class to help the user to customize the QCodeEdit editor.
 *
 * \bc 0.10.0.0
 *
 * Create a widget that allows to customize the QCodeEdit editor. This program uses
 * QLanguageFactory and QFormatScheme to show the list of available formats to
 * change.
 *
 * The method setHiddenFormat() is used to hide some format of the list to forbide
 * the user to change it.
 *
 * \image html customschemeimpl1.png
 * \image latex customschemeimpl1.png
 *
 * When the user selects a format, the widget updates chexkboxes, and comboboxes to
 * the new format.
 *
 * All changes made on a format are shown on the example part of the widget.
 */

/*!
 * \brief Construct the widget.
 * \param parent The parent widget
 * \param f Flags for the widget
 */
CustomSchemeImpl::CustomSchemeImpl(QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f), d(new CustomSchemeImplPrivate)
{
	d->_ui->setupUi(this);

	d->m_exampleEditor = new QCodeEdit(false, this);
	QHBoxLayout * hb = new QHBoxLayout;
	hb->addWidget(d->m_exampleEditor->editor());
	d->_ui->m_exampleGroupBox->setLayout(hb);
	d->_ui->m_exampleGroupBox->setVisible(false);
}

//! Destroy the widget
CustomSchemeImpl::~CustomSchemeImpl()
{
}

/*!
 * \brief Set a new format scheme in the widget and configure it.
 * \sa formatScheme()
 */
void CustomSchemeImpl::setFormatScheme(QFormatScheme * formats)
{
	if (formats != d->m_formats)
	{
		d->m_formats = formats;
		d->m_exampleEditor->editor()->document()->setFormatScheme(d->m_formats);
		d->updateFormatList();
	}
}

/*!
 * \brief Return the current format scheme.
 * \sa setFormatScheme()
 */
QFormatScheme * CustomSchemeImpl::formatScheme() const
{
	return d->m_formats;
}

/*!
 * \brief Set the exemple in the custom widget to \p value.
 * \sa example()
 */
void CustomSchemeImpl::setExample(const QString & value)
{
	if (d->m_example != value)
	{
		d->m_example = value;
		d->m_exampleEditor->editor()->setText(d->m_example);
		d->_ui->m_exampleGroupBox->setVisible(! d->m_example.isEmpty());
	}
}

/*!
 * \property CustomSchemeImpl::example
 * \brief Returns the exemple defined in the object.
 * \sa setExample(), example
 */
const QString & CustomSchemeImpl::example() const
{
	return d->m_example;
}

/*!
 * \brief Set the language factory used in the custom editor to \p value
 * \sa languageFactory()
 */
void CustomSchemeImpl::setLanguageFactory(QLanguageFactory * value)
{
	if (value != d->m_languageFactory)
	{
		d->m_languageFactory = value;
	}
}

/*!
 * \brief Return the language factory defined in the object.
 * \sa setLanguageFactory()
 */
QLanguageFactory * CustomSchemeImpl::languageFactory() const
{
	return d->m_languageFactory;
}

/*!
 * \brief Set the language definition to \p value.
 * \sa languageDefinition()
 */
void CustomSchemeImpl::setLanguageDefinition(const QString & value)
{
	if (d->m_languageFactory)
	{
		d->m_languageFactory->setLanguage(d->m_exampleEditor->editor(), value);
	}
}

/*!
 * \property CustomSchemeImpl::languageDefinition
 * \brief Return the language definition defined in the object.
 * \sa setLanguageDefinition()
 */
QString CustomSchemeImpl::languageDefinition() const
{
	if (d->m_exampleEditor->editor()->languageDefinition())
		return d->m_exampleEditor->editor()->languageDefinition()->language();
	else
		return QString();
}

/*!
 * \brief Set the list of format that the widget does not show.
 * \sa hiddenFormat()
 */
void CustomSchemeImpl::setHiddenFormat(const QStringList & value)
{
	if (d->m_hiddenFormat != value)
	{
		d->m_hiddenFormat = value;
		d->updateFormatList();
	}
}

/*!
 * \property CustomSchemeImpl::hiddenFormat
 * \brief Return the list of format to hide.
 * \sa setHiddenFormat()
 */
const QStringList & CustomSchemeImpl::hiddenFormat() const
{
	return d->m_hiddenFormat;
}


