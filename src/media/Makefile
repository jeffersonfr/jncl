MODULE	= media

EXE			= lib$(MODULE).so

INCLUDE	+= \
					 -I../tuner/src/include \
					 -I../demux/src/include \
					 -I/usr/local/include/directfb\
					 -I/usr/local/include/fusionsound\

CFLAGS	+= -shared

OBJS 		= \
					mediaplayer.o\
					audioplayer.o\
					datasource.o\
					demuxdatasource.o\
					playermanager.o\
					videoplayer.o\
					mediaexception.o\
					mediaplayerlistener.o\
					mediaplayerevent.o\

include ../../Makefile.defs
include ../../Makefile.build

