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

// Qt header
#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QFileDialog>

// Xinx header
#include "core/xinxconfig.h"
#include "core/xinxcore.h"
#include "plugins/xinxpluginsloader.h"
#include "editors/xinxformatscheme.h"
#include "editors/xinxlanguagefactory.h"

/* ToolsNotDefinedException */

ToolsNotDefinedException::ToolsNotDefinedException(const QString & tool) : XinxException(QString("Tool %1 not correctly defined.").arg(tool))
{

}

/* XINXConfig */

XINXConfig::XINXConfig(const XINXConfig & origine) : AppSettings(origine)
{

}

XINXConfig::XINXConfig() : AppSettings()
{

}

XINXConfig::~XINXConfig()
{

}

XINXConfig& XINXConfig::operator=(const XINXConfig& p)
{
	AppSettings::operator=(p);
	return *this;
}

void XINXConfig::load()
{
	AppSettings::load();
	updateFormatsSchemeFromConfig();
}

void XINXConfig::save()
{
	putFormatsSchemeToConfig();
	AppSettings::save();

	emit changed();
}

XinxLanguageFactory * XINXConfig::languageFactory()
{
	if (! m_languages)
	{
		m_languages = new XinxLanguageFactory(new XinxFormatScheme(this), this);
	}
	return m_languages;
}

void XINXConfig::addFormatScheme(const QString & id, XinxFormatScheme * scheme)
{
	if (m_formatScheme.contains(id))
	{
		delete m_formatScheme[ id ];
	}
	scheme->updateFormatsFromConfig();
	m_formatScheme[ id ] = scheme;
}

XinxFormatScheme * XINXConfig::scheme(const QString & highlighter)
{
	if (! m_formatScheme.contains(highlighter))
	{
		XinxFormatScheme * scheme = XinxPluginsLoader::self()->scheme(highlighter, this);
		if (scheme)
		{
			scheme->updateFormatsFromConfig();
			m_formatScheme[ highlighter ] = scheme;
		}
	}
	return m_formatScheme.value(highlighter, qobject_cast<XinxFormatScheme*>(languageFactory()->defaultFormatScheme()));
}

void XINXConfig::updateFormatsSchemeFromConfig()
{
	foreach(XinxFormatScheme * scheme, m_formatScheme.values())
	{
		scheme->updateFormatsFromConfig();
	}
}

void XINXConfig::putFormatsSchemeToConfig()
{
	foreach(XinxFormatScheme * scheme, m_formatScheme.values())
	{
		scheme->putFormatsToConfig();
	}
}

QString XINXConfig::getTools(const QString & tool, bool showDialog, QWidget * parentWindow)
{
	QString toolsPath = config().tools.value(tool);
	if (toolsPath.isEmpty() || (! QFile::exists(toolsPath)))
	{
		if (! showDialog)
			throw ToolsNotDefinedException(toolsPath);

		toolsPath = QFileDialog::getOpenFileName(parentWindow, tr("Select the %1 tool").arg(tool), toolsPath);
		if ((!toolsPath.isEmpty()) && QFile::exists(toolsPath))
			config().tools[ tool ] = toolsPath;
		else
			throw ToolsNotDefinedException(toolsPath);
	}
	return toolsPath;
}

void XINXConfig::addDefaultTool(const QString & tool, const QString & defaultValue)
{
	if (config().tools.value(tool).isEmpty())
	{
		config().tools[ tool ] = defaultValue;
		// Store imediately the hashtable
		AppSettingsSettings settings("Shadoware.Org", "XINX");
		setSettingsHash_QString(&settings, "Tools", config().tools);
	}
}

AppSettings::struct_globals XINXConfig::getDefaultGlobals()
{
	struct_globals value = AppSettings::getDefaultGlobals();

#ifndef Q_WS_WIN
	value.xinxTrace = "/tmp/xinx_trace.log";
#else
	value.xinxTrace = "c:\\xinx_trace.log";
#endif

	return value;
}

AppSettings::struct_editor XINXConfig::getDefaultEditor()
{
	struct_editor value = AppSettings::getDefaultEditor();

	value.defaultFormat.setFamily("Monospace");
	value.defaultFormat.setFixedPitch(true);
	value.defaultFormat.setPointSize(8);

	return value;
}


