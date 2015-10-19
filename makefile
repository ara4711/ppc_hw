
PROC=powerpc
TYPE=eabi
PREFIX=$(PROC)-$(TYPE)-
#PATH:=/usr/local/bin:$(PATH)
CFLAGS=-Iinc

CC=$(PREFIX)gcc
AS=$(PREFIX)as
AR=$(PREFIX)ar
LD=$(PREFIX)ld
NM=$(PREFIX)nm
OBJDUMP=$(PREFIX)objdump
OBJCOPY=$(PREFIX)objcopy

QEMU=/home/ara/qemu-aug2015/qemu/ppc-softmmu/qemu-system-ppc

.PHONY: all

all : test.bin

startup.o : startup.S
	$(CC) -mbig-endian -g -c -gstabs+ -Wa,-alh=startup.lst,-L -o $@ $<

test.o : test.c
	$(CC) $(CFLAGS) -c -mcpu=8540 -g $< -o $@


test.elf : test.o startup.o  test.ld
	$(LD) -T test.ld test.o startup.o -o $@


test.bin : test.elf
	 $(OBJCOPY) -O binary  $< $@

nm_test:
	$(NM) test.elf
dis_test:
	$(OBJDUMP) --disassemble test.elf

clean :
	rm -f -v *.o *.elf *.bin *.lst

run:
	$(QEMU) -cpu e500 -d guest_errors,unimp -M ppce500 -nographic -bios test.elf  -s


debug:
	$(QEMU) -cpu e500 -d guest_errors,unimp -M ppce500 -nographic -bios test.elf  -s -S
