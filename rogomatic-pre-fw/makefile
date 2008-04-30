# makefile: Rog-O-Matic XIV (CMU) Wed Feb  6 18:34:24 1985 - mlm
# Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
#
BINARIES=   rogomatic player rgmplot datesub histplot gene
BINDIR=     /usr/mlm/bin/
PUBDIR=     /usr/mlm/rgm/src14a
CCFLAGS=    -g -O
LDFLAGS=    
PLAYER_OBJS=arms.o command.o database.o debug.o explore.o io.o learn.o \
	    ltm.o main.o mess.o monsters.o pack.o rand.o replay.o rooms.o \
	    scorefile.o search.o stats.o strategy.o survival.o tactics.o \
	    things.o titlepage.o utility.o worth.o \
	    getroguetoken.o debuglog.o
SRCS=	    arms.c command.c database.c debug.c explore.c io.c learn.c \
	    ltm.c main.c mess.c monsters.c pack.c rand.c replay.c rooms.c \
	    scorefile.c search.c stats.c strategy.c survival.c tactics.c \
	    things.c titlepage.c utility.c worth.c \
	    getroguetoken.c debuglog.c
HDRS=	    types.h globals.h install.h termtokens.h
OTHERS=     setup.c findscore.c datesub.l histplot.c rgmplot.c gene.c\
	    rplot Bugreport
ROGO_OBJS=findscore.o \
		setup.o \
		scorefile.o \
		utility.o

#
# The following commands are declared:
#
all: $(BINARIES)
	echo -n "" >/dev/tty
#
#
# General makefile stuff:
#
arms.o: arms.c types.h globals.h
	cc -c $(CCFLAGS) arms.c
command.o: command.c types.h globals.h
	cc -c $(CCFLAGS) command.c
database.o: database.c types.h globals.h
	cc -c $(CCFLAGS) database.c
datesub.c: datesub.l
	lex datesub.l
	mv lex.yy.c datesub.c
datesub.o: datesub.c
	cc -c $(CCFLAGS) datesub.c
datesub: datesub.o
	cc $(LDFLAGS) -o datesub datesub.o
debuglog.o: debuglog.c
	cc -c $(CCFLAGS) debuglog.c
debug.o: debug.c types.h globals.h install.h
	cc -c $(CCFLAGS) debug.c
explore.o: explore.c types.h globals.h
	cc -c $(CCFLAGS) explore.c
findscore.o: findscore.c install.h
	cc -c $(CCFLAGS) findscore.c
gene: gene.c rand.o learn.o stats.o utility.o types.h install.h
	cc $(CCFLAGS) $(LDFLAGS) -o gene gene.c \
		rand.o learn.o stats.o utility.o -lm
getroguetoken: getroguetoken.c types.h termtokens.h
	cc $(CCFLAGS) getroguetoken.c
histplot: histplot.o utility.o
	cc $(LDFLAGS) -o histplot histplot.o utility.o
histplot.o:
	cc -c histplot.c
io.o: io.c types.h globals.h install.h termtokens.h
	cc -c $(CCFLAGS) io.c
mess.o: mess.c types.h globals.h
	cc -c $(CCFLAGS) mess.c
learn.o: learn.c types.h  install.h
	cc -c $(CCFLAGS) learn.c
ltm.o: ltm.c types.h globals.h install.h
	cc -c $(CCFLAGS) ltm.c
main.o: main.c install.h termtokens.h types.h globals.h
	cc -c $(CCFLAGS) main.c
monsters.o: monsters.c types.h globals.h
	cc -c $(CCFLAGS) monsters.c
pack.o: pack.c types.h globals.h
	cc -c $(CCFLAGS) pack.c
player: $(PLAYER_OBJS)
#	cc $(LDFLAGS) -o player $(PLAYER_OBJS) -lm -lcurses -ltermcap
	cc $(LDFLAGS) -o player $(PLAYER_OBJS) -lm -lcurses
	size player
