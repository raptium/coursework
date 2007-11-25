######################################################################## 
# GNU Makefile demonstrating combination of C and assembly source files 
# File Name: makefile 
# All targets in the makefile are processed sequentially by SDCC 
# To create the file ‘file.hex’ using GNU make, just execute ‘make’ 
######################################################################## 
# sdcc PATH
CPATH := sdcc/bin/
# The following lines defines additional directories to search for include files 
INCLUDES := -I"./"
# The following line defines flags given to the SDCC C compiler 
CFLAGS := -c $(INCLUDES) 
# The following line defines flags given to the SDCC linker 
# Non-specific: –code-loc 0x6000 –xram-loc 0xB000 –model-large 
#LFLAGS := --code-loc 0x6000 --code-size 0x5000 --xram-loc 0xB000 --xram-size 0x3000 --model-large 
# The following line specifies the default target(s) to build 
all: vos.ihx 
# The following line specifies the object files that are to be linked together 
OBJECTS := vos.rel lcd.rel
# The following lines define a rule that sends the object files through the linker to 
# create file.ihx which then has to be processed by packihx to create file.hex 
vos.ihx : $(OBJECTS)
	$(CPATH)sdcc $(LFLAGS) $^
# The following rule sends each C file through the preprocessor and creates the asm file 
# that is then assembled to create the rel file. 
%.rel : %.c
	$(CPATH)sdcc $(CFLAGS) $<
# The following rule sends each asm file (Not the asm files created by SDCC as an 
# intermediate output of the compilation process.) through the assembler to create a rel 
# file. 
%.rel : %.asm 
	$(CPATH)asx8051 –losa $< 
# The following rule will clean all the derived objects from your directory.  This will 
# save you from accidentally tying ‘rm *’ if you are developing on a UNIX platform. 
# Note: To execute this rule type ‘make clean’ 
clean: 
	rm *.rel *.lst *.rst *.hex *.ihx *.orig *.asm *.map *.lnk *.sym *.mem