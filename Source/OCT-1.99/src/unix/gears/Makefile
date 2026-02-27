#
# makefile - For building Objective-C 4.0 demonstration application for SUN
#          - (c) Stepstone 1990. All rights reserved.
#          - Andy Novobilski
#          - December 14, 1988

.SUFFIXES:
.SUFFIXES: .o .m .c

SUNLIBS= -lplot

OBJS = BBObject.o StdPlotter.o SimPlotter.o Wheel.o Clock.o GearModel.o

OCFLAGS = 

simDemo:	$(OBJS) simMain.o
	objcc -o simDemo \
		simMain.o $(OBJS) \
		$(SUNLIBS)	

#general rule for making .m files
.m.o:	;objcc -c $(OCFLAGS) $*.m
.c.o:	;cc -c $*.c
