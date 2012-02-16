MODULE	= test

EXE			= $(MODULE)

INCLUDE	+= \
					 -I../src/include \

LIBRARY	+= \
					 -L../bin -ltuner \
					 -L../../media/bin -lmedia \
					 -L../../demux/bin -ldemux \
					 -L../../formatter/bin -lformatter \
					 -lfusionsound \
					 -lxine \

OBJS	= \
				test.o\

include ../../../Makefile.defs
include ../../../Makefile.build
