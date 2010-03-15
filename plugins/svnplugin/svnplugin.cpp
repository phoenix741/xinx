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

// Xinx header
#include "svnplugin.h"
#include "rcs_svn.h"

// Qt header
#include <QString>
#include <QVariant>
#include <QTranslator>
#include <QApplication>

/* SVNPlugin */

SVNPlugin::SVNPlugin()
{
	Q_INIT_RESOURCE(svnplugin);
}

bool SVNPlugin::initializePlugin(const QString & lang)
{
	QTranslator * tranlator = new QTranslator(this);
	tranlator->load(QString(":/translations/svnplugin_%1").arg(lang));
	qApp->installTranslator(tranlator);

	return true;
}

QVariant SVNPlugin::getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr)
{
	switch (attr)
	{
	case PLG_NAME:
		return tr("Wrapper for SubVersion");
	case PLG_DESCRIPTION:
		return tr("Enable using SubVersion throw a wrapper of the svn commande.");
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
	case PLG_ICON:
		return QPixmap(":/images/repos_svn.png");
	case PLG_EMAIL:
		return "ulrich.vdh@shadoware.org";
	case PLG_WEBSITE:
		return "http://www.shadoware.org";
	case PLG_VERSION:
		return "0.1";
	case PLG_LICENCE:
		return "GPL v2.0 or later";
	}
	return QVariant();
}

QList< QPair<QString,QString> > SVNPlugin::pluginTools()
{
	QList< QPair<QString,QString> > tools;
#ifdef Q_WS_WIN
	tools.append(qMakePair(QString("svn"), QString("%1/Subversion/bin/svn.exe").arg("C:/Program Files")));
#else
	tools.append(qMakePair(QString("svn"), QString("/usr/bin/svn")));
#endif // Q_WS_WIN
	return tools;
}

QStringList SVNPlugin::rcs()
{
	return QStringList() << "svn";
}

QString SVNPlugin::descriptionOfRCS(const QString & rcs)
{
	if (rcs.toLower() == "svn")
		return tr("SVN - Subversion");
	return QString();
}

RCS * SVNPlugin::createRCS(const QString & rcs, const QString & basePath)
{
	if (rcs.toLower() == "svn")
	{
		return new RCS_SVN(basePath);
	}
	return NULL;
}

Q_EXPORT_PLUGIN2(svnplugin, SVNPlugin)
