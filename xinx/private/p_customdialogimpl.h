/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef P_CUSTOMDIALOGIMPL_H_
#define P_CUSTOMDIALOGIMPL_H_

// Qt header
#include <QString>
#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QHash>

// Xinx header
#include "../customdialogimpl.h"
#include "../syntaxhighlighter.h"

class PrivateCustomDialogImpl {
public:
	PrivateCustomDialogImpl( CustomDialogImpl * parent );

	XINXConfig m_config;
	QString m_previousFormat;
	SyntaxHighlighter * m_highlighter;
	
	void showConfig();
	void storeConfig();
private:
	CustomDialogImpl * m_parent;
};

class ToolsModelIndex : public QAbstractTableModel {
	Q_OBJECT
public:
	ToolsModelIndex( QHash<QString,QString> * tools, QObject * parent = 0 );
	virtual ~ToolsModelIndex();
	
	void setTools( QHash<QString,QString> * tools );

	virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
private:
	QHash<QString,QString> * m_tools;
};

class DirectoryEditDelegate : public QItemDelegate {
	Q_OBJECT
public:
	DirectoryEditDelegate( QObject *parent = 0 );

    QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    void setEditorData( QWidget *editor, const QModelIndex &index ) const;
    void setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;
    void updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    
    void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
};

class SpecifiqueModelIndex : public QAbstractTableModel {
	Q_OBJECT
public:
	SpecifiqueModelIndex( QHash<QString,struct_extentions> * extentions, QObject * parent = 0 );
	virtual ~SpecifiqueModelIndex();
	
	void setExtentions( QHash<QString,struct_extentions> * extentions );
	const QHash<QString,struct_extentions> & extentions() const;

	virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
private:
	QHash<QString,struct_extentions> m_extentions;
};


#endif /*P_CUSTOMDIALOGIMPL_H_*/