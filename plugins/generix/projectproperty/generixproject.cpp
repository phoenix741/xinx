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
#include "generixproject.h"
#include <project/xinxproject.h>

/* GenerixProject */

bool GenerixProject::isGenerixActivated() const
{
	return activatedPlugin().contains("GenerixPlugin");
}

void GenerixProject::setWebModuleLocation(const QString & value)
{
	writeProperty("generix.webModule", value);
}

QString GenerixProject::webModuleLocation() const
{
	return readProperty("generix.webModule").toString();
}

void GenerixProject::setDerivationsPath(const QList<GenerixProject::DerivationPath> & value)
{
	writeProperty("generix.derivationPath.count", value.count());
	int i = 1;
	foreach(const DerivationPath & dp, value)
	{
		writeProperty(QString("generix.derivationPath.%1.name").arg(i), dp.name);
		writeProperty(QString("generix.derivationPath.%1.path").arg(i), dp.path);
		writeProperty(QString("generix.derivationPath.%1.derivation").arg(i), dp.derivation);
		writeProperty(QString("generix.derivationPath.%1.visible").arg(i), dp.visible);

		i++;
	}
}

QList<GenerixProject::DerivationPath> GenerixProject::derivationsPath() const
{
	QList<GenerixProject::DerivationPath> derivationPaths;

	int count = readProperty("generix.derivationPath.count").toInt();
	for (int i = 1; i <= count ; i++)
	{
		DerivationPath dp;
		dp.name       = readProperty(QString("generix.derivationPath.%1.name").arg(i)).toString();
		dp.path       = readProperty(QString("generix.derivationPath.%1.path").arg(i)).toString();
		dp.derivation = readProperty(QString("generix.derivationPath.%1.derivation").arg(i)).toBool();
		dp.visible    = readProperty(QString("generix.derivationPath.%1.visible").arg(i)).toBool();

		derivationPaths << dp;
	}
	return derivationPaths;
}

bool GenerixProject::createMissingDirectory() const
{
	return readProperty("generix.createMissingDirectory").toBool();
}

void GenerixProject::setCreateMissingDirectory(bool value)
{
	writeProperty("generix.createMissingDirectory", value);
}

QStringList GenerixProject::prefixes() const
{
	return readProperty("generix.prefixes").toStringList();
}

void GenerixProject::setPrefixes(const QStringList & value)
{
	writeProperty("generix.prefixes", value);
}

QString GenerixProject::defaultPrefix() const
{
	return readProperty("generix.defaultPrefix").toString();
}

void GenerixProject::setDefaultPrefix(const QString & value)
{
	writeProperty("generix.defaultPrefix", value);
}

bool GenerixProject::copySourceFileInDerivationPath() const
{
	return readProperty("generix.copySourceFileInDerivationPath").toBool();
}

void GenerixProject::setCopySourceFileInDerivationPath(bool value)
{
	writeProperty("generix.copySourceFileInDerivationPath", value);
}


