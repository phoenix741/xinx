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
#include "dictionarydockwidgetimpl.h"
#include <configuration/configurationmanager.h>
#include "projectproperty/generixproject.h"
#include "dictionarymodel.h"
#include "addmodifylabeldialog.h"

/* Static */

bool dictionaryLessThan(const QString & dico1, const QString & dico2)
{
	const QString filename1 = QFileInfo(dico1).fileName();
	const QString filename2 = QFileInfo(dico2).fileName();

	if (filename1.contains("_spe") && !filename2.contains("_spe"))
	{
		return true;
	}
	if (filename2.contains("_spe") && !filename1.contains("_spe"))
	{
		return false;
	}

	if (filename1.contains("_dev") && !filename2.contains("_dev"))
	{
		return true;
	}
	if (filename2.contains("_dev") && !filename1.contains("_dev"))
	{
		return false;
	}

	if (filename1.contains("_patch") && !filename2.contains("_patch"))
	{
		return true;
	}
	if (filename2.contains("_patch") && !filename1.contains("_patch"))
	{
		return false;
	}

	return dico1 < dico2;
}

/* DictionaryDockWidgetImpl */

DictionaryDockWidgetImpl::DictionaryDockWidgetImpl(QWidget * parent) : XinxDockWidget(parent), _model(new DictionaryModel(this)), m_timerId(0)
{
	setupUi(this);
	setWindowTitle(tr("Dictionary"));
	setWindowIcon(QIcon(":/generix/images/dictionary16.png"));

	setFocusProxy(m_filterLine);

	connect(_model.data(), SIGNAL(changed()), this, SIGNAL(changed()));
	connect(_model.data(), SIGNAL(changed()), this, SLOT(dictionariesLoaded()));

	_model->loadDictionaries(QString());
	m_dictionaryTreeWidget->setModel(_model.data());
}

DictionaryDockWidgetImpl::~DictionaryDockWidgetImpl()
{

}

void DictionaryDockWidgetImpl::setPrefix(const QString& prefix)
{
	if (_prefix != prefix)
	{
		_prefix = prefix;
		_model->loadDictionaries(prefix);
	}
}

const QString& DictionaryDockWidgetImpl::prefix()
{
	return _prefix;
}

void DictionaryDockWidgetImpl::dictionariesLoaded()
{
	/* Number of labels */
	if (_prefix.isEmpty())
	{
		m_informationLbl->setText(tr("No label(s) loaded."));
	}
	else
	{
		m_informationLbl->setText(tr("%n label(s) loaded.", "", _model->rowCount()));
	}

	/* Dictionary list */
	XinxProject::ProjectPtr project = _model->getProject();

	QStringList dictionaries = _model->dictionaries();
	qSort(dictionaries.begin(), dictionaries.end(), dictionaryLessThan);

	for(int i = m_dictionaryList->count(); i >= 0; i--)
	{
		if (! dictionaries.contains(m_dictionaryList->itemData(i).toString()))
		{
			m_dictionaryList->removeItem(i);
		}
	}

	for(int i = 0; i < dictionaries.count(); i++)
	{
		const QString & dictionary = dictionaries.at(i);
		const QString filename = project.isNull() ? QFileInfo(dictionary).fileName() : QDir(project->projectPath()).relativeFilePath(dictionary);

		if (m_dictionaryList->findData(dictionary) == -1)
		{
			m_dictionaryList->insertItem(i, filename, dictionary);
		}
	}

}

void DictionaryDockWidgetImpl::on_m_addToDico_clicked()
{
	AddModifyLabelDialog dlg;
	if (dlg.exec())
	{
		_model->changeLabel(m_dictionaryList->itemData(m_dictionaryList->currentIndex()).toString(), dlg.getCode(), dlg.getLang(), dlg.getContext(), dlg.getDescription());
	}
}

void DictionaryDockWidgetImpl::on_m_filterLine_textChanged(QString filter)
{
	Q_UNUSED(filter);
	if (m_timerId) killTimer(m_timerId);
	m_timerId = startTimer(500);
}

void DictionaryDockWidgetImpl::timerEvent(QTimerEvent * event)
{
	if (m_timerId == event->timerId())
	{
		killTimer(m_timerId);
		m_timerId = 0;
		setPrefix(m_filterLine->text());
	}
}

void DictionaryDockWidgetImpl::on_m_dictionaryTreeWidget_doubleClicked(const QModelIndex &index)
{
	AddModifyLabelDialog dlg;
	dlg.setCode(index.data(DictionaryModel::ITEM_CODE).toString());
	dlg.setLang(index.data(DictionaryModel::ITEM_LANG).toString());
	dlg.setContext(index.data(DictionaryModel::ITEM_CTX).toString());
	dlg.setDescription(index.data().toString());
	if ((!dlg.getCode().isEmpty()) && dlg.exec())
	{
		_model->changeLabel(index.data(DictionaryModel::ITEM_DICO).toString(), dlg.getCode(), dlg.getLang(), dlg.getContext(), dlg.getDescription());
	}
}
