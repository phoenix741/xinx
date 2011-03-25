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

#ifndef TRANSLATIONMANAGER_P_H
#define TRANSLATIONMANAGER_P_H

#include "translationmanager.h"
#include <QStringList>
#include <QTranslator>


class TranslationManagerPrivate
{
public:
	QHash<QString, TranslationManager::Language> _languages;
	QString _lang;
	QStringList _files;
	QList<QTranslator*> _translators;

	QStringList findQmFiles();
	TranslationManager::Language languageName(const QString & qmFile);
};


#endif // TRANSLATIONMANAGER_P_H
