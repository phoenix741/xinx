/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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
#include "customcontentviewimpl.h"
#include <contentview2/contentview2manager.h>

// Qt header
#include <QFileInfo>

CustomContentViewImpl::CustomContentViewImpl(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
	updateInformations();
}

CustomContentViewImpl::~CustomContentViewImpl()
{

}

QPixmap CustomContentViewImpl::image()
{
	return QPixmap(":/images/preferences-contentview.png");
}

QString CustomContentViewImpl::name()
{
	return windowTitle();
}

void CustomContentViewImpl::updateInformations()
{
	int cntProject, cntFile, cntNode, fileSize;

	QSqlQuery countQuery(ContentView2::Manager::self()->database());
	countQuery.prepare("SELECT count(1) FROM cv_project");
	countQuery.exec();
	countQuery.first();
	cntProject = countQuery.value(0).toInt();

	countQuery.prepare("SELECT count(1) FROM cv_file");
	countQuery.exec();
	countQuery.first();
	cntFile = countQuery.value(0).toInt();

	countQuery.prepare("SELECT count(1) FROM cv_node");
	countQuery.exec();
	countQuery.first();
	cntNode = countQuery.value(0).toInt();

	fileSize = QFileInfo(ContentView2::Manager::self()->fileName()).size();


	QString summary = 			"Summary\n"
								"=======\n"
								"\n"
								"Statistique\n"
								"-----------\n"
								"Number of project : %1\n"
								"Number of files : %2\n"
								"Number of nodes : %3\n"
								"\n"
								"Size of File   :\n"
								"----------------\n"
								"contentview.db : %4kb\n";
	summary = summary.arg(cntProject).arg(cntFile).arg(cntNode).arg((double)fileSize / 1024, 0, 'f', 2);

	m_sumaryEdit->setPlainText(summary);
}


QWidget * CustomContentViewImpl::settingsDialog()
{
	return this;
}

bool CustomContentViewImpl::loadSettingsDialog()
{
	return true;
}

bool CustomContentViewImpl::saveSettingsDialog()
{
	return true;
}

bool CustomContentViewImpl::cancelSettingsDialog()
{
	return true;
}

bool CustomContentViewImpl::isSettingsValid()
{
	return true;
}

bool CustomContentViewImpl::isVisible()
{
	return true;
}

void CustomContentViewImpl::on_m_compactBaseBtn_clicked()
{
	ContentView2::Manager::self()->compactDatabase();
	updateInformations();
}

void CustomContentViewImpl::on_m_reindexBaseBtn_clicked()
{
	ContentView2::Manager::self()->rebuildIndex();
	updateInformations();
}

