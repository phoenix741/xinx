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
#include "dictionarywidget.h"
#include "dictionarymodel.h"

/* DictionaryWidget */

DictionaryWidget::DictionaryWidget(QWidget * parent) : QTreeView(parent), _model(new DictionaryModel(this))
{
	connect(_model.data(), SIGNAL(changed()), this, SIGNAL(changed()));
	setSortingEnabled(true);
	_model->loadDictionaries(QString());
	setModel(_model.data());
}

DictionaryWidget::~DictionaryWidget()
{

}

void DictionaryWidget::setPrefix(const QString& prefix)
{
	if (_prefix != prefix)
	{
		_prefix = prefix;
		_model->loadDictionaries(prefix);
	}
}

const QString& DictionaryWidget::prefix()
{
	return _prefix;
}

