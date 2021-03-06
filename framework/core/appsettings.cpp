// This file is auto-generated file, don't modify it unless you want to lose your code
// Generated by qgenconfig v0.1 (c) Ulrich Van Den Hekke

#include <QLocale>
#include <QDir>
#include <QApplication>

#include "appsettings.h"

/* AppSettingsSettings */

AppSettingsSettings::AppSettingsSettings(const QString & organization, const QString & application) : QSettings(organization, application)
{
}

void AppSettingsSettings::setValue(const QString & key, const QVariant & value, const QVariant & defaultValue)
{
	if (value == defaultValue)
		remove(key);
	else
		QSettings::setValue(key, value);
}

void AppSettingsSettings::setValue(const QString & key, const QVariant & value)
{
	QSettings::setValue(key, value);
}

/* PrivateAppSettings */

class PrivateAppSettings
{
public:
	PrivateAppSettings(AppSettings * parent);

	AppSettingsSettings * m_settings;
	void createSettings();
	void deleteSettings();

	AppSettings::struct_globals m_globals;
private:
	AppSettings * m_parent;
};

PrivateAppSettings::PrivateAppSettings(AppSettings * parent)
{
	m_parent = parent;
}

void PrivateAppSettings::createSettings()
{
	m_settings = new AppSettingsSettings("Shadoware.Org", "XINX");
}

void PrivateAppSettings::deleteSettings()
{
	if (m_settings)
		delete m_settings;
	m_settings = NULL;
}

/* AppSettings */

AppSettings::AppSettings(const AppSettings & origine)
{
	d = new PrivateAppSettings(this);
	d->m_globals = origine.d->m_globals;
}

AppSettings::AppSettings()
{
	d = new PrivateAppSettings(this);
}

AppSettings::~AppSettings()
{
	delete d;
}

AppSettings&AppSettings::operator=(const AppSettings& p)
{
	d->m_globals = p.d->m_globals;
	return *this;
}

AppSettings::struct_globals & AppSettings::config()
{
	return d->m_globals;
}

void AppSettings::setDefault()
{
	d->m_globals = getDefaultGlobals();
}

void AppSettings::save()
{
	d->createSettings();
	setSettingsGlobals(d->m_settings, "", d->m_globals);
	d->deleteSettings();
}

void AppSettings::load()
{
	d->createSettings();
	d->m_globals = getSettingsGlobals(d->m_settings, "", getDefaultGlobals());
	d->deleteSettings();
}

AppSettings::struct_docks AppSettings::getDefaultDocks()
{
	struct_docks value;

	value.showShortcut = true;

	return value;
}

