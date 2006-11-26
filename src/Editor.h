/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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
 
#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <QTextEdit>

class QTabWidget;
class QTextEdit;
class QHBoxLayout;
class QPixmap;
class QRect;
class QPaintEvent;

class NumberBar : public QWidget {
     Q_OBJECT
public:
     NumberBar( QWidget *parent );
     ~NumberBar();
 
     void setCurrentLine( int lineno );
     void setStopLine( int lineno );
     void setBugLine( int lineno );
 
     int currentLine() const;
     int stopLine() const;
     int bugLine() const;
 
     void setTextEdit( QTextEdit *edit );
     void paintEvent( QPaintEvent *ev );
protected:
     bool event( QEvent *ev );
 private:
     QTextEdit *edit;
     QPixmap stopMarker;
     QPixmap currentMarker;
     QPixmap bugMarker;
     int m_stopLine;
     int m_currentLine;
     int m_bugLine;
     QRect stopRect;
     QRect currentRect;
     QRect bugRect;
};

class Editor : public QFrame {
  Q_OBJECT
  Q_PROPERTY( QString text READ text WRITE setText )
  Q_PROPERTY( int currentLine READ currentLine WRITE setCurrentLine )
  Q_PROPERTY( int stopLine READ stopLine WRITE setStopLine )
  Q_PROPERTY( int bugLine READ bugLine WRITE setBugLine )public:
public:
  Editor( QWidget *parent = 0 );

  const QString & getCurrentFile() const;

  void loadFile(const QString &fileName);
  bool saveFile(const QString &fileName = "");
 
  QTextEdit *textEdit() const { return view; }
 
  void setCurrentLine( int lineno );
  void setStopLine( int lineno );
  void setBugLine( int lineno );

  int currentLine() const;
  int stopLine() const;
  int bugLine() const;

  void duplicateCurrentLine();
  void moveLineUp();
  void moveLineDown();
  void upperSelectedText();
  void lowerSelectedText();
  void commentSelectedText();
  void uncommentSelectedText();

  bool eventFilter( QObject *obj, QEvent *event );

  QString text() const;
  void setText( const QString &text );
signals:
  void mouseHover( const QString &word );
  void mouseHover( const QPoint &pos, const QString &word );

protected slots:
  void textChanged( int pos, int added, int removed );
private:
  void setCurrentFile(const QString & name);

  QString m_curFile;

  QTextEdit *view;
  NumberBar *numbers;
  QHBoxLayout *box;
  QTextCursor highlight;
};

#endif
