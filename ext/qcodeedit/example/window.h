
#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <QString>
#include <QMainWindow>
#include "ui_window.h"

class QCodeEdit;
class QEditConfig;
class QFormatConfig;
class QFormatScheme;
class QLanguageFactory;

class SnippetManager;

class Window : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT
	
	public:
		Window(QWidget *p = 0);
		virtual ~Window();

	public slots:
		void load(const QString& file);
		
	protected:
		virtual void closeEvent(QCloseEvent *e);
		
		virtual void switchPage(int i);
		
	private slots:
		void on_action_New_triggered();
		void on_action_Open_triggered();
		void on_action_Save_triggered();
		void on_action_SaveAs_triggered();
		void on_action_Print_triggered();
		
		void on_clbEdit_clicked();
		
		void on_clbMore_clicked();
		void on_clbLess_clicked();
		
		void on_bbSettings_clicked(QAbstractButton *b);
		
		void on_lwSnippets_currentRowChanged(int idx);
		
		void on_bNewSnippet_clicked();
		void on_bEraseSnippet_clicked();
		void on_bExitSnippets_clicked();
		
		void on_action_Settings_triggered();
		void on_action_Snippets_triggered();
		void on_action_Reload_syntax_files_triggered();
		
		void on_action_About_triggered();
		void on_action_About_Qt_triggered();
		
		void on_menu_Recent_triggered(QAction *a);
		
	private:
		bool maybeSave();
		bool maybeCommitSnippetChanges();
		
		void updateRecentFiles(const QString& filename = QString());
		
		QEditConfig *m_config;
		QFormatConfig *m_formatConfig;
		
		QCodeEdit *m_editControl;
		QFormatScheme *m_formats;
		QLanguageFactory *m_languages;
		
		QToolBar *m_edit, *m_find;
		
		int m_editedSnippet;
		QStringList m_snippetPatterns;
		SnippetManager *m_snippetManager;
};

#endif
