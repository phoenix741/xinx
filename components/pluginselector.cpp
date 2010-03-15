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
#include "p_pluginselector.h"
#include "pluginelement.h"

// Qt header
#include <QApplication>
#include <QMessageBox>
#include <QFont>

Q_DECLARE_METATYPE(PluginElement*);

/* PrivateXinxPluginSelector */

PrivatePluginSelector::PrivatePluginSelector(PluginSelector * parent) : m_parent(parent)
{
	m_model = new PluginModel(parent);
	m_delegate = new PluginDelegate(parent);
}

/* XinxPluginModel */

PluginModel::PluginModel(QWidget * parent) : QAbstractListModel(parent)
{

}

PluginModel::~PluginModel()
{

}

void PluginModel::addPlugin(PluginElement * plugin)
{
	beginInsertRows(QModelIndex(), m_plugins.count(), m_plugins.count());
	m_plugins.append(plugin);
	endInsertRows();
}

const QList<PluginElement*> & PluginModel::plugins() const
{
	return m_plugins;
}

void PluginModel::clear()
{
	m_plugins.clear();
	reset();
}

bool PluginModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!(index.isValid() && value.isValid())) return false;

	int i = index.row();
	if ((i < 0) || (i >= m_plugins.count())) return false;

	PluginElement * element;
	switch (role)
	{
	case Qt::CheckStateRole:
		element = m_plugins.at(i);
		if (! element->isModifiable()) return false;
		element->setActivated(value.toBool());
		emit dataChanged(index, index);
		return true;
	default:
		return false;
	}

	return false;
}

QVariant PluginModel::data(const QModelIndex &index, int role) const
{
	if (! index.isValid())
		return QVariant();

	int i = index.row();
	if ((i < 0) || (i >= m_plugins.count())) return QVariant();
	PluginElement * e = m_plugins.at(i);

	switch (role)
	{
	case Qt::DisplayRole:
	case PluginModel::PLG_NAME:
		return e->name();
	case PluginModel::PLG_DESCRIPTION:
		return e->description();
	case PluginModel::PLG_ICON:
		return e->pixmap();
	case Qt::UserRole:
	{
		QVariant result;
		result.setValue(e);
		return result;
	}
	case Qt::CheckStateRole:
		return e->isActivated();
	default:
		return QVariant();
	}
}

Qt::ItemFlags PluginModel::flags(const QModelIndex &index) const
{
	if (index.isValid())
	{
		int i = index.row();
		if ((i < 0) || (i >= m_plugins.count()))
			return QAbstractListModel::flags(index);

		if (! m_plugins.at(i)->isModifiable())
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
		else
			return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	else
		return QAbstractListModel::flags(index);
}

QModelIndex PluginModel::index(int row, int column, const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return createIndex(row, column);
}

int PluginModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_plugins.count();
}

/* PluginDelegate */

PluginDelegate::PluginDelegate(QObject * parent) : QItemDelegate(parent), m_separatorPixels(8), m_rightMargin(0), m_leftMargin(0),
		m_iconHeight(32), m_iconWidth(32), m_minimumItemWidth(200), m_buttonPressed(false)
{
}

PluginDelegate::~PluginDelegate()
{

}

bool PluginDelegate::eventFilter(QObject *watched, QEvent *event)
{
	QWidget * viewport = qobject_cast<QWidget*>(watched);
	PluginSelector * listView  = qobject_cast<PluginSelector*>(viewport->parent());
	if (! listView)
		listView = qobject_cast<PluginSelector*>(viewport);
	if (!(listView && viewport)) return false;

	m_cursorPosition = viewport->mapFromGlobal(QCursor::pos());

	QModelIndex currentIndex;
	if (listView)
		currentIndex = listView->indexAt(m_cursorPosition);

	if (! currentIndex.isValid()) return false;

	QAbstractItemModel * model = listView->model();
	PluginElement* element = model->data(currentIndex, Qt::UserRole).value<PluginElement*>();

	QStyleOptionViewItem option;
	option.fontMetrics = listView->viewOptions().fontMetrics;
	option.direction = listView->layoutDirection();
	option.rect = listView->visualRect(currentIndex);
	if (element->isActivated())
		option.state |= QStyle::State_Enabled;
	else
		option.state &= ~ QStyle::State_Enabled;

	QStyleOptionButton optAbout, optConfigure;
	optAbout = calculateButtonAbout(option);
	if (element->isConfigurable())
		optConfigure = calculateButtonConfigure(option, m_separatorPixels + optAbout.rect.width());

	QRect rectCheck;
	QStyleOptionViewItem optCheck = calculateCheckbox(option, rectCheck);

	if (!(optAbout.rect.contains(m_cursorPosition) || optConfigure.rect.contains(m_cursorPosition) || rectCheck.contains(m_cursorPosition)))
		return false;

	if (viewport && (event->type() == QEvent::MouseMove))
	{
		viewport->update();
		return true;
	}
	else if (viewport && (event->type() == QEvent::MouseButtonPress))
	{
		m_buttonPressed  = true;
		viewport->update();
		return true;
	}
	else if (viewport && (event->type() == QEvent::MouseButtonRelease))
	{
		m_buttonPressed  = false;
		viewport->update();

		if (optAbout.rect.contains(m_cursorPosition) && optAbout.state.testFlag(QStyle::State_Enabled))
			emit aboutPlugin(element);
		if (optConfigure.rect.contains(m_cursorPosition) && optConfigure.state.testFlag(QStyle::State_Enabled))
			emit configurePlugin(element);
		if (rectCheck.contains(m_cursorPosition))
		{
			bool value = model->data(currentIndex, Qt::CheckStateRole).toBool();
			model->setData(currentIndex, ! value, Qt::CheckStateRole);
		}

		return true;
	}
	return false;
}

void PluginDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	painter->save();

	bool isActivated = index.model()->data(index, Qt::UserRole).value<PluginElement*>()->isActivated();
	QStyleOptionViewItem myOption = option;
	if (isActivated)
		myOption.state |= QStyle::State_Enabled;
	else
		myOption.state &= ~ QStyle::State_Enabled;

	QStyleOptionViewItem titleOpt(myOption);
	QFontMetrics metrics = painter->fontMetrics();
	QFont titleFont(painter->font());
	QFont descFont(painter->font());
	titleFont.setBold(true);
	titleOpt.font = titleFont;
	titleOpt.fontMetrics = QFontMetrics(titleFont);

	if (myOption.state & QStyle::State_Selected)
	{
		painter->fillRect(myOption.rect, myOption.palette.color(QPalette::Highlight));
	}
	else
	{
		if (index.row() % 2 != 0)
			painter->fillRect(myOption.rect, myOption.palette.color(QPalette::AlternateBase));
		else
			painter->fillRect(myOption.rect, myOption.palette.color(QPalette::Base));
	}

	// Draw checkbox
	QRect checkRect;
	QStyleOptionViewItem checkOpt = calculateCheckbox(option, checkRect);
	if (index.model()->flags(index) & Qt::ItemIsUserCheckable)
		drawCheck(painter, checkOpt, checkRect, index.model()->data(index, Qt::CheckStateRole).toBool() ? Qt::Checked : Qt::Unchecked);

	// Draw icon
	QPixmap iconPixmap = index.model()->data(index, PluginModel::PLG_ICON).value<QPixmap>();
	iconPixmap = iconPixmap.scaled(iconSize());
	int pixmapLeftPosition = myOption.direction == Qt::LeftToRight ?
	                         checkRect.right() + m_separatorPixels
	                         :   checkRect.left() - m_separatorPixels - iconPixmap.width();

	painter->drawPixmap(pixmapLeftPosition, (myOption.rect.height() - iconPixmap.height()) / 2 + myOption.rect.top(), iconPixmap);

	// Draw buttons
	const QAbstractItemModel * model = index.model();
	PluginElement* element = model->data(index, Qt::UserRole).value<PluginElement*>();

	QStyleOptionButton optAbout = drawButtonAbout(painter, myOption);
	QStyleOptionButton optConfigure;
	if (element->isConfigurable())
		optConfigure = drawButtonConfigure(painter, myOption, m_separatorPixels + optAbout.rect.width());

	// Draw text
	int maxTextLength = myOption.rect.width() - checkRect.width() - iconPixmap.width() - m_leftMargin - m_rightMargin - m_separatorPixels * 7 - optAbout.rect.width() - optConfigure.rect.width();

	QString display = index.model()->data(index, PluginModel::PLG_NAME).toString();
	QString description = index.model()->data(index, PluginModel::PLG_DESCRIPTION).toString();

#	define LeftPosition( text, metrics ) myOption.direction == Qt::LeftToRight ? \
	pixmapLeftPosition + iconPixmap.width() + m_separatorPixels * 2 : \
	pixmapLeftPosition - iconPixmap.width() - m_separatorPixels * 2 - metrics.width( text )

	if (myOption.state & QStyle::State_Selected)
		painter->setPen(myOption.palette.color(QPalette::HighlightedText));

	painter->setFont(titleFont);
	display = titleOpt.fontMetrics.elidedText(display, Qt::ElideRight, maxTextLength);
	painter->drawText(
	    LeftPosition(display, titleOpt.fontMetrics),
	    m_separatorPixels + myOption.rect.top(),
	    titleOpt.fontMetrics.width(display),
	    titleOpt.fontMetrics.height(),
	    Qt::AlignLeft,
	    display);

	painter->setFont(descFont);
	description = metrics.elidedText(description, Qt::ElideRight, maxTextLength);
	painter->drawText(
	    LeftPosition(description, metrics),
	    myOption.rect.height() - m_separatorPixels - metrics.height() + myOption.rect.top(),
	    metrics.width(description),
	    metrics.height(),
	    Qt::AlignLeft,
	    description);

	painter->restore();
}

QStyleOptionButton PluginDelegate::calculateButtonAbout(const QStyleOptionViewItem & option, int decalage) const
{
	return calculateButton(QIcon(":/images/help-about.png"), tr("About ..."), option, decalage);
}

