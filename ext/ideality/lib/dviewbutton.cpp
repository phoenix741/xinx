/***************************************************************************
 *   Copyright (C) 2006 by David Cuadrado                                *
 *   krawek@gmail.com                                                      *
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

#include "dviewbutton.h"

#include "dtoolview.h"

#include <QToolBar>
#include <QStylePainter>
#include <QMenu>
#include <QIcon>
#include <QMouseEvent>
#include <QMainWindow>

#include <QTimer>
#include <QtDebug>

class DViewButton::Animator
{
	public:
		Animator(QObject *parent) : count(0), MAXCOUNT(30), INTERVAL(30), isEnter(false)
		{
			timer = new QTimer(parent);
		}
		~Animator() {}
		void start()
		{
			timer->start(INTERVAL);
		}
		
		QColor blendColors( const QColor& color1, const QColor& color2, int percent )
		{
			const float factor1 = ( 100 - ( float ) percent ) / 100;
			const float factor2 = ( float ) percent / 100;

			const int r = static_cast<int>( color1.red() * factor1 + color2.red() * factor2 );
			const int g = static_cast<int>( color1.green() * factor1 + color2.green() * factor2 );
			const int b = static_cast<int>( color1.blue() * factor1 + color2.blue() * factor2 );
			
			QColor result;
			
			if ( r > 0 && r < 255 && g > 0 && g < 255 && b > 0 && b < 255 )
			{
				result.setRgb( r , g , b);
			}
			
			return result;
		}
			
		QTimer *timer;
		int count;
		const int MAXCOUNT;
		const int INTERVAL;
			
		bool isEnter;
};

DViewButton::DViewButton(Qt::ToolBarArea area, DToolView *toolView, QWidget * parent) : QToolButton(parent), m_area(area), m_isClicked(false), m_toolView(toolView)
{
	setup();
}


DViewButton::DViewButton(DToolView *toolView, QWidget *parent) : QToolButton(parent), m_area(Qt::LeftToolBarArea), m_isClicked(false), m_toolView(toolView)
{
	setup();
}

void DViewButton::setup()
{
	connect(m_toolView->toggleViewAction(), SIGNAL(triggered()), this, SLOT(viewtoggled()));

	setIconText();
// 	setAutoExclusive(true);
	setCheckable( true );
	setAutoRaise(true);
	
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setFocusPolicy(Qt::NoFocus);
	
	m_isSensible = false;
	
	m_animator = new Animator(this);
	connect( m_animator->timer, SIGNAL( timeout() ), this, SLOT( animate() ) );
	
	setChecked(false);
// 	setDown(false);
	
	m_blending = true;
	
	///
	QAction *act = m_toolView->toggleViewAction();
	
	setText(m_toolView->windowTitle());
	setIcon(m_toolView->windowIcon());
	
	if ( !isChecked() )
	{
		m_toolView->close();
	}
}

DViewButton::~DViewButton()
{
	delete m_animator;
}

void DViewButton::setIconText()
{
	setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

void DViewButton::setOnlyText()
{
	setToolButtonStyle(Qt::ToolButtonTextOnly);
}

void DViewButton::setOnlyIcon()
{
	setToolButtonStyle(Qt::ToolButtonIconOnly);
}


void DViewButton::setArea(Qt::ToolBarArea area)
{
	m_area = area;
	update();
}

Qt::ToolBarArea DViewButton::area() const
{
	return m_area;
}

QSize DViewButton::sizeHint() const
{
	QSize size = QToolButton::sizeHint();
	
	if ( m_area == Qt::RightToolBarArea || m_area == Qt::LeftToolBarArea )
	{
		size.transpose();
	}
	
	return size;
}


QStyleOptionToolButton DViewButton::styleOption() const
{
	QStyleOptionToolButton opt;
	opt.init(this);
	
	bool down = isDown();
	bool checked = defaultAction() ? defaultAction()->isChecked() : isChecked();
	bool forceNoText = false;
	
	
	opt.toolButtonStyle = toolButtonStyle();
	if ( m_area == Qt::LeftToolBarArea )
	{
		QSize size = opt.rect.size();
		size.transpose();
		opt.rect = QRect(QPoint(opt.rect.x(), opt.rect.y()), size );
	}
	else if ( m_area == Qt::RightToolBarArea )
	{
		QSize size = opt.rect.size();
		size.transpose();
		opt.rect = QRect(QPoint(opt.rect.x(), opt.rect.y()), size );
	}
	
	if (parentWidget())
	{
		if (QToolBar *toolBar = qobject_cast<QToolBar *>(parentWidget()))
		{
			opt.iconSize = toolBar->iconSize();
		}
		else 
		{
			opt.iconSize = iconSize();
		}
	}

	if (!forceNoText)
	{
		opt.text = text();
	}
	
	opt.icon = icon();
	
	opt.arrowType = arrowType();
	
#if 1
	if (down)
		opt.state |= QStyle::State_Sunken;
// 	if (checked)
// 		opt.state |= QStyle::State_On;
	if (autoRaise())
		opt.state |= QStyle::State_AutoRaise;
	if (!checked && !down)
		opt.state |= QStyle::State_Raised;
#endif 

	opt.subControls = QStyle::SC_ToolButton;
	opt.activeSubControls = QStyle::SC_None;

	opt.features = QStyleOptionToolButton::None;
// 	if (popupMode() == QToolButton::MenuButtonPopup) {
// 		opt.subControls |= QStyle::SC_ToolButtonMenu;
// 		opt.features |= QStyleOptionToolButton::Menu;
// 		if (menuButtonDown || down) {
// 			opt.state |= QStyle::State_MouseOver;
// 			opt.activeSubControls |= QStyle::SC_ToolButtonMenu;
// 		}
// 	} 
// 	else 
// 	{
// 		if (menuButtonDown)
// 			opt.state  |= QStyle::State_Sunken;
// 	}
	
	if (arrowType() != Qt::NoArrow)
	{
		opt.features |= QStyleOptionToolButton::Arrow;
	}
	
	if (popupMode() == QToolButton::DelayedPopup)
	{
		opt.features |= QStyleOptionToolButton::PopupDelay;
	}
	
// 	opt.toolButtonStyle = toolButtonStyle();
// 	
// 	if (icon().isNull() && arrowType() == Qt::NoArrow && !forceNoText) 
// 	{
// 		if (!text().isEmpty())
// 		{
// 			opt.toolButtonStyle = Qt::ToolButtonTextOnly;
// 		}
// 		else if (opt.toolButtonStyle != Qt::ToolButtonTextOnly)
// 		{
// 			opt.toolButtonStyle = Qt::ToolButtonIconOnly;
// 		}
// 	}
// 	else
// 	{
// 		if (text().isEmpty() && opt.toolButtonStyle != Qt::ToolButtonIconOnly)
// 		{
// 			opt.toolButtonStyle = Qt::ToolButtonIconOnly;
// 		}
// 	}

	opt.pos = pos();
	opt.font = font();
	return opt;
}

void DViewButton::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e);
	
	QStyleOptionToolButton opt = styleOption();
	
	QRect r = opt.rect;
	QColor fillColor, textColor;
	
	bool checked = defaultAction() ? defaultAction()->isChecked() : isChecked();
	
	if ( m_animator->count <= 0 ) m_animator->count = 1;
	
	if ( checked )
	{
		fillColor = m_animator->blendColors( palette().highlight().color(), palette().background().color(), static_cast<int>( m_animator->count * 3.5 ) );
		textColor = m_animator->blendColors( palette().highlightedText().color(), palette().text().color(), static_cast<int>( m_animator->count * 4.5 ) );
	}
	else
	{
		fillColor = m_animator->blendColors( palette().background().color(), palette().highlight().color(), static_cast<int>( m_animator->count * 3.5 ) );
		textColor = m_animator->blendColors( palette().text().color(), palette().highlightedText().color(), static_cast<int>( m_animator->count * 4.5 ) );
	}
	
	opt.palette.setColor(QPalette::Background, fillColor.isValid() ? fillColor : m_palette.background().color() );
// 	opt.palette.setColor(QPalette::Text, textColor);
	
// 	opt.palette.setColor(QPalette::Button, fillColor);
	opt.palette.setColor(QPalette::ButtonText, textColor.isValid() ? textColor : m_palette.text().color() );
	
	QPixmap pm(r.width(), r.height());
	pm.fill(fillColor.isValid() ? fillColor : m_palette.color(QPalette::Background));
	
	
	QStylePainter p(&pm, this);
	p.drawComplexControl(QStyle::CC_ToolButton, opt);
	
	QPainter painter(this);

	switch (m_area)
	{
		case Qt::LeftToolBarArea:
			painter.rotate(-90);
			painter.drawPixmap(-pm.width(), 0, pm);
			break;
		case Qt::RightToolBarArea:
			painter.rotate(90);
			painter.drawPixmap(0, -pm.height(), pm);
			break;
		default:
			painter.drawPixmap(0, 0, pm);
			break;
	}
	
	m_palette.setBrush(QPalette::Background, opt.palette.background());
	m_palette.setBrush(QPalette::ButtonText, opt.palette.buttonText());
}

QMenu *DViewButton::createMenu()
{
	QMenu *menu = new QMenu(tr("Menu"), this);
	menu->addAction(tr("Only icon"), this, SLOT(setOnlyIcon()) );
	menu->addAction(tr("Only text"), this, SLOT(setOnlyText()) );
	menu->addAction(tr("Text && Icon"), this, SLOT(setIconText()) );
	menu->addSeparator();
	
	QAction *a = menu->addAction(tr("Mouse sensibility")/*, this, SLOT(toggleSensibility())*/);
	connect(a, SIGNAL(toggled(bool)), this, SLOT(setSensible( bool ) ));
	a->setCheckable(true);
	a->setChecked( isSensible() );
	
	return menu;
}

