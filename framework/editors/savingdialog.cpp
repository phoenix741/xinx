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
#include "savingdialog.h"
#include <editors/abstracteditor.h>
#include <core/xinxconfig.h>

/* EditorItem */

class EditorItem : public QListWidgetItem
{
public:
	EditorItem(AbstractEditor * editor, QListWidget * parent);

	AbstractEditor * m_editor;
};

EditorItem::EditorItem(AbstractEditor * editor, QListWidget * parent) : QListWidgetItem(editor->getLongTitle(), parent), m_editor(editor)
{

}

/* SavingDialog */

SavingDialog::SavingDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);

	if(XINXConfig::self()->config().editor.autoindentOnSaving == "closing")
		m_labelMessage2->setText(tr("Pretty Print on closing is activated. This option change file when you closing it."));

	m_selectAll   = new QPushButton(tr("Select All"));
	m_deselectAll = new QPushButton(tr("Deselect All"));

	m_buttonBox->addButton(m_selectAll, QDialogButtonBox::ActionRole);
	m_buttonBox->addButton(m_deselectAll, QDialogButtonBox::ActionRole);

	connect(m_buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxClicked(QAbstractButton*)));
}

void SavingDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);

	switch (e->type())
	{
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void SavingDialog::buttonBoxClicked(QAbstractButton* btn)
{
	if(btn == m_selectAll)
	{
		for(int i = 0 ; i < m_listFile->count(); i++)
		{
			m_listFile->item(i)->setCheckState(Qt::Checked);
		}
	}
	else if(btn == m_deselectAll)
	{
		for(int i = 0 ; i < m_listFile->count(); i++)
		{
			m_listFile->item(i)->setCheckState(Qt::Unchecked);
		}
	}
}

void SavingDialog::addEditor(AbstractEditor * editor)
{
	Q_ASSERT_X(editor, "SavingDialog::addEditor", "Editor must be initialized");
	EditorItem * item = new EditorItem(editor, m_listFile);
	item->setCheckState(Qt::Checked);
}

int SavingDialog::countEditor()
{
	return m_listFile->count();
}

QList<AbstractEditor*> SavingDialog::selectedEditor()
{
	QList<AbstractEditor*> result;
	for(int i = 0; i < countEditor(); i++)
	{
		EditorItem * item = static_cast<EditorItem*>(m_listFile->item(i));
		if(item->checkState() == Qt::Checked)
		{
			result << item->m_editor;
		}
	}

	return result;
}
