JAVAC?=javac
JAR?=jar
JAVAH?=javah
GCJ?=gcj
CC?=gcc
LD?=ld
PPFLAGS?=-C -P
CFLAGS?=-fpic -Wall -Os -pedantic -std=c99
LDFLAGS?=-fpic -shared -lc 
GCJFLAGS?=-fjni
JCFLAGS?=
INCLUDES?=-I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux

PREFIX?=/usr/local
JARDIR?=$(PREFIX)/share/java
LIBDIR?=$(PREFIX)/lib/jni

MATTVER=0.3
DEBUGVER=1.1
UNIXVER=0.2
CGIVER=0.5
IOVER=0.1
HEXVER=0.1

SRC=$(shell find cx -name '*.java' -and -not -name 'Debug.java')

all: libcgi-java.so libunix-java.so unix-$(UNIXVER).jar cgi-$(CGIVER).jar debug-enable-$(DEBUGVER).jar debug-disable-$(DEBUGVER).jar io-$(IOVER).jar hexdump-$(HEXVER).jar

classes: .classes 
.classes: $(SRC)
	mkdir -p classes
	$(JAVAC) $(JCFLAGS) -d classes -cp classes $^
	touch .classes
clean:
	rm -rf classes
	rm -f .classes .enabledebug .disabledebug *.o *.h *.so *.tar.gz *.jar *.cgi
	rm -rf libmatthew-java-$(MATTVER)

cgi-$(CGIVER).jar: .classes
	(cd classes; $(JAR) cf ../$@ cx/ath/matthew/cgi/*class)
io-$(IOVER).jar: .classes
	(cd classes; $(JAR) cf ../$@ cx/ath/matthew/io/*class)
unix-$(UNIXVER).jar: .classes
	(cd classes; $(JAR) cf ../$@ cx/ath/matthew/unix/*class)
hexdump-$(HEXVER).jar: .classes
	(cd classes; $(JAR) cf ../$@ cx/ath/matthew/utils/Hexdump.class)

%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<
lib%.so: %.o
	$(LD) $(LDFLAGS) -o $@ $<
unix-java.h: .classes
	$(JAVAH) -classpath classes -o $@ cx.ath.matthew.unix.UnixServerSocket cx.ath.matthew.unix.UnixSocket cx.ath.matthew.unix.USInputStream cx.ath.matthew.unix.USOutputStream
cgi-java.h: .classes
	$(JAVAH) -classpath classes -o $@ cx.ath.matthew.cgi.CGI

test.cgi: cgi-$(CGIVER).jar libcgi-java.so
	$(GCJ) $(GCJFLAGS) -L. -lcgi-java -o test.cgi --main=cx.ath.matthew.cgi.testcgi cgi-$(CGIVER).jar
	
libmatthew-java-$(MATTVER).tar.gz: Makefile cx cgi-java.c unix-java.c README INSTALL COPYING changelog
	mkdir -p libmatthew-java-$(MATTVER)
	cp -a $^ libmatthew-java-$(MATTVER)
	tar zcf $@ libmatthew-java-$(MATTVER)

debug-enable-$(DEBUGVER).jar: cx/ath/matthew/debug/Debug.jpp
	make .enabledebug
	(cd classes;jar cf ../$@ cx/ath/matthew/debug/*.class)
debug-disable-$(DEBUGVER).jar: cx/ath/matthew/debug/Debug.jpp
	make .disabledebug
	(cd classes;jar cf ../$@ cx/ath/matthew/debug/*.class)
.enabledebug: cx/ath/matthew/debug/Debug.jpp .classes
	cpp $(PPFLAGS) -DDEBUGSETTING=true < cx/ath/matthew/debug/Debug.jpp > cx/ath/matthew/debug/Debug.java
	$(JAVAC) $(JCFLAGS) -cp classes -d classes cx/ath/matthew/debug/Debug.java
	rm -f .disabledebug
	touch .enabledebug
.disabledebug: cx/ath/matthew/debug/Debug.jpp .classes
	cpp $(PPFLAGS) -DDEBUGSETTING=false < cx/ath/matthew/debug/Debug.jpp > cx/ath/matthew/debug/Debug.java
	$(JAVAC) $(JCFLAGS) -cp classes -d classes cx/ath/matthew/debug/Debug.java
	rm -f .enabledebug
	touch .disabledebug

install: libcgi-java.so libunix-java.so unix-$(UNIXVER).jar cgi-$(CGIVER).jar debug-enable-$(DEBUGVER).jar debug-disable-$(DEBUGVER).jar io-$(IOVER).jar hexdump-$(HEXVER).jar
	install -d $(DESTDIR)$(JARDIR)
	install -m 644 debug-enable-$(DEBUGVER).jar $(DESTDIR)$(JARDIR)
	install -m 644 debug-disable-$(DEBUGVER).jar $(DESTDIR)$(JARDIR)
	install -m 644 unix-$(UNIXVER).jar $(DESTDIR)$(JARDIR)
	install -m 644 cgi-$(CGIVER).jar $(DESTDIR)$(JARDIR)
	install -m 644 io-$(IOVER).jar $(DESTDIR)$(JARDIR)
	install -m 644 hexdump-$(HEXVER).jar $(DESTDIR)$(JARDIR)
	ln -sf debug-disable-$(DEBUGVER).jar $(DESTDIR)$(JARDIR)/debug-disable.jar
	ln -sf debug-enable-$(DEBUGVER).jar $(DESTDIR)$(JARDIR)/debug-enable.jar
	ln -sf unix-$(UNIXVER).jar $(DESTDIR)$(JARDIR)/unix.jar
	ln -sf io-$(IOVER).jar $(DESTDIR)$(JARDIR)/io.jar
	ln -sf cgi-$(CGIVER).jar $(DESTDIR)$(JARDIR)/cgi.jar
	ln -sf hexdump-$(HEXVER).jar $(DESTDIR)$(JARDIR)/hexdump.jar
	install -d $(DESTDIR)$(LIBDIR) 
	install libcgi-java.so $(DESTDIR)$(LIBDIR)
	install libunix-java.so $(DESTDIR)$(LIBDIR)

