# $Id$ 
# This should be a pretty standard makefile for all PG projects


# What do you want to call the executable?
TARGET=Cassandra

# Uncomment for ATLAS neural net. Make sure similar lines are uncommented
# in pg library makefile
#ATLASCFLAGS=-DHAVE_ATLAS
#ATLASLIBSPATH=${HOME}/no.backups/ATLAS/lib/Linux_P4SSE2/
#ATLASLIBSPATH=-L$(HOME)/src/ATLAS/lib/OSX_PPCG4AltiVec
#ATLASLIBS=-lATLAS_lapack -lcblas -latlas

# You need to link clapack if you used
# HAVE_CLAPACK in the pg makefile. These
#CLAPACKLIBS=-lclapack -lF77 -lI77 -lcblaswr

PGPATH=../../

# Turning on NDEBUG will make uBlas go *much* faster, but turns off all
# your asserts. Use proper if () { throw runtime_error() } calls for errors.
#OPTIMS=-O3 -DNDEBUG
DEBUG=-g

# This will enable options processing.
PERL=yes

############################################################################
# Editing below this line not advised unless you know what's going on.

PGLIBPATH=-L$(PGPATH)
PGLIB=$(PGPATH)/libpg.a
LIBS=-lpg $(CLAPACKLIBS) $(ATLASLIBS) 
CFLAGS=-Wall $(DEBUG) $(OPTIMS) $(ATLASCFLAGS) -I $(PGPATH) -I ${HOME}/include 
LFLAGS=$(OPTIMS) $(PGLIBPATH) $(ATLASLIBSPATH)
CCDEP=$(CXX) -MM
MAKEOPTS=./makeOptionsFromDefines
OPTIONS_HEADER=options.hh
CXX=g++
OBJS=$(patsubst %.cc,%.o,$(wildcard *.cc))

.PHONY: clean distclean $(PGLIB)


$(TARGET) : $(OBJS) $(PGLIB)
	$(CXX) $(LFLAGS) -o $@ $(OBJS) $(LIBS)

$(PGLIB) :
	make -C $(PGPATH)

ifeq ($(PERL), yes)
Ps.hh Ps.cc:  $(OPTIONS_HEADER) $(MAKEOPTS).cc.template $(MAKEOPTS).hh.template $(MAKEOPTS).perl 
	$(MAKEOPTS).perl Ps $(OPTIONS_HEADER)
endif


%.o : %.cc Ps.hh
	$(CXX) $(CFLAGS) -c $<


clean:
	rm -fr *.o *~ *.bak

distclean: clean
	rm -fr $(TARGET)

# All this stuff looks after automatic header dependencies

%.d: %.cc 
	@echo "Finding dependencies for $<"
	@$(SHELL) -ec '$(CCDEP) $(CFLAGS) $(INCLUDES) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o  $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'




-include $(OBJS:%.o=%.d)
