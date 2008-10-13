/****************************************************************************
**
** Copyright (C) 2006-2008 fullmetalcoder <fullmetalcoder@hotmail.fr>
**
** This file is part of the Edyuk project <http://edyuk.org>
** 
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef _QCODE_EDIT_H_
#define _QCODE_EDIT_H_

#include "qce-config.h"

#include <QList>
#include <QPointer>
#include <QGenericArgument>

class QPanel;
class QEditor;
class QWidget;
class QString;
class QAction;
class QPanelLayout;

#define Q_COMMAND QList<QGenericArgument>()

class QCE_EXPORT QCodeEdit
{
	public:
		enum Position
		{
			West,
			North,
			South,
			East
		};
		
		QCodeEdit(QWidget *p = 0);
		QCodeEdit(const QString& layout, QWidget *p = 0);
		virtual ~QCodeEdit();
		
		QEditor* editor() const;
		QPanelLayout* panelLayout() const;
		
		QAction* addPanel(QPanel *panel, Position pos, bool _add = false);
		
		QList<QPanel*> panels() const;
		QAction* toggleViewAction(QPanel *p) const;
		
		void sendPanelCommand(	const QString& type,
								const char *signature,
								const QList<QGenericArgument>& args = Q_COMMAND);
		
		static QCodeEdit* manager(QEditor *e);
		static QEditor* managed(const QString& f);
		
	private:
		QPointer<QEditor> m_editor;
		QPointer<QPanelLayout> m_layout;
		
		QList<QAction*> m_actions;
		
		static QList<QCodeEdit*> m_instances;
};

#endif // _QCODE_EDIT_H_
