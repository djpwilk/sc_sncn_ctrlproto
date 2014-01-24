UNAME := $(shell uname)
MASTER_INSTALLDIR := /opt/etherlab
INCLUDE_DIRS = -I include
INCLUDE_DIRS += -I ../app_linux_ctrlproto-master-example/config/motor
INCLUDE_DIRS += -I ../inc_ctrlproto-common/
INCLUDE_DIRS += -I ../../sc_sncn_ethercat/module_ethercat/src/canod/
INCLUDE_DIRS += -I $(MASTER_INSTALLDIR)/include
LIB_DIRS += -L $(MASTER_INSTALLDIR)/lib

LIBS += -lethercat -lm

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
		mkdir lib -p;\
		gcc src/ctrlproto_m.c  $(INCLUDE_DIRS) $(LIBS) $(LIB_DIRS) -o lib/libsncn_ctrlproto.so -lstdc++ -shared -fPIC;\
		gcc -c src/ctrlproto_m.c  $(INCLUDE_DIRS) $(LIBS) $(LIB_DIRS) -o lib/libsncn_ctrlproto.o;\
		ar rcs lib/libsncn_ctrlproto.a lib/*.o;\
		rm -f lib/*.o;\
		rm -f lib/*.so;\
		else echo \
		"\n\
		-----------------------------------------------------------------------------------------------------------------\n\
		||Ethercat Master is not installed at given directory if you installed it elsewhere change location in Makefile||\n\
		-----------------------------------------------------------------------------------------------------------------\n";\
		fi
endif

clean:
	rm -f lib/*.so
	rm -f lib/*.a