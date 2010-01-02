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

#ifndef GENERIXPROJECT_H
#define GENERIXPROJECT_H
#pragma once

// Xinx header
#include <project/xinxproject.h>

/*!
 * Cet objet s'utilise avec un static_cast<> sur l'objet XinxProject.
 * Il ne doit donc contenir aucun membre, et aucune m�thode virtuelle.
 * Il ne contient par ailleurs aucun constructeur car il n'y a pas lieu
 * de construire un objet avec cette classe.
 *
 * Si le projet est un projet "Generix", alors il peut g�rer la d�rivation,
 * ou non.
 * Dans tous les cas il faut d�finir l'emplacement du module (normallement le
 * m�me que le dossier projet. Cela permet de d�terminer la version et donc le
 * resolver � utiliser.
 *
 * En cas de d�rivation, il faut d�finir le dossier de destination ainsi que le
 * dossier source. Les fichiers ne se trouvant pas dans le dossier de destination
 * (ou un sous-dossier) sont alors consid�r� comme standard. --Cela implique donc
 * que le dossier source ne peux pas �tre un sous-dossier du dossier de destination
 * --
 * Lors de la sauvegarde d'un fichier standard, une d�rivation est propos�. L'application
 * propose alors de sauvegarder le fichier dans le dossier de d�rivation (mais en utilisant la
 * m�me structure que le dossier source).
 * Si necessaire le fichier est pr�fix� (prefixe par d�fant non vide), et le fichier source et
 * copi� �galement dans le dossier de destination.
 *
 * En cas de sauvegarde d'un nouveau fichier, le dossier de d�rivation sera alors propos�. Il
 * n'y a plus de notion de sous-dossier de destination par type d'extention (Necessitera peut-�tre
 * une confirmation sur la mailing-liste utilisateur).
 */
class GenerixProject : public XinxProject {
public:
	bool isGenerixActivated() const;

	/* Option du Web-module */

	void setWebModuleLocation( const QString & value );
	QString webModuleLocation() const;

	/* Options de d�rivation */

	class DerivationPath {
	public:
		DerivationPath() : derivation(true), visible(true) {}

		bool derivation, visible;
		QString name, path;
	};

	void setDerivationsPath( const QList<DerivationPath> & value );
	QList<DerivationPath> derivationsPath() const;

	/* Options */

	bool createMissingDirectory() const;
	void setCreateMissingDirectory( bool value );

	QStringList prefixes() const;
	void setPrefixes( const QStringList & value );

	QString defaultPrefix() const;
	void setDefaultPrefix( const QString & value );

	bool copySourceFileInDerivationPath() const;
	void setCopySourceFileInDerivationPath( bool value );


};

#endif // GENERIXPROJECT_H
