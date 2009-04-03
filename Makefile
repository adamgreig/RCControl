# Makefile for RCControl
#  Adam Greig, March 09

#INCLUDE = -I/usr/include/
#LIBDIR  = -L/usr/X11R6/lib

# Release build: optimise (use -g for debug build)
COMPILERFLAGS = -Wall -O4 -DLINUX
CC = g++
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
#LIBRARIES = -lX11 -lXi -lXmu -lglut -lGL -lGLU -lm

# All objects that need compiling, add to the end of this if a new class is added
OBJ = RCControl.o file_logger.o mftech_receiver.o pololu_servo_controller.o serial_port.o gps_receiver.o

# Final executable name
EXEC = rccontrol

# Default build
all: $(EXEC)

# Executable depends on all the objects being built first
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBDIR) $(LIBRARIES)

# Each object is built from its .cpp file
$(OBJ):
	$(CC) $(CFLAGS) -c -o $@ $(basename $@).cpp

clean:
	rm *.o rccontrol

# Each object file depends on its own cpp and h file, and also any other .h files it includes
#  Add a line here for any new classes that are added

RCControl.o: RCControl.cpp pololu_servo_controller.h mftech_receiver.h file_logger.h
pololu_servo_controller.o: pololu_servo_controller.h pololu_servo_controller.cpp serial_port.h
file_logger.o: file_logger.h file_logger.cpp
mftech_receiver.o: mftech_receiver.h mftech_receiver.cpp
gps_receiver.o: gps_receiver.h gps_receiver.cpp serial_port.h
serial_port.o: serial_port.h serial_port.cpp ports.h

