
#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <QMainWindow>
#include "ui_window.h"

class QCodeEdit;
class QFormatFactory;
class QLanguageFactory;

class Window : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT
	
	public:
		Window(QWidget *p = 0);
		
	public slots:
		void load(const QString& file);
		
	private slots:
		void on_action_Open_triggered();
		
		void on_action_Settings_triggered();
		void on_action_Reload_syntax_files_triggered();
		
		void on_action_About_triggered();
		void on_action_About_Qt_triggered();
		
	private:
		QCodeEdit *m_editControl;
		QFormatFactory *m_formats;
		QLanguageFactory *m_languages;
};

#endif
