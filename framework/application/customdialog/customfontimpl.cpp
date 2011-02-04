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
#include "customfontimpl.h"
#include <core/xinxconfig.h>

/* CustomFontImpl */

CustomFontImpl::CustomFontImpl(QWidget * parent) : QWidget(parent)
{
	setupUi(this);
}

CustomFontImpl::~CustomFontImpl()
{
}

QPixmap CustomFontImpl::image()
{
	return QPixmap(":/images/preferences-font.png");
}

QString CustomFontImpl::name()
{
	return windowTitle();
}

QWidget * CustomFontImpl::settingsDialog()
{
	return this;
}

bool CustomFontImpl::loadSettingsDialog()
{
	// Auto highlight text
	m_autoHighlightCheckBox->setChecked(XINXConfig::self()->config().editor.autoHighlight);

	// Color of highlighted text
	m_highlightTextColorComboBox->setColor(XINXConfig::self()->config().editor.highlightWord);

	// Show tabulation and space in the editor
	m_showTabulationCheckBox->setChecked(XINXConfig::self()->config().editor.showTabulationAndSpace);

	// Show current line
	m_showCurrentLineCheckBox->setChecked(XINXConfig::self()->config().editor.highlightCurrentLine);

	// Size of tabulation
	m_sizeOfTabSpinBox->setValue(XINXConfig::self()->config().editor.tabulationSize);

	// Font size
	m_fontSizeSpinBox->setValue(XINXConfig::self()->config().editor.defaultFormat.pointSize());

	// Font name
	m_fontComboBox->setCurrentFont(XINXConfig::self()->config().editor.defaultFormat);

	return true;
}

bool CustomFontImpl::saveSettingsDialog()
{
	// Auto highlight text
	XINXConfig::self()->config().editor.autoHighlight = m_autoHighlightCheckBox->isChecked();

	// Color of highlighted text
	XINXConfig::self()->config().editor.highlightWord = m_highlightTextColorComboBox->color();

	// Show tabulation and space in the editor
	XINXConfig::self()->config().editor.showTabulationAndSpace = m_showTabulationCheckBox->isChecked();

	// Show current line
	XINXConfig::self()->config().editor.highlightCurrentLine = m_showCurrentLineCheckBox->isChecked();

	// Size of tabulation
	XINXConfig::self()->config().editor.tabulationSize = m_sizeOfTabSpinBox->value();

	// Font name
	XINXConfig::self()->config().editor.defaultFormat = m_fontComboBox->currentFont();

	// Font size
	XINXConfig::self()->config().editor.defaultFormat.setPointSize(m_fontSizeSpinBox->value());

	return true;
}

bool CustomFontImpl::cancelSettingsDialog()
{
	return true;
}

bool CustomFontImpl::isSettingsValid()
{
	return true;
}

bool CustomFontImpl::isVisible()
{
	return true;
}