rand.o: rand.c
	cc -c $(CCFLAGS) rand.c
replay.o: replay.c types.h globals.h
	cc -c $(CCFLAGS) replay.c
rgmplot.o: rgmplot.c
	cc -c $(CCFLAGS) rgmplot.c
rogomatic.o: rogomatic.c install.h
	cc -c $(CCFLAGS) rogomatic.c
rgmplot: rgmplot.o utility.o
	cc $(LDFLAGS) -o rgmplot rgmplot.o utility.o
rogo: rogomatic.o findscore.o scorefile.o utility.o
	cc $(LDFLAGS) -o rogo rogomatic.o findscore.o scorefile.o utility.o
	size rogomatic
rogomatic: setup.o findscore.o scorefile.o utility.o
	cc $(LDFLAGS) -o rogomatic setup.o findscore.o scorefile.o utility.o
	size rogomatic
rooms.o: rooms.c types.h globals.h
	cc -c $(CCFLAGS) rooms.c
scorefile.o: scorefile.c types.h globals.h install.h
	cc -c $(CCFLAGS) scorefile.c
search.o: search.c types.h globals.h
	cc -c $(CCFLAGS) search.c
setup.o: setup.c install.h
	cc -c $(CCFLAGS) setup.c
stats.o: stats.c types.h
	cc -c $(CCFLAGS) stats.c
strategy.o: strategy.c types.h globals.h install.h
	cc -c $(CCFLAGS) strategy.c
survival.o: survival.c types.h globals.h
	cc -c $(CCFLAGS) survival.c
tactics.o: tactics.c types.h globals.h install.h
	cc -c $(CCFLAGS) tactics.c
testfind: testfind.o findscore.o utility.o
	cc $(LDFLAGS) -o testfind testfind.o findscore.o utility.o
things.o: things.c types.h globals.h
	cc -c $(CCFLAGS) things.c
titlepage.o: titlepage.c
	cc -c $(CCFLAGS) titlepage.c
titler.o: titler.c
	cc -c titler.c
utility.o: utility.c install.h
	cc -c $(CCFLAGS) utility.c
worth.o: worth.c types.h globals.h
	cc -c $(CCFLAGS) worth.c
#
# Miscellaneous useful pseduo-makes
#
backup:
	rm -f backup.tar
	tar cvf backup.tar *.c *.h *.l rogomatic.6 makefile
	chmod ugo-w backup.tar
clean:
	rm -f *.CKP *.o datesub.c core
	strip $(BINARIES)
install:
	rm -f $(BINDIR)/player
	ln player $(BINDIR)/player
	rm -f $(BINDIR)/rogomatic
	ln rogomatic $(BINDIR)/rogomatic
titler: titler.c
#	cc -o titler titler.c -lcurses -ltermcap
	cc -o titler titler.c -lcurses
anim: anim.c utility.o
#	cc -o anim anim.c utility.o -lcurses -ltermcap
	cc -o anim anim.c utility.o -lcurses
index: $(SRCS)
	ctags -x $(SRCS) > index
fluff: $(SRCS)
	lint *.c	| grep -v 'variable # of args' \
			| grep -v 'unused in function' \
			| grep -v 'used inconsistently' \
			| grep -v 'declared inconsistently' \
			| grep -v 'multiply declared' \
			> fluff
print: $(SRCS) $(HDRS)
	@echo $? > printit
dist: $(SRCS) $(HDRS) $(OTHERS) makefile rogomatic.6 README
	rm -rf $(PUBDIR)	
	mkdir $(PUBDIR)
	cp $(SRCS) $(HDRS) $(OTHERS) makefile rogomatic.6 README $(PUBDIR)
	chmod 0444 $(PUBDIR)/*
	du $(PUBDIR)

genetest: genetest.o learn.o rand.o stats.o utility.o types.h
	cc -g -o genetest genetest.o learn.o rand.o stats.o utility.o -lm

gplot: gplot.c
	cc -g -o gplot gplot.c -lm
