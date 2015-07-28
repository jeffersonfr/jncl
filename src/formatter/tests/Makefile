MODULE	= test

EXE			= $(MODULE)

INCLUDE	+= \
					 -I../src/include \

LIBRARY	= \
					-L../bin -lformatter \
					-L../../media/bin -lmedia \
					-L../../demux/bin -ldemux \
					-L../../tuner/bin -ltuner \
					/usr/local/lib/liblua.a \
					-lxine \
					-lssl \

OBJS	= \
				test.o\

include ../../../Makefile.defs
include ../../../Makefile.build
