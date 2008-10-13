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

#ifndef _QDOCUMENT_COMMAND_H_
#define _QDOCUMENT_COMMAND_H_

#include "qce-config.h"

#include <QUndoCommand>

#include "qdocument.h"

class QDocumentLine;
class QDocumentLineHandle;
class QDocumentCursorHandle;

class QCE_EXPORT QDocumentCommand : public QUndoCommand
{
	public:
		enum Command
		{
			None,
			Insert,
			Erase,
			Replace,
			Custom
		};
		
		struct TextCommandData
		{
			QString begin, end;
			int lineNumber, startOffset, endOffset;
			QList<QDocumentLineHandle*> handles;
		};
		
		QDocumentCommand(Command c, QDocument *d, QDocumentCommand *p = 0);
		virtual ~QDocumentCommand();
		
		virtual int id() const;
		
		virtual bool mergeWith(const QUndoCommand *command);
		
		virtual void redo();
		virtual void undo();
		
		bool isSilent() const;
		void setSilent(bool y);
		
		void setTargetCursor(QDocumentCursorHandle *h);
		
		void setRedoOffset(int off);
		void setUndoOffset(int off);
		
		static bool isAutoUpdated(const QDocumentCursorHandle *h);
		static void enableAutoUpdate(QDocumentCursorHandle *h);
		static void disableAutoUpdate(QDocumentCursorHandle *h);
		
	protected:
		bool m_state;
		QDocument *m_doc;
		int m_redoOffset, m_undoOffset;
		
		void markRedone(QDocumentLineHandle *h);
		void markUndone(QDocumentLineHandle *h);
		
		void updateTarget(int l, int offset);
		
		void insertText(QDocumentLineHandle *h, int pos, const QString& s);
		void removeText(QDocumentLineHandle *h, int pos, int length);
		
		void insertLines(int after, const QList<QDocumentLineHandle*>& l);
		void removeLines(int after, int n);
		
	private:
		bool m_silent;
		Command m_command;
		QDocumentCursorHandle *m_cursor;
		
		static QList<QDocumentCursorHandle*> m_autoUpdated;
};

Q_DECLARE_TYPEINFO(QDocumentCommand::TextCommandData, Q_MOVABLE_TYPE);

class QCE_EXPORT QDocumentInsertCommand : public QDocumentCommand
{
	public:
		QDocumentInsertCommand(	int l, int offset,
								const QString& text,
								QDocument *doc,
								QDocumentCommand *p = 0);
		
		virtual ~QDocumentInsertCommand();
		
		virtual bool mergeWith(const QUndoCommand *command);
		
		virtual void redo();
		virtual void undo();
		
	private:
		TextCommandData m_data;
};

class QCE_EXPORT QDocumentEraseCommand : public QDocumentCommand
{
	public:
		QDocumentEraseCommand(	int bl, int bo,
								int el, int eo,
								QDocument *doc,
								QDocumentCommand *p = 0);
		
		virtual ~QDocumentEraseCommand();
		
		virtual bool mergeWith(const QUndoCommand *command);
		
		virtual void redo();
		virtual void undo();
		
	private:
		TextCommandData m_data;
};

class QDocumentCommandBlock : public QDocumentCommand
{
	public:
		QDocumentCommandBlock(QDocument *d);
		virtual ~QDocumentCommandBlock();
		
		virtual void redo();
		virtual void undo();
		
		void setWeakLock(bool l);
		bool isWeakLocked() const;
		
		void addCommand(QDocumentCommand *c);
		void removeCommand(QDocumentCommand *c);
		
	private:
		bool m_weakLocked;
		QList<QDocumentCommand*> m_commands;
};

#endif