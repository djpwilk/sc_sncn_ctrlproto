UNAME := $(shell uname)
MASTER_INSTALLDIR := /opt/etherlab
INCLUDE_DIRS = -I include
INCLUDE_DIRS += -I ../common_include/
INCLUDE_DIRS += -I $(MASTER_INSTALLDIR)/include

LIB_DIRS += -L $(MASTER_INSTALLDIR)/lib

LIBS += -lethercat

all:

ifneq "$(UNAME)" "Linux"
		@echo
		@echo "----------------------------------------------------------------------------------------------------"
		@echo "||Your System isn't Linux, so the ctrlproto libary for the Linux Ethercat Master can not be built!||"
		@echo "----------------------------------------------------------------------------------------------------"
		@echo
else
		@if test -d $(MASTER_INSTALLDIR); \
		then \
		gcc src/ctrlproto_m.c $(INCLUDE_DIRS) $(LIBS) $(LIB_DIRS) -o lib/libsncn_ctrlproto.so -shared -fPIC;\
		gcc -c src/ctrlproto_m.c $(INCLUDE_DIRS) $(LIBS) $(LIB_DIRS) -o lib/libsncn_ctrlproto.o;\
		ar  rcs lib/libsncn_ctrlproto.a lib/libsncn_ctrlproto.o;\
		rm -f lib/libsncn_ctrlproto.o;\
		else echo \
		"\n\
		-----------------------------------------------------------------------------------------------------------------\n\
		||Ethercat Master is not installed at given directory if you installed it elsewhere change location in Makefile||\n\
		-----------------------------------------------------------------------------------------------------------------\n";\
		fi
endif

clean:
	rm -f lib/libsncn_ctrlproto.so
	rm -f lib/libsncn_ctrlproto.a