void DViewButton::mousePressEvent(QMouseEvent *e)
{
	QToolButton::mousePressEvent(e);
	
	if ( e->button() == Qt::RightButton )
	{
		createMenu()->exec(e->globalPos());
		e->accept();
	}
}

void DViewButton::enterEvent( QEvent* )
{
// 	bool checked = defaultAction() ? defaultAction()->isChecked() : isChecked();
// 	qDebug() << "CHECKED: " << checked << " DOWN: " << isDown();
	
	if ( m_isSensible )
	{
		m_isSensible = false;
		animateClick();
		
		QTimer::singleShot(300, this, SLOT(toggleSensibility()));
	}
	
	m_animator->isEnter = true;
	m_animator->count = 1;
	
	if ( m_blending )
	{
		m_animator->start();
	}
}

void DViewButton::leaveEvent( QEvent* )
{
	if ( m_animator->count == 0 )
	{
		m_animator->count = 1;
	}
	
	m_animator->isEnter = false;
	
	if ( m_blending )
	{
		m_animator->timer->start();
	}
}

void DViewButton::animate()
{
	if ( m_animator->isEnter ) 
	{
		m_animator->count += 1;
		update();
		if ( m_animator->count > m_animator->MAXCOUNT )
		{
			m_animator->timer->stop();
		}
	} else 
	{
		m_animator->count -= 1;
		if ( m_animator->count < 1 )
		{
			m_animator->timer->stop();
			m_animator->count = 1;
		}
		
		update();
	}
}

void DViewButton::toggleSensibility()
{
	m_isSensible = !m_isSensible;
}

void DViewButton::setSensible(bool s)
{
	m_isSensible = s;
}

bool DViewButton::isSensible() const
{
	return m_isSensible;
}

void DViewButton::setBlending(bool e)
{
	m_blending = e;
}

bool DViewButton::blending() const
{
	return m_blending;
}


void DViewButton::toggleView()
{

	QMainWindow *mw = static_cast<QMainWindow *>(m_toolView->parentWidget());
	
	m_toolView->setUpdatesEnabled(false);
	if ( mw  )
	{
		mw->setUpdatesEnabled( false );
	}
	
	if (!m_isClicked)
	{
		m_toolView->toggleViewAction()->trigger();
	}
	setChecked( m_toolView->isVisible() );
	
	
	m_toolView->setUpdatesEnabled(true);
	if ( mw )
	{
		mw->setUpdatesEnabled(true);
	}
}

void DViewButton::viewtoggled()
{
	if (! m_isClicked)
	{
		m_isClicked = true;
		click();
		m_isClicked = false;
	}
}

DToolView *DViewButton::toolView() const
{
	return m_toolView;
}
