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
#include "tracticketcreationwizard.h"

// Qt header
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
#if defined(Q_WS_WIN)
	if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA && QSysInfo::WindowsVersion < QSysInfo::WV_NT_based)
		QApplication::setStyle(QStyleFactory::create("Explorer"));
#endif // Q_WS_WIN

	QApplication app(argc, argv);
	app.setOrganizationName("Shadoware.Org");
	app.setOrganizationDomain("Shadoware.Org");
	app.setApplicationName("XINX");


	TracTicketCreationWizard wizard;
	/* 
	Parameters are:
	- Message
	- Version
	*/
	wizard.show();

	return app.exec();
}

