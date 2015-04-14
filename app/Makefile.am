## Process this file with automake to produce Makefile.in

bin_PROGRAMS = machines

machines_SOURCES = \
	main.c \
	machines.c \
	machines.h \
	wavegen.c \
	wavegen.h \
	wavegennote.c \
	wavegennote.h \
	wavetable.c \
	wavetable.h \
	wavetablenote.c \
	wavetablenote.h \
	delay.c \
	delay.h \
	flanger.c \
	flanger.h \
	mixer.c \
	mixer.h \
	speaker.c \
	speaker.h \
	disk.c \
	disk.h \
	lipaconf.c \
	lipaconf.h \
	audio_oss.c \
	audio_oss.h \
	midievents.c \
	midievents.h \
	midi.h \
	util.c \
	util.h \
	getopt1.c \
	getopt.c \
	getopt.h

machines_LDADD = @GLIB_LIBS@ -lm -laudiofile -llipa

INCLUDES = @GLIB_CFLAGS@

CPPFLAGS = -I..

CFLAGS = -Wall @CFLAGS@ 

