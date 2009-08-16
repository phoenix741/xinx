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

#ifndef CUSTOMDIALOGMODELES_H_
#define CUSTOMDIALOGMODELES_H_
#pragma once

// Qt header
#include <QString>
#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QHash>
#include <QApplication>

// Xinx header
#include "scriptmanager.h"
#include <plugins/xinxpluginsloader.h>

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
	SpecifiqueModelIndex( QHash<QString,AppSettings::struct_extentions> * extentions, QObject * parent = 0 );
	virtual ~SpecifiqueModelIndex();

	void setExtentions( QHash<QString,AppSettings::struct_extentions> * extentions );
	const QHash<QString,AppSettings::struct_extentions> & extentions() const;

	virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
private:
	QHash<QString,AppSettings::struct_extentions> m_extentions;
};

class ScriptElement : public PluginElement {
	Q_DECLARE_TR_FUNCTIONS(ScriptElement)
public:
	ScriptElement( const ScriptValue & script );

	virtual bool isModifiable() const;
	virtual bool isConfigurable() const;

	virtual QPixmap pixmap() const;
	virtual QString name() const;
	virtual QString author() const;
	virtual QString version() const;
	virtual QString licence() const;
	virtual QString description() const;

	ScriptValue & script();
private:
	ScriptValue m_script;
};

#endif /* CUSTOMDIALOGMODELES_H_ */
