MODULE	= test

EXE			= $(MODULE)

INCLUDE	+= \
					 -I../src/include \
					 -I../../tuner/src/include \
					 -I../../demux/src/include \

LIBRARY	+= \
					 -L../bin -ldemux \
					 -L../../tuner/bin -ltuner \
					 -L../../media/bin -lmedia \
					 -L../../formatter/bin -lformatter \
					 -lfusionsound \
					 -lxine \

OBJS	= \
				test.o\

include ../../../Makefile.defs
include ../../../Makefile.build
