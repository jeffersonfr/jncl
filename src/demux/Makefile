MODULE	= demux

EXE			= lib$(MODULE).so

INCLUDE	+= \
					 -I../tuner/src/include \

CFLAGS	+= -shared

OBJS 		= \
					demux.o\
					demuxbypass.o\
					demuxsection.o\
					demuxpes.o\
					demuxraw.o\
					filter.o\
					filterand.o\
					filterpes.o\
					filterraw.o\
					filtersection.o\
					packet.o\
					section.o\
					source.o\
					demuxmanager.o\

include ../../Makefile.defs
include ../../Makefile.build