QStyleOptionButton PluginDelegate::drawButtonAbout(QPainter * painter, const QStyleOptionViewItem & option, int decalage) const
{
	QStyleOptionButton o = calculateButtonAbout(option, decalage);
	QApplication::style()->drawControl(QStyle::CE_PushButton, &o, painter);
	return o;
}

QStyleOptionButton PluginDelegate::calculateButtonConfigure(const QStyleOptionViewItem & option, int decalage) const
{
	return calculateButton(QIcon(":/images/configure.png"), tr("Configure ..."), option, decalage);
}

QStyleOptionButton PluginDelegate::drawButtonConfigure(QPainter * painter, const QStyleOptionViewItem & option, int decalage) const
{
	QStyleOptionButton o = calculateButtonConfigure(option, decalage);
	QApplication::style()->drawControl(QStyle::CE_PushButton, &o, painter);
	return o;
}

QStyleOptionButton PluginDelegate::calculateButton(const QIcon & icon, const QString & caption, const QStyleOptionViewItem & option, int decalage) const
{
	QSize buttonSize;
	QStyleOptionButton o;
	if (option.state.testFlag(QStyle::State_Enabled))
		o.state |= QStyle::State_Enabled;

	o.icon = icon;
	o.iconSize = QSize(16, 16);
	o.text = caption;
	o.fontMetrics = option.fontMetrics;

	buttonSize = QApplication::style()->sizeFromContents(
	                 QStyle::CT_PushButton,
	                 &option,
	                 QSize(o.fontMetrics.width(caption) + o.iconSize.width() + m_separatorPixels * 3,
	                       qMax(o.fontMetrics.height(), o.iconSize.height()) + m_separatorPixels
	                      )
	             );

	o.rect.setTop((option.rect.height() - buttonSize.height()) / 2 + option.rect.top());
	o.rect.setSize(buttonSize);
	if (option.direction == Qt::LeftToRight)
		o.rect.setLeft(option.rect.right() - m_rightMargin - m_separatorPixels - decalage - o.rect.width());
	else
		o.rect.setLeft(m_leftMargin + m_separatorPixels + decalage);
	o.rect.setSize(buttonSize);

	if (o.rect.contains(m_cursorPosition))
	{
		o.state |= QStyle::State_MouseOver;
		if (m_buttonPressed && o.state.testFlag(QStyle::State_Enabled))
			o.state |= QStyle::State_Sunken;
	}

	return o;
}

QStyleOptionButton PluginDelegate::drawButton(QPainter * painter, const QIcon & icon, const QString & caption, const QStyleOptionViewItem & option, int decalage) const
{
	QStyleOptionButton o = calculateButton(icon, caption, option, decalage);
	QApplication::style()->drawControl(QStyle::CE_PushButton, &o, painter);
	return o;
}

QStyleOptionViewItem PluginDelegate::calculateCheckbox(const QStyleOptionViewItem & option, QRect & rect, int decalage) const
{
	QStyleOptionViewItem checkOpt(option);
	rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option);
	QSize checkSize = rect.size();
	rect.setTopLeft(QPoint(
	                    option.direction == Qt::LeftToRight ? option.rect.left() + m_leftMargin + m_separatorPixels + decalage
	                    : option.rect.right() - m_rightMargin -  m_separatorPixels - checkSize.width() - decalage,
	                    (option.rect.height() - checkSize.height()) / 2 + option.rect.top()
	                ));
	rect.setSize(checkSize);
	return checkOpt;
}

QSize PluginDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(index);

	QFont title(option.font);
	title.setBold(true);

	QFontMetrics titleMetrics(title);
	QFontMetrics currentMetrics(option.font);

	QStyleOptionButton optAbout = calculateButtonAbout(option);

	int height = qMax(m_iconHeight, titleMetrics.height() + currentMetrics.height() + m_separatorPixels);
	height = qMax(optAbout.rect.height(), height);
	height += m_separatorPixels * 2;

	return QSize(m_minimumItemWidth, height);
}


/* PluginSelector */

PluginSelector::PluginSelector(QWidget *parent) : QListView(parent)
{
	Q_INIT_RESOURCE(xinxpluginselector);
	qRegisterMetaType<PluginElement*>("PluginElement*");

	d = new PrivatePluginSelector(this);
	connect(d->m_delegate, SIGNAL(aboutPlugin(PluginElement*)), this, SIGNAL(aboutPlugin(PluginElement*)));
	connect(d->m_delegate, SIGNAL(configurePlugin(PluginElement*)), this, SIGNAL(configurePlugin(PluginElement*)));
	setModel(d->m_model);
	setItemDelegate(d->m_delegate);
	viewport()->installEventFilter(d->m_delegate);
	installEventFilter(d->m_delegate);
	setMouseTracking(true);
}

PluginSelector::~PluginSelector()
{
	delete d;
}

void PluginSelector::addPlugin(PluginElement * plugin)
{
	d->m_model->addPlugin(plugin);
}

const QList<PluginElement*> & PluginSelector::plugins() const
{
	return d->m_model->plugins();
}

void PluginSelector::clear()
{
	d->m_model->clear();
}

QStyleOptionViewItem PluginSelector::viewOptions() const
{
	return QListView::viewOptions();
}
