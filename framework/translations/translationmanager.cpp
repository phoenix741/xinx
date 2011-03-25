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

#include "translationmanager_p.h"
#include <core/xinxconfig.h>

#include <QDir>
#include <QApplication>

/* TranslationManagerPrivate */

QStringList TranslationManagerPrivate::findQmFiles()
{
	QStringList result, filenames;
	foreach(const QString & path, QDir::searchPaths(":translations"))
	{
		QDir dir(path);

		QStringList localFileNames = dir.entryList(QStringList("*.qm"), QDir::Files, QDir::Name);

		foreach(const QString fileName, localFileNames)
		{
			if (! filenames.contains(fileName))
			{
				filenames << fileName;
				result << dir.filePath(fileName);
			}
		}
	}

	return result;
}

TranslationManager::Language TranslationManagerPrivate::languageName(const QString & qmFile)
{
	QTranslator translator;
	translator.load(qmFile);

	TranslationManager::Language lang;

	lang.code  = qmFile.section("_", -1);
	lang.name  = translator.translate("TranslationManager", "C", "Translate this word in the name of your lang (ie: English, Français, ...)");
	lang.image = translator.translate("TranslationManager", ":/images/unknown.png", "Give the image that contains the flag of your lang");

	if (lang.name == "C")
	{
		lang.code = lang.name = lang.image = QString();
	}

	return lang;
}


/* TranslationManager */

TranslationManager::TranslationManager() : d(new TranslationManagerPrivate)
{
	d->_lang = QLocale::system().name();

	QStringList qmFiles = d->findQmFiles();
	foreach(const QString & qmFile, qmFiles)
	{
		Language l = d->languageName(qmFile);
		if (! l.code.isEmpty())
		{
			d->_languages.insert(l.code, l);
		}
	}
}

TranslationManager::~TranslationManager()
{

}

void TranslationManager::setLanguage(const QString& lang)
{
	d->_lang = lang;
}

const QString& TranslationManager::language() const
{
	return d->_lang;
}

void TranslationManager::apply()
{
	foreach(QTranslator * translator, d->_translators)
	{
		qApp->removeTranslator(translator);
		delete translator;
	}
	d->_translators.clear();

	foreach(const QString & file, d->_files)
	{
		if (file.endsWith("_" + d->_lang + ".qm"))
		{
			QTranslator * translator = new QTranslator(qApp);
			translator->load(file);
			qApp->installTranslator(translator);
		}
	}
}

QList<TranslationManager::Language> TranslationManager::languages() const
{
	return d->_languages.values();
}

void startTranslation()
{
	TranslationManager::self()->setLanguage(XINXConfig::self()->config().language);
	TranslationManager::self()->apply();
}
