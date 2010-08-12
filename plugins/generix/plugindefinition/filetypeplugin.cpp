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
#include "filetypeplugin.h"

/* MaquetteFileType */

QString MaquetteFileType::description()
{
	return tr("Generix Maquette File");
}

QString MaquetteFileType::match()
{
	return "*.std_fra *.eta *.maq *.txt *.rmf_fra *.pcl_fra *.xtf_fra";
}

QString MaquetteFileType::icon()
{
	return ":/generix/images/typemaq.png";
}

AbstractEditor * MaquetteFileType::createEditor(const QString & filename)
{
	MaquetteFileEditor * editor = new MaquetteFileEditor(this);

	if (! filename.isEmpty())
		editor->loadFromFile(filename);

	return editor;
}

QString MaquetteFileType::highlighterId() const
{
	return "Maquette";
}

XinxFormatScheme * MaquetteFileType::createFormatScheme(XINXConfig * config) const
{
	return new MaquetteFormatScheme(config);
}

QString MaquetteFileType::createLanguageDescription() const
{
	QString result;
	QFile description(":/generix/qcodeedit/lan/maquette.qnfa");
	if (description.open(QIODevice::ReadOnly))
	{
		QTextStream textDescription(&description);
		result = textDescription.readAll();
	}
	return result;
}

QString MaquetteFileType::fileExample() const
{
	return
		"%CR Auteur: XINX\n"
		"\n"
		"P   LG_VARIABLE\n"
		"\n"
		"%CR Codsoc\n"
		"V   ~001=\"zzzz\",0\n"
		"VEE ~001=@001\n"
		"\n"
		"V10 ~311=\"N\"\n"
		"V10 ~999=ISQL(select '~312='||substr(instr(evt.Codzn3,'U'),1,2) from evt, evp where evt.codsoc=~001 and evt.achvte='~002' and evt.typeve='~003' and evt.numeve=~004 and evp.codpro=~057 and evp.codsoc=evt.codsoc and evp.achvte=evt.achvte and evp.typeve=evt.typeve and evp.numeve=evt.numeve and evp.numpos=evt.numpos and rownum=1 order by evp.numpos ;)\n"
		"T10 si (~312<>0)\n"
		"V10 ~311=\"O\"\n"
		"T10 fsi\n"
		"\n"
		"T10 si (~311=\"O\")\n"
		"%10Bloc 10\n"
		"%10Bloc 10\n"
		"T10 fsi\n";
}

QString MaquetteFileType::parserType()
{
	return 0;
}

QString MaquetteFileType::defaultFileName()
{
	return tr("noname") + ".maq";
}

