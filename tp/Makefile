CC=gcc
CFLAGS=-g -Wall -DTEACHER=$(TEACHER)
LDFLAGS=-g -DTEACHER=$(TEACHER) -lpthread
TEACHER=false
PROG=runapsvr

PRESOURCES=\
periodictasks.c\
backgroundserver.c\
pollingserver.c\
deferredserver.c\
sporadicserver.c\


SOURCES = \
aperiodicservers.c\
aperiodictasks.c\
backgroundserver.c\
pollingserver.c\
deferredserver.c\
sporadicserver.c\
events.c\
runapsvr.c\
scenarii.c\
tasks.c\

OBJECTS = \
aperiodicservers.o\
aperiodictasks.o\
backgroundserver.o\
pollingserver.o\
deferredserver.o\
sporadicserver.o\
events.o\
periodictasks.o\
runapsvr.o\
scenarii.o\
tasks.o\

%.c: %.p.c
	awk -f presources.awk -v TEACHER=$(TEACHER) $< >$@

.c.o:
	$(CC) -c $(CFLAGS) $<

all : $(PROG)

clean : 
	$(RM) $(OBJECTS) *~

veryclean: clean
	$(RM) $(PRESOURCES)

$(PROG) : $(OBJECTS) $(PRESOURCES)
	$(CC) $(LDFLAGS) -o $(PROG) $(OBJECTS)

deps: $(SOURCES)
	$(CC) -M $(SOURCES) >deps

local-install : 
	make veryclean
	make TEACHER=false
	tar zcf src.tar.gz `cat MANIFEST`

-include deps
