#---------------------------------------------------------------
# Project         : Intel Software College
# Module          : SemaphoreLab
# File            : Makefile
# Version         : $Id: Makefile,v 1.7 2006/03/28 09:34:27 fred Exp $
# Author          : Frederic Lepied
# Created On      : Wed Mar 22 08:25:25 2006
#---------------------------------------------------------------

PROGS = serial threaded 
CXX = icc
LDLIBS = -lpthread

all: $(PROGS)

serial: serial.c
	$(CXX) -o $@ $<

threaded: threaded.c
	$(CXX) -o $@ $< $(LDLIBS)

clean:
	rm -f *.o *~ $(PROGS) *.output

# Local variables:
# mode: makefile
# End:
#
# Makefile ends here