AppSettings::struct_docks AppSettings::getSettingsDocks(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_docks & defaultValue)
{
	struct_docks value;
	settings->beginGroup(path);

	value.showShortcut = settings->value("Show Shortcut", defaultValue.showShortcut).toBool();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsDocks(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_docks & value)
{
	struct_docks defaultValue = getDefaultDocks();
	settings->beginGroup(path);

	settings->setValue("Show Shortcut", value.showShortcut, defaultValue.showShortcut);

	settings->endGroup();
}

AppSettings::struct_snipets AppSettings::getDefaultSnipets()
{
	struct_snipets value;

	value.alwaysShowDialog = false;

	return value;
}

AppSettings::struct_snipets AppSettings::getSettingsSnipets(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_snipets & defaultValue)
{
	struct_snipets value;
	settings->beginGroup(path);

	value.alwaysShowDialog = settings->value("Always Show Dialog", defaultValue.alwaysShowDialog).toBool();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsSnipets(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_snipets & value)
{
	struct_snipets defaultValue = getDefaultSnipets();
	settings->beginGroup(path);

	settings->setValue("Always Show Dialog", value.alwaysShowDialog, defaultValue.alwaysShowDialog);

	settings->endGroup();
}

AppSettings::struct_qformat AppSettings::getDefaultQformat()
{
	struct_qformat value;

	value.italic = false;
	value.bold = false;
	value.overline = false;
	value.strikout = false;
	value.underline = false;
	value.waveunderline = false;

	return value;
}

AppSettings::struct_qformat AppSettings::getSettingsQformat(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_qformat & defaultValue)
{
	struct_qformat value;
	settings->beginGroup(path);

	value.italic = settings->value("italic", defaultValue.italic).toBool();
	value.bold = settings->value("bold", defaultValue.bold).toBool();
	value.overline = settings->value("overline", defaultValue.overline).toBool();
	value.strikout = settings->value("strikout", defaultValue.strikout).toBool();
	value.underline = settings->value("underline", defaultValue.underline).toBool();
	value.waveunderline = settings->value("waveunderline", defaultValue.waveunderline).toBool();
	value.foreground = settings->value("foreground", defaultValue.foreground).value<QColor>();
	value.background = settings->value("background", defaultValue.background).value<QColor>();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsQformat(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_qformat & value)
{
	struct_qformat defaultValue = getDefaultQformat();
	settings->beginGroup(path);

	settings->setValue("italic", value.italic, defaultValue.italic);
	settings->setValue("bold", value.bold, defaultValue.bold);
	settings->setValue("overline", value.overline, defaultValue.overline);
	settings->setValue("strikout", value.strikout, defaultValue.strikout);
	settings->setValue("underline", value.underline, defaultValue.underline);
	settings->setValue("waveunderline", value.waveunderline, defaultValue.waveunderline);
	settings->setValue("foreground", value.foreground, defaultValue.foreground);
	settings->setValue("background", value.background, defaultValue.background);

	settings->endGroup();
}

AppSettings::struct_editor AppSettings::getDefaultEditor()
{
	struct_editor value;

	value.defaultTextCodec = "ISO-8859-1";
	value.autoindentOnSaving = "false";
	value.popupWhenFileModified = true;
	value.createBackupFile = true;
	value.tabulationSize = 4;
	value.showTabulationAndSpace = false;
	value.highlightCurrentLine = true;
	value.highlightWord = Qt::yellow;
	value.autoHighlight = false;
	value.hideCloseTab = false;
	value.closeButtonOnEachTab = false;
	value.automaticModelRefreshTimeout = 1000;
	value.wrapLine = false;
	value.closeErrorDockAutomatically = true;

	return value;
}

AppSettings::struct_editor AppSettings::getSettingsEditor(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_editor & defaultValue)
{
	struct_editor value;
	settings->beginGroup(path);

	value.defaultTextCodec = settings->value("Default Text Codec", defaultValue.defaultTextCodec).toString();
	value.autoindentOnSaving = settings->value("Autoindent On Saving", defaultValue.autoindentOnSaving).toString();
	value.popupWhenFileModified = settings->value("Popup When File Modified", defaultValue.popupWhenFileModified).toBool();
	value.createBackupFile = settings->value("Create Backup File", defaultValue.createBackupFile).toBool();
	value.tabulationSize = settings->value("Tabulation Size", defaultValue.tabulationSize).toInt();
	value.showTabulationAndSpace = settings->value("Show Tabulation and space", defaultValue.showTabulationAndSpace).toBool();
	value.highlightCurrentLine = settings->value("Highlight Current Line", defaultValue.highlightCurrentLine).toBool();
	value.defaultFormat = settings->value("Default format", defaultValue.defaultFormat).value<QFont>();
	value.highlightWord = settings->value("Highlight word", defaultValue.highlightWord).value<QColor>();
	value.autoHighlight = settings->value("Auto Highlight", defaultValue.autoHighlight).toBool();
	value.hideCloseTab = settings->value("Hide Close Tab", defaultValue.hideCloseTab).toBool();
	value.closeButtonOnEachTab = settings->value("Close Button on each Tab", defaultValue.closeButtonOnEachTab).toBool();
	value.automaticModelRefreshTimeout = settings->value("Automatic Model Refresh Timeout", defaultValue.automaticModelRefreshTimeout).toInt();
	value.wrapLine = settings->value("Wrap Line", defaultValue.wrapLine).toBool();
	value.closeErrorDockAutomatically = settings->value("Close Error Dock Automatically", defaultValue.closeErrorDockAutomatically).toBool();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsEditor(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_editor & value)
{
	struct_editor defaultValue = getDefaultEditor();
	settings->beginGroup(path);

	settings->setValue("Default Text Codec", value.defaultTextCodec, defaultValue.defaultTextCodec);
	settings->setValue("Autoindent On Saving", value.autoindentOnSaving, defaultValue.autoindentOnSaving);
	settings->setValue("Popup When File Modified", value.popupWhenFileModified, defaultValue.popupWhenFileModified);
	settings->setValue("Create Backup File", value.createBackupFile, defaultValue.createBackupFile);
	settings->setValue("Tabulation Size", value.tabulationSize, defaultValue.tabulationSize);
	settings->setValue("Show Tabulation and space", value.showTabulationAndSpace, defaultValue.showTabulationAndSpace);
	settings->setValue("Highlight Current Line", value.highlightCurrentLine, defaultValue.highlightCurrentLine);
	settings->setValue("Default format", value.defaultFormat, defaultValue.defaultFormat);
	settings->setValue("Highlight word", value.highlightWord, defaultValue.highlightWord);
	settings->setValue("Auto Highlight", value.autoHighlight, defaultValue.autoHighlight);
	settings->setValue("Hide Close Tab", value.hideCloseTab, defaultValue.hideCloseTab);
	settings->setValue("Close Button on each Tab", value.closeButtonOnEachTab, defaultValue.closeButtonOnEachTab);
	settings->setValue("Automatic Model Refresh Timeout", value.automaticModelRefreshTimeout, defaultValue.automaticModelRefreshTimeout);
	settings->setValue("Wrap Line", value.wrapLine, defaultValue.wrapLine);
	settings->setValue("Close Error Dock Automatically", value.closeErrorDockAutomatically, defaultValue.closeErrorDockAutomatically);

	settings->endGroup();
}

AppSettings::struct_rcs AppSettings::getDefaultRcs()
{
	struct_rcs value;

	value.createChangelog = false;
	value.autoAddFileToVersionningSystem = true;

	return value;
}

AppSettings::struct_rcs AppSettings::getSettingsRcs(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_rcs & defaultValue)
{
	struct_rcs value;
	settings->beginGroup(path);

	value.createChangelog = settings->value("Create ChangeLog", defaultValue.createChangelog).toBool();
	value.autoAddFileToVersionningSystem = settings->value("Auto add file to versionning system", defaultValue.autoAddFileToVersionningSystem).toBool();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsRcs(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_rcs & value)
{
	struct_rcs defaultValue = getDefaultRcs();
	settings->beginGroup(path);

	settings->setValue("Create ChangeLog", value.createChangelog, defaultValue.createChangelog);
	settings->setValue("Auto add file to versionning system", value.autoAddFileToVersionningSystem, defaultValue.autoAddFileToVersionningSystem);

	settings->endGroup();
}

AppSettings::struct_project AppSettings::getDefaultProject()
{
	struct_project value;

	value.openTheLastSessionAtStart = false;
	value.singleProjectMode = false;
	value.defaultPath = QDir(qApp->applicationDirPath()).absoluteFilePath("project");
	value.closeVersionManagementLog = true;
	value.automaticProjectDirectoryRefreshTimeout = 500;

	return value;
}

AppSettings::struct_project AppSettings::getSettingsProject(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_project & defaultValue)
{
	struct_project value;
	settings->beginGroup(path);

	value.lastOpenedSession = settings->value("Last opened session", defaultValue.lastOpenedSession).toString();
	value.openTheLastSessionAtStart = settings->value("Open the last session at start", defaultValue.openTheLastSessionAtStart).toBool();
	value.singleProjectMode = settings->value("Single Project Mode", defaultValue.singleProjectMode).toBool();
	value.defaultPath = settings->value("Default Path", defaultValue.defaultPath).toString();
	value.recentProjectFiles = settings->value("Recent Project Files", defaultValue.recentProjectFiles).toStringList();
	value.closeVersionManagementLog = settings->value("Close Version Management Log", defaultValue.closeVersionManagementLog).toBool();
	value.automaticProjectDirectoryRefreshTimeout = settings->value("Automatic Project Directory Refresh Timeout", defaultValue.automaticProjectDirectoryRefreshTimeout).toInt();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsProject(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_project & value)
{
	struct_project defaultValue = getDefaultProject();
	settings->beginGroup(path);

	settings->setValue("Last opened session", value.lastOpenedSession, defaultValue.lastOpenedSession);
	settings->setValue("Open the last session at start", value.openTheLastSessionAtStart, defaultValue.openTheLastSessionAtStart);
	settings->setValue("Single Project Mode", value.singleProjectMode, defaultValue.singleProjectMode);
	settings->setValue("Default Path", value.defaultPath, defaultValue.defaultPath);
	settings->setValue("Recent Project Files", value.recentProjectFiles, defaultValue.recentProjectFiles);
	settings->setValue("Close Version Management Log", value.closeVersionManagementLog, defaultValue.closeVersionManagementLog);
	settings->setValue("Automatic Project Directory Refresh Timeout", value.automaticProjectDirectoryRefreshTimeout, defaultValue.automaticProjectDirectoryRefreshTimeout);

	settings->endGroup();
}

AppSettings::struct_globals AppSettings::getDefaultGlobals()
{
	struct_globals value;

	value.language = QLocale::system().name();
	value.position = QPoint(200,200);
	value.size = QSize(400,400);
	value.maximized = false;
	value.project = getDefaultProject();
	value.rcs = getDefaultRcs();
	value.editor = getDefaultEditor();
	value.snipets = getDefaultSnipets();
	value.formats = getDefaultHash_struct_qformat();

	return value;
}

AppSettings::struct_globals AppSettings::getSettingsGlobals(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_globals & defaultValue)
{
	struct_globals value;
	settings->beginGroup(path);

	value.language = settings->value("Language", defaultValue.language).toString();
	value.position = settings->value("Position", defaultValue.position).toPoint();
	value.size = settings->value("Size", defaultValue.size).toSize();
	value.maximized = settings->value("Maximized", defaultValue.maximized).toBool();
	value.state = settings->value("State", defaultValue.state).toByteArray();
	value.xinxTrace = settings->value("XINX Trace", defaultValue.xinxTrace).toString();
	value.style = settings->value("Style", defaultValue.style).toString();
	value.plugins = getSettingsHash_bool(settings, "Plugins", defaultValue.plugins);
	value.dock =  getSettingsDocks(settings, "Dock", defaultValue.dock);
	value.project = getSettingsProject(settings, "Project", defaultValue.project);
	value.rcs = getSettingsRcs(settings, "RCS", defaultValue.rcs);
	value.editor = getSettingsEditor(settings, "Editor", defaultValue.editor);
	value.snipets = getSettingsSnipets(settings, "Snipets", defaultValue.snipets);
	value.tools = getSettingsHash_QString(settings, "Tools", defaultValue.tools);
	value.formats = getSettingsHash_struct_qformat(settings, "Formats", defaultValue.formats);
	value.version = settings->value("Version", defaultValue.version).toString();
	value.lastUpdate = settings->value("Last Update", defaultValue.lastUpdate).toString();

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsGlobals(AppSettingsSettings * settings, const QString & path, const AppSettings::struct_globals & value)
{
	struct_globals defaultValue = getDefaultGlobals();
	settings->beginGroup(path);

	settings->setValue("Language", value.language, defaultValue.language);
	settings->setValue("Position", value.position, defaultValue.position);
	settings->setValue("Size", value.size, defaultValue.size);
	settings->setValue("Maximized", value.maximized, defaultValue.maximized);
	settings->setValue("State", value.state, defaultValue.state);
	settings->setValue("XINX Trace", value.xinxTrace, defaultValue.xinxTrace);
	settings->setValue("Style", value.style, defaultValue.style);
	setSettingsHash_bool(settings, "Plugins", value.plugins);
	setSettingsDocks(settings, "Dock", value.dock);
	setSettingsProject(settings, "Project", value.project);
	setSettingsRcs(settings, "RCS", value.rcs);
	setSettingsEditor(settings, "Editor", value.editor);
	setSettingsSnipets(settings, "Snipets", value.snipets);
	setSettingsHash_QString(settings, "Tools", value.tools);
	setSettingsHash_struct_qformat(settings, "Formats", value.formats);
	settings->setValue("Version", value.version, defaultValue.version);
	settings->setValue("Last Update", value.lastUpdate, defaultValue.lastUpdate);

	settings->endGroup();
}

QHash<QString,bool> AppSettings::getSettingsHash_bool(AppSettingsSettings * settings, const QString & path, const QHash<QString,bool> & defaultValue)
{
	QHash<QString,bool> value;
	settings->beginGroup(path);

	QStringList keys = settings->childKeys() + settings->childGroups();
	foreach(const QString & key, keys)
	{
		bool def = defaultValue[ key ];
		value[ key ] = settings->value(key, def).toBool();
	}
	foreach(const QString & defaultValueKey, defaultValue.keys())
	{
		if (! value.contains(defaultValueKey))
		{
			value[ defaultValueKey ] = defaultValue[ defaultValueKey ];
		}
	}

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsHash_bool(AppSettingsSettings * settings, const QString & path, const QHash<QString,bool> & value)
{
	settings->beginGroup(path);

	foreach(const QString & key, value.keys())
	{
		settings->setValue(key, value[ key ], true);
	}

	settings->endGroup();
}

QHash<QString,QString> AppSettings::getSettingsHash_QString(AppSettingsSettings * settings, const QString & path, const QHash<QString,QString> & defaultValue)
{
	QHash<QString,QString> value;
	settings->beginGroup(path);

	QStringList keys = settings->childKeys() + settings->childGroups();
	foreach(const QString & key, keys)
	{
		QString def = defaultValue[ key ];
		value[ key ] = settings->value(key, def).toString();
	}
	foreach(const QString & defaultValueKey, defaultValue.keys())
	{
		if (! value.contains(defaultValueKey))
		{
			value[ defaultValueKey ] = defaultValue[ defaultValueKey ];
		}
	}

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsHash_QString(AppSettingsSettings * settings, const QString & path, const QHash<QString,QString> & value)
{
	settings->beginGroup(path);

	foreach(const QString & key, value.keys())
	{
		settings->setValue(key, value[ key ], QString());
	}

	settings->endGroup();
}

QHash<QString,AppSettings::struct_qformat> AppSettings::getDefaultHash_struct_qformat()
{
	QHash<QString,struct_qformat> value;


	return value;
}

QHash<QString,AppSettings::struct_qformat> AppSettings::getSettingsHash_struct_qformat(AppSettingsSettings * settings, const QString & path, const QHash<QString,AppSettings::struct_qformat> & defaultValue)
{
	QHash<QString,struct_qformat> value;
	settings->beginGroup(path);

	QStringList keys = settings->childKeys() + settings->childGroups();
	foreach(const QString & key, keys)
	{
		struct_qformat def;
		if (defaultValue.contains(key))
			def = defaultValue[ key ];
		else
			def = getDefaultQformat();
		value[ key ] = getSettingsQformat(settings, key, def);
	}
	foreach(const QString & defaultValueKey, defaultValue.keys())
	{
		if (! value.contains(defaultValueKey))
		{
			value[ defaultValueKey ] = defaultValue[ defaultValueKey ];
		}
	}

	settings->endGroup();
	return value;
}

void AppSettings::setSettingsHash_struct_qformat(AppSettingsSettings * settings, const QString & path, const QHash<QString,AppSettings::struct_qformat> & value)
{
	QHash<QString,struct_qformat> defaultValue = getDefaultHash_struct_qformat();
	settings->beginGroup(path);

	foreach(const QString & key, value.keys())
	{
		setSettingsQformat(settings, key, value[ key ]);
	}

	settings->endGroup();
}

