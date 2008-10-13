
#include "qnfa.h"

#include "qformatfactory.h"

#include <QDomElement>
#include <QDomDocument>

#include <QTime>
#include <QFile>
#include <QStringList>
#include <QTextStream>

//#define _TEST_CASE_

static int matches = 0;
static const QChar *ld;
static QTextStream log;

enum HandlerAction
{
	NoAction	= 0,
	
	FormatMask	= 0x000000ff,
	ParenMask	= 0x0000ff00,
	
	Highlight	= 0x01000000,
	Indent		= 0x02000000,
	ParenOpen	= 0x04000000,
	Parenclose	= 0x08000000,
	Fold		= 0x10000000,
};

/*
class QNFAAction
{
	public:
		enum
		{
			NoAction	= 0,
			
			FormatMask	= 0x000000ff,
			ParenMask	= 0x0000ff00,
			
			Highlight	= 0x01000000,
			Indent		= 0x02000000,
			ParenOpen	= 0x04000000,
			ParenClose	= 0x08000000,
			Fold		= 0x10000000
		};
		
		inline static quint32 format(quint8 id)
		{ return id; }
		
		inline static quint32 parenthesis(quint8 id)
		{ return id << 8; }
};
*/

#include "xml2qnfa.cpp"

class MatchHandler : public QNFAMatchHandler
{
	public:
		virtual void matched(int pos, int len, int action)
		{
			matches++;
			
			#ifdef _TEST_CASE_
			qDebug("match[%i, %i] : %s",
				pos, len,
				qPrintable(QString(ld + pos, len)));
			#else
			if ( ld && (ld[pos] != '\n') )
				log << QString(ld + pos, len) << " ";
			#endif
		}
};

int main (int argc, char **argv)
{
	QTime t;
	
	printf("building syntax model...");
	fflush(stdout);
	
	t.start();
	
	QNFA *l = lexer();
	
	#if 1
	QFile xmlnfa("../../qxs/cpp.qnfa");
	
	if ( !xmlnfa.open(QFile::Text | QFile::ReadOnly) )
	{
		qWarning("[Failed]");
		return -1;
	}
	
	QDomDocument doc;
	doc.setContent(&xmlnfa);
	
	QFormatFactory formats("../../qxs/formats.xml");
	
	QHash<QString, int> pids;
	fillContext(l, doc.documentElement(), &formats, pids);
	
	#else
	addSequence(l, "(", Match);
	addSequence(l, ")", Match);
	addSequence(l, "{", Match | Indent | Fold);
	addSequence(l, "}", Match | Indent | Fold);
	addSequence(l, "\\[", Match);
	addSequence(l, "\\]", Match);
	
	addWord(l, "public", Highlight);
	addWord(l, "protected", Highlight);
	addWord(l, "private", Highlight);
	
	addWord(l, "const", Highlight);
	addWord(l, "volatile", Highlight);
	addWord(l, "extern", Highlight);
	addWord(l, "static", Highlight);
	addWord(l, "mutable", Highlight);
	addWord(l, "auto", Highlight);
	addWord(l, "register", Highlight);
	addWord(l, "inline", Highlight);
	
	addWord(l, "signed", Highlight);
	addWord(l, "unsigned", Highlight);
	addWord(l, "bool", Highlight);
	addWord(l, "char", Highlight);
	addWord(l, "int", Highlight);
	addWord(l, "long", Highlight);
	addWord(l, "float", Highlight);
	addWord(l, "double", Highlight);
	addWord(l, "enum", Highlight);
	addWord(l, "union", Highlight);
	addWord(l, "wchar_t", Highlight);
	addWord(l, "void", Highlight);
	
	addWord(l, "for", Highlight);
	addWord(l, "while", Highlight);
	addWord(l, "do", Highlight);
	addWord(l, "if", Highlight);
	addWord(l, "else", Highlight);
	addWord(l, "switch", Highlight);
	addWord(l, "case", Highlight);
	addWord(l, "default", Highlight);
	addWord(l, "break", Highlight);
	addWord(l, "continue", Highlight);
	addWord(l, "try", Highlight);
	addWord(l, "catch", Highlight);
	addWord(l, "throw", Highlight);
	addWord(l, "goto", Highlight);
	addWord(l, "foreach", Highlight);
	addWord(l, "forever", Highlight);
	
	addWord(l, "new", Highlight);
	addWord(l, "delete", Highlight);
	addWord(l, "class", Highlight);
	addWord(l, "struct", Highlight);
	addWord(l, "const_cast", Highlight);
	addWord(l, "dynamic_cast", Highlight);
	addWord(l, "static_cast", Highlight);
	addWord(l, "reinterpret_cast", Highlight);
	addWord(l, "operator", Highlight);
	addWord(l, "virtual", Highlight);
	addWord(l, "this", Highlight);
	addWord(l, "explicit", Highlight);
	addWord(l, "friend", Highlight);
	addWord(l, "namespace", Highlight);
	
	addWord(l, "asm", Highlight);
	addWord(l, "false", Highlight);
	addWord(l, "true", Highlight);
	addWord(l, "template", Highlight);
	addWord(l, "typename", Highlight);
	addWord(l, "return", Highlight);
	addWord(l, "sizeof", Highlight);
	addWord(l, "typeid", Highlight);
	addWord(l, "typedef", Highlight);
	
	QNFA *dox, *sub = context("/\\*[!\\*]", "\\*/", "", Highlight, &dox);
	addWord(dox, "\\\\class", Highlight);
	
	addNFA(l, sub);
	addNFA(l, context("\"", "\"", "", Highlight));
	addNFA(l, context("\'", "\'", "", Highlight));
	addNFA(l, context("/\\*", "\\*/", "", Highlight));
	addNFA(l, context("//", "\n", "", Highlight));
	addNFA(l, context("#" , "\n", "", Highlight));
	
	addWord(l, "-?[0123456789]*.[0123456789]+f?", Highlight);
	addWord(l, "0x[0123456789abcdefABCDEF]+", Highlight);
	addWord(l, "0[01234567]+", Highlight);
	addWord(l, "-?[0123456789]+", Highlight);
	#endif
	
	squeeze(l);
	
	printf("\t\t[done in %i ms]\n", t.elapsed());
	fflush(stdout);
	
	MatchHandler handler;
	QNFAMatchContext cxt(l);
	cxt.meaningless
		<< QLatin1Char(' ').unicode()
		<< QLatin1Char('\t').unicode()
		<< QLatin1Char(',').unicode()
		<< QLatin1Char(';').unicode()
		<< QLatin1Char('!').unicode()
		<< QLatin1Char('=').unicode()
		<< QLatin1Char('<').unicode()
		<< QLatin1Char('>').unicode()
		<< QLatin1Char('|').unicode()
		<< QLatin1Char('&').unicode()
		<< QLatin1Char('~').unicode()
		;
	
	#ifdef _TEST_CASE_
	QString test =
	"int main(int argc, char **argv)"
	"/* int rocks for ever */"
	"\"some text inside quotes\""
	"/*! \\class nested... */"
	"{ float pi = 3.141592f, doh = -.14; }";
	
	ld = test.constData();
	match(&cxt, test, &handler);
	#else
	QFile f(argv[1]), ln("match.log");
	
	if ( !f.open(QFile::ReadOnly | QFile::Text) )
		return -1;
	
	if ( !ln.open(QFile::WriteOnly | QFile::Text) )
		return -1;
	
	log.setDevice(&ln);
	
	printf("loading textfile data ...");
	fflush(stdout);
	t.restart();
	
	QTextStream s(&f);
	QString text = s.readAll();
	
	f.close();
	
	printf("\t\t[done in %i ms]\n", t.elapsed());
	fflush(stdout);
	
	const QChar *d = text.constData();
	
	int line = 0, i = 0, next, len, end = text.count();
	
	printf("processing text data ...");
	fflush(stdout);
	t.restart();
	
	while ( i < end )
	{
		line++;
		next = text.indexOf(QLatin1Char('\n'), i);
		
		ld = d + i;
		len = (next == -1) ? end - i : ++next - i;
		
		//qDebug("pos:%i; len:%i; txt:\"%s\"", i, len, qPrintable(QString(ld, len)));
		
		match(&cxt, ld, len, &handler);
		
		if ( next == -1 )
			break;
		
		log << endl;
		
		i = next;
	}
	
	printf("\t\t[done in %i ms]\n", t.elapsed());
	fflush(stdout);
	
	qDebug("%i lines processed in %i ms [%i matches]",
			line,
			t.elapsed(),
			matches);
	
	text.clear();
	ln.close();
	#endif
	
	return 0;
}


