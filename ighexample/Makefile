
CC = gcc
LD = gcc

CFLAGS = -g -O2 -std=gnu99
LDFLAGS = -g

UNAME := $(shell uname)

WARNINGS = -Wall -pedantic

#####################################################################
##Additional Code for the igh Linux Master Application (LINUX Only!)#
######################################################################

#########################################
#Installdir to check for Ethercat Master#
#########################################
MASTER_INSTALLDIR := /opt/etherlab

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $(WARNINGS) -D_BSD_SOURCE $^ -I$(MASTER_INSTALLDIR)/include


all: ighexpl

ighexpl: main.o
	$(LD) $(WARNINGS)  $^ -lpthread -lethercat -I$(MASTER_INSTALLDIR)/include/ -L$(MASTER_INSTALLDIR)/lib/ -o $@  -static


.PHONY: clean

clean:
	@rm -f ighexpl *.o

