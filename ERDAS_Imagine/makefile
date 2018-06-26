#/*
# *  makefile
# *  LibKEA
# *
# *  Created by Sam Gillingham on 26/07/2013.
# *  Copyright 2013 LibKEA. All rights reserved.
# *
# *  This file is part of LibKEA.
# *
# *  Permission is hereby granted, free of charge, to any person 
# *  obtaining a copy of this software and associated documentation 
# *  files (the "Software"), to deal in the Software without restriction, 
# *  including without limitation the rights to use, copy, modify, 
# *  merge, publish, distribute, sublicense, and/or sell copies of the 
# *  Software, and to permit persons to whom the Software is furnished 
# *  to do so, subject to the following conditions:
# *
# *  The above copyright notice and this permission notice shall be 
# *  included in all copies or substantial portions of the Software.
# *
# *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
# *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
# *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
# *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
# *  ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
# *  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
# *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# *
# */
SHELL=/bin/sh

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CFLAGS) -c $<

CC		= CC
KEA_PATH	= /opt/osgeo/imagine32
LOCAL_INCLUDES 	= -I./
KEA_INCLUDES    = -I$(KEA_PATH)/include
ERDAS_INCLUDES 	= -I$(IMAGINE_HOME)/usr/include/
ALLOW_GLOBALS	= -DERDAS_GLOBAL_DATA=0
CFLAGS		= -xO4 -fPIC $(LOCAL_INCLUDES) $(KEA_INCLUDES) $(ERDAS_INCLUDES) \
			-D_POSIX_SOURCE -D$(ARCH) $(ALLOW_GLOBALS) 

KEA_LIBS_PATH   = $(KEA_PATH)/lib
KEA_LIBS        = -lkea -lhdf5_hl -lhdf5_cpp -lhdf5 -lz
KEA_LDFLAGS     = -L$(KEA_LIBS_PATH) $(KEA_LIBS)
ERDAS_LIBS_PATH	= $(IMAGINE_HOME)/usr/lib/$(ARCH)/
ERDAS_LIBS	= -leraster -lgnutim -lethr -lergx
ERDAS_LDFLAGS  	= -L$(ERDAS_LIBS_PATH) $(ERDAS_LIBS)
CPP_LDFLAGS 	= -lCrun -lCstd
INSTALL_DIR	= $(HOME)/.imagine$(VERSION)/RasterFormats

OBJS = keainstance.o keafile.o kealayer.o keatable.o keaproj.o

TARGETS = kea.dll

all: $(TARGETS)

kea.o: kea.h keainstance.cpp keafile.cpp kealayer.cpp keatable.cpp keaproj.cpp keaproj.h makefile

#
# mask
#

kea.dll: $(OBJS)
	$(CC) -G $(OBJS) $(KEA_LDFLAGS) $(ERDAS_LDFLAGS) $(CPP_LDFLAGS) -o $@

clean:
	rm -f *.o kea.dll

install:
	mkdir -p $(INSTALL_DIR)
	cp kea.dll $(INSTALL_DIR)
