
#include "window.h"

#include <QApplication>

void MessageHandler(QtMsgType t, const char *msg)
{
	static const char *type[] =
	{
		"",
		"QCE::Warning  : ",
		"QCE::Critical : ",
		"QCE::Fatal    : "
	};
	
	fprintf(stderr, "%s%s\n", type[t], msg);
	fprintf(stdout, "%s%s\n", type[t], msg);
	
	if ( t & 0x02 )
	{
		fprintf(stderr, "aborting...\n");
		fprintf(stdout, "aborting...\n");
		
		#if defined(Q_OS_UNIX) && defined(_EDYUK_DEBUG_)
		abort(); // trap; generates core dump
		#else
		exit(-1); // goodbye cruel world
		#endif
	}
}

int main (int argc, char **argv)
{
	QApplication app(argc, argv);
	
	qInstallMsgHandler(MessageHandler);
	
	Window window;
	
	if ( argc > 1 )
		window.load(argv[argc - 1]);
	
	window.show();
	
	int ret = app.exec();
	
	return ret;
}
