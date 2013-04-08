UNAME := $(shell uname)
SOURCE_DIRS = src

MASTER_INSTALLDIR := /opt/etherlab

SOURCES += src/main.c

INCLUDE_DIRS += -I ../common_cmd 
INCLUDE_DIRS += -I $(MASTER_INSTALLDIR)/include
INCLUDE_DIRS += -I ../linux_library_ctrlproto/include

LIB_DIRS += -L $(MASTER_INSTALLDIR)/lib
LIB_DIRS += -L ../linux_library_ctrlproto/lib

LIBS += -lsncn_ctrlproto
LIBS += -lethercat

FLAGS = -static


all:
		@cd ../linux_library_ctrlproto; make
ifneq "$(UNAME)" "Linux"
		@echo
		@echo "------------------------------------------------------------------------------------------------------"
		@echo "||Your System isn't Linux, so the Master Application for the Linux Ethercat Master can not be built.||"
		@echo "------------------------------------------------------------------------------------------------------"
		@echo
else
		@if test -d $(MASTER_INSTALLDIR); \
		then \
		mkdir bin -p;\
		gcc $(SOURCES) $(LIBS) $(INCLUDE_DIRS) $(LIB_DIRS) $(FLAGS) -o bin/linux_ctrlproto_ecmaster_example ; \
		else echo \
		"\n\
		-----------------------------------------------------------------------------------------------------------------\n\
		||Ethercat Master is not installed at given directory if you installed it elsewhere change location in Makefile||\n\
		-----------------------------------------------------------------------------------------------------------------\n";\
		fi
endif

##Add clean Master Application
clean: masterAppClean
masterAppClean:
	rm -f bin/linux_ctrlproto_ecmaster_example