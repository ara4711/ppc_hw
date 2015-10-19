# ppc_hw
helloworld powerpc-app on qemu-e500


This is a minimal program that prints to the serial port.
A real program will have to do a lot more configuration.

The program is based on a stripped version of the u-boot code,
and the three header files in the inc folder have been copied from
u-boot. A little hack was be done in processor.h (see AAA_HACK_DISABLE)
to avoid having to include even more.

At startup, tlb1, entry0 is mapped and code is executed from it.
However, it is only 4kB so in real use this code will have to 
map more code into memory. Since entry0 is in use, entry1 is used
for the ccsrbar mapping. 

You need to have a toolchain to cross-compile to ppc
and an appropriate qemu-version. Instructions for these
are forthcoming.


files:
startup.S  -- sets the stackpointer and calls c_entry
test.c     -- implements c_entry which maps the ccsrbar to
              be able to access the serial port.
	      Then it prints a message to the serial port.


Usage:
 make       -- compiles and links it.
 make run   -- let qemu run the compiled binary
 make debug -- start qemu, waiting for a gdb-connection.

 attaching with gdb (inside emacs) --
 	  /home/ara/gnutools-bin/powerpc-eabi/bin/powerpc-eabi-gdb -i=mi /home/ara/u-boot/u-boot
 	  target remote localhost:1234


