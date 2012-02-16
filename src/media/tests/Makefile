MODULE	= test

EXE			= $(MODULE)

INCLUDE	+= \
					 -I../src/include \
					 -I../../menu/src/include \
					 -I../../tuner/src/include \

LIBRARY += \
					 -L../bin -lmedia \
					 -L../../tuner/bin -ltuner \
					 -L../../demux/bin -ldemux \
					 -lfusionsound \
					 -lxine \
					 -lfusion \
					 -ldirectfb \
					 -ldirect \

OBJS	= \
				test.o\

include ../../../Makefile.defs
include ../../../Makefile.build