QHash<QString, int> QNFADefinition::m_paren;
QHash<QString, QNFA*> QNFADefinition::m_contexts;
//QHash<QString, QNFADefinition*> QNFADefinition::m_definitions;
QHash<QString, QNFADefinition::EmbedRequestList> QNFADefinition::m_pendingEmbeds;

void QNFADefinition::addContext(const QString& id, QNFA *nfa)
{
	//qDebug("registering context : %s", qPrintable(id));
	m_contexts[id] = nfa;
}

void QNFADefinition::flushEmbedRequests(const QString& lang)
{
	//qDebug("flushing requests...");
	QHash<QString, EmbedRequestList>::iterator it;
	
	it = m_pendingEmbeds.begin();
	
	while ( it != m_pendingEmbeds.end() )
	{
		QString r = it.key();
		
		if (
				r.startsWith(lang)
			&&
				(
					(r.count() == lang.count())
				||
					(r.at(lang.count() + 1) == ':')
				)
			)
		{
			QNFA *src = m_contexts.value(r);
			
			if ( !src )
			{
				++it;
				continue;
			}
			
			foreach( const EmbedRequest& request, *it )
			{
				//qDebug("embedding %s in 0x%x at index %i",
				//		qPrintable(r), request.target, request.index);
				
				embed(src, request.target, request.index);
			}
			
			it = m_pendingEmbeds.erase(it);
		} else {
			++it;
		}
	}
}

void QNFADefinition::addEmbedRequest(const QString& cxt, QNFA *dest)
{
	//qDebug("Adding request : %s", qPrintable(cxt));
	
	if ( m_contexts.contains(cxt) )
	{
		//qDebug("dealing with request : %s", qPrintable(cxt));
		embed(m_contexts[cxt], dest, dest->out.branch->count());
	} else {
		m_pendingEmbeds[cxt] << EmbedRequest(dest, dest->out.branch->count());
	}
}

void QNFADefinition::shareEmbedRequests(QNFA *src, QNFA *dest)
{
	QHash<QString, EmbedRequestList>::iterator it = m_pendingEmbeds.begin();
	
	while ( it != m_pendingEmbeds.end() )
	{
		foreach ( const EmbedRequest& request, *it )
		{
			if ( request.target == src )
			{
				it->append(EmbedRequest(dest, dest->out.branch->count()));
				break;
			}
		}
		
		++it;
	}
}
