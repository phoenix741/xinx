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
#include "customsyntaximpl.h"
#include <core/xinxconfig.h>
#include <editors/xinxformatscheme.h>
#include <editors/xinxlanguagefactory.h>
#include <plugins/xinxpluginsloader.h>

/* CustomSyntaxImpl */

CustomSyntaxImpl::CustomSyntaxImpl(QWidget * parent) : QWidget(parent)
{
	setupUi(this);
}

CustomSyntaxImpl::~CustomSyntaxImpl()
{
	qDeleteAll(m_formatScheme);
}

QPixmap CustomSyntaxImpl::image()
{
	return QPixmap(":/images/preferences-color.png");
}

QString CustomSyntaxImpl::name()
{
	return windowTitle();
}

QWidget * CustomSyntaxImpl::settingsDialog()
{
	return this;
}

bool CustomSyntaxImpl::loadSettingsDialog()
{
	QStringList languages = XINXConfig::self()->languageFactory()->languages();
	languages.removeAll("None");
	languages.sort();

	m_highlighterComboBox->clear();
	foreach(QString lang, languages)
	{
		m_highlighterComboBox->addItem(lang);
		XinxFormatScheme * scheme;
		if (m_formatScheme.value(lang))
			scheme = m_formatScheme.value(lang);
		else
		{
			scheme = new XinxFormatScheme();
			m_formatScheme.insert(lang, scheme);
		}

		*scheme = *(XINXConfig::self()->scheme(lang));
	}
	m_highlighterComboBox->setCurrentIndex(0);
	on_m_highlighterComboBox_activated(m_highlighterComboBox->currentText());

	return true;
}

bool CustomSyntaxImpl::saveSettingsDialog()
{
	QStringList languages = XINXConfig::self()->languageFactory()->languages();
	languages.removeAll("None");

	foreach(QString lang, languages)
	{
		*(XINXConfig::self()->scheme(lang)) = *(m_formatScheme.value(lang));
	}
	XINXConfig::self()->putFormatsSchemeToConfig();

	return true;
}

void CustomSyntaxImpl::on_m_highlighterComboBox_activated(QString text)
{
	m_customScheme->setHiddenFormat(QStringList() << "normal" << "search" << "match" << "braceMatch" << "braceMismatch");
	m_customScheme->setFormatScheme(m_formatScheme.value(text));
	m_customScheme->setLanguageFactory(XINXConfig::self()->languageFactory());
	m_customScheme->setLanguageDefinition(text);
	m_customScheme->setExample(XinxPluginsLoader::self()->exampleOfHighlighter(text));
}

bool CustomSyntaxImpl::cancelSettingsDialog()
{
	return true;
}

bool CustomSyntaxImpl::isSettingsValid()
{
	return true;
}

bool CustomSyntaxImpl::isVisible()
{
	return true;
}


