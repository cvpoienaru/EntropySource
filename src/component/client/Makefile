# Copyright (c) 2016, Codrin-Victor Poienaru <cvpoienaru@gmail.com>.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 
# This software is provided by the copyright holders and contributors "as is"
# and any express or implied warranties, including, but not limited to, the
# implied warranties of merchantability and fitness for a particular purpose are
# disclaimed. In no event shall the copyright holder or contributors be liable
# for any direct, indirect, incidental, special, exemplary, or consequential
# damages (including, but not limited to, procurement of substitute goods or
# services; loss of use, data, or profits; or business interruption) however
# caused and on any theory of liability, whether in contract, strict liability,
# or tort (including negligence or otherwise) arising in any way out of the use
# of this software, even if advised of the possibility of such damage.

# Binary options
ES_BIN_NAME = client
ES_BIN_PREFIX = es
ES_BIN_SRC = $(ES_SRC)/component/client
ES_BIN_OUT = $(ES_BIN)/$(ES_BIN_PREFIX)-$(ES_BIN_NAME)

# Binary source & object files
ES_SOURCES = $(ES_BIN_SRC)/client.c
ES_OBJECTS = $(ES_SOURCES:.c=.o)

# Compiler options
CC = gcc
CFLAGS = -Wall -O3 $(ES_FLAGS)
LFLAGS = `pkg-config --cflags --libs glib-2.0` -lesglobal -lescommunication

all: $(ES_SOURCES) $(ES_BIN_OUT)

$(ES_BIN_OUT): $(ES_OBJECTS)
	$(CC) $^ -o $@ $(LFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(LFLAGS)

.PHONY: clean
clean:
	rm $(ES_BIN_SRC)/*.o
