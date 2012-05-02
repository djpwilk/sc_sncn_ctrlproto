# Defining the compiler:
CC=gcc

# Defining compiler flags:
CFLAGS=-O2 -Wall -g

# Defining linker flags:
LFLAGS=-O2 -lpthread -lm -lrt -lc -Wall -g

# Defining the object files:
sim_o   = simple_test.o \
          ethercatconfig.o \
          ethercatcoe.o \
          ethercatfoe.o \
          ethercatmain.o \
          ethercatbase.o \
          ethercatprint.o \
          ethercatsoe.o \
          nicdrv.o

red_o   = red_test.o \
          ethercatconfig.o \
          ethercatcoe.o \
          ethercatdc.o \
          ethercatmain.o \
          ethercatbase.o \
          ethercatprint.o \
          ethercatsoe.o \
          nicdrv.o

info_o   = slaveinfo.o \
          ethercatconfig.o \
          ethercatcoe.o \
          ethercatdc.o \
          ethercatmain.o \
          ethercatbase.o \
          ethercatprint.o \
          ethercatsoe.o \
          nicdrv.o

eep_o   = eepromtool.o \
          ethercatmain.o \
          ethercatbase.o \
          nicdrv.o

ebox_o   = ebox.o \
          ethercatconfig.o \
          ethercatcoe.o \
          ethercatfoe.o \
          ethercatdc.o \
          ethercatmain.o \
          ethercatbase.o \
          ethercatprint.o \
          ethercatsoe.o \
          nicdrv.o

sim_o    = simple_test.o \
          ethercatconfig.o \
          ethercatcoe.o \
          ethercatfoe.o \
          ethercatmain.o \
          ethercatbase.o \
          ethercatprint.o \
          ethercatsoe.o \
          nicdrv.o

coe_o    = coe.o \
          ethercatconfig.o \
          ethercatcoe.o \
          ethercatfoe.o \
          ethercatmain.o \
          ethercatbase.o \
          ethercatprint.o \
          ethercatsoe.o \
          nicdrv.o

voe_o   = voe.o \
          ethercatconfig.o \
          ethercatcoe.o \
          ethercatfoe.o \
          ethercatmain.o \
          ethercatbase.o \
          ethercatprint.o \
          ethercatsoe.o \
          nicdrv.o

voe-cmd_o = voe-cmd.o \
          ethercatconfig.o \
          ethercatcoe.o \
          ethercatfoe.o \
          ethercatmain.o \
          ethercatbase.o \
          ethercatprint.o \
          ethercatsoe.o \
          nicdrv.o

voe-demo_o = voe-demo.o \
          ethercatconfig.o \
          ethercatcoe.o \
          ethercatfoe.o \
          ethercatmain.o \
          ethercatbase.o \
          ethercatprint.o \
          ethercatsoe.o \
          nicdrv.o

# The default rule - compiling our main program:
all:	simple_test red_test slaveinfo eepromtool ebox coe voe voe-cmd voe-demo
	echo all: make complete

simple_test: $(sim_o)
	$(CC) $(LFLAGS) -o $@ $+

red_test: $(red_o)
	$(CC) $(LFLAGS) -o $@ $+

slaveinfo: $(info_o)
	$(CC) $(LFLAGS) -o $@ $+

eepromtool: $(eep_o)
	$(CC) $(LFLAGS) -o $@ $+

ebox: $(ebox_o)
	$(CC) $(LFLAGS) -o $@ $+

coe: $(coe_o)
	$(CC) $(LFLAGS) -o $@ $+

voe: $(voe_o)
	$(CC) $(LFLAGS) -o $@ $+

voe-cmd: $(voe-cmd_o)
	$(CC) $(LFLAGS) -o $@ $+

voe-demo: $(voe-demo_o)
	$(CC) $(LFLAGS) -o $@ $+

# Tell make how to build .o files from .c files:
%.o:%.c
	$(CC) $(CFLAGS) -c $+
	
# Removing the executable and the object files
clean: 
	rm slaveinfo simple_test red_test eepromtool ebox voe voe-cmd voe-demo *.o
	echo clean: make complete
