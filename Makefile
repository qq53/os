ENTRYPOINT = 0x30400

ENTRYOFFSET = 0x400

ASM = nasm
DASM = ndisasm
CC = gcc
LD = ld
ASMBFLAGS = -I boot/include/
ASMKFLAGS = -I include/ -f elf32
CFLAGS = -I include/ -c -m32 -fno-builtin
LDFLAGS = -s -m elf_i386 -Ttext $(ENTRYPOINT)
DASMFLAGS = -u -o $(ENTRYPOINT) -e $(ENTRYOFFSET)

SENBOOT = boot/boot.bin boot/loader.bin
SENKERNEL = kernel.bin
OBJS = kernel/kernel.o kernel/start.o lib/kliba.o lib/string.o lib/misc.o kernel/i8259a.o \
	   		kernel/protect.o kernel/global.o lib/klib.o kernel/main.o kernel/clock.o \
			kernel/syscall.o kernel/proc.o kernel/keyboard.o kernel/tty.o kernel/console.o \
			kernel/printf.o kernel/vsprintf.o kernel/systask.o
DASMOUTPUT = kernel.bin.asm

IMG = a.img
FLOPPY = /mnt/floppy/

.PHONY : everything buildimg clean disasm

everything : $(SENBOOT) $(SENKERNEL)

clean : 
	rm -f $(OBJS) $(SENBOOT) $(SENKERNEL)
	rm -f $(DASMOUTPUT)

disasm:
	$(DASM) $(DASMFLAGS) $(SENKERNEL) > $(DASMOUTPUT)

buildimg : everything
	dd if=boot/boot.bin of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) $(FLOPPY)
	sudo cp -fv boot/loader.bin $(FLOPPY)
	sudo cp -fv $(SENKERNEL) $(FLOPPY)
	sleep 1
	sudo umount $(FLOPPY)
	
boot/boot.bin : boot/boot.asm boot/include/fat12hdr.inc
	$(ASM) $(ASMBFLAGS) $< -o $@

boot/loader.bin : boot/loader.asm boot/include/fat.inc boot/include/fat12hdr.inc \
		boot/include/pm.inc boot/include/lib.inc
	$(ASM) $(ASMBFLAGS) $< -o $@

kernel/kernel.o : kernel/kernel.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/start.o : kernel/start.c include/type.h include/const.h include/protect.h include/proto.h \
		include/string.h include/global.h include/proc.h
	$(CC) $(CFLAGS) $< -o $@ 

lib/kliba.o : lib/kliba.asm
	$(ASM) $(ASMKFLAGS) $< -o $@

lib/string.o : lib/string.asm
	$(ASM) $(ASMKFLAGS) $< -o $@

lib/misc.o : lib/misc.c
	$(CC) $(CFLAGS) $< -o $@ 

lib/klib.o : lib/klib.c
	$(CC) $(CFLAGS) $< -o $@ 

kernel/i8259a.o : kernel/i8259a.c include/protect.h include/proto.h include/const.h \
		include/type.h
	$(CC) $(CFLAGS) $< -o $@ 

kernel/protect.o : kernel/protect.c include/protect.h include/proto.h include/const.h \
		include/type.h include/global.h include/proc.h
	$(CC) $(CFLAGS) $< -o $@

kernel/global.o : kernel/global.c
	$(CC) $(CFLAGS) $< -o $@

kernel/klib.o : kernel/klib.c
	$(CC) $(CFLAGS) $< -o $@

kernel/main.o : kernel/main.c include/type.h include/const.h include/protect.h include/proto.h \
		include/string.h include/global.h include/proc.h
	$(CC) $(CFLAGS) $< -o $@ 

kernel/clock.o : kernel/clock.c include/type.h include/const.h include/protect.h include/proto.h \
		include/string.h include/global.h include/proc.h
	$(CC) $(CFLAGS) $< -o $@ 

kernel/syscall.o : kernel/syscall.asm
	$(ASM) $(ASMKFLAGS) $< -o $@

kernel/proc.o : kernel/proc.c include/type.h include/const.h include/protect.h include/proto.h \
		include/string.h include/global.h include/proc.h
	$(CC) $(CFLAGS) $< -o $@ 

kernel/keyboard.o : kernel/keyboard.c include/type.h include/const.h include/protect.h include/proto.h \
		include/string.h include/global.h include/proc.h include/keyboard.h include/keymap.h
	$(CC) $(CFLAGS) $< -o $@ 

kernel/tty.o : kernel/tty.c include/type.h include/const.h include/protect.h include/proto.h \
		include/string.h include/global.h include/proc.h
	$(CC) $(CFLAGS) $< -o $@ 

kernel/console.o : kernel/console.c include/type.h include/const.h include/protect.h include/proto.h \
		include/string.h include/global.h include/proc.h include/tty.h include/console.h
	$(CC) $(CFLAGS) $< -o $@ 

kernel/printf.o : kernel/printf.c include/type.h include/const.h
	$(CC) $(CFLAGS) $< -o $@ 

kernel/vsprintf.o : kernel/vsprintf.c include/type.h include/const.h include/string.h
	$(CC) $(CFLAGS) $< -o $@ 

kernel/systask.o : kernel/systask.c
	$(CC) $(CFLAGS) $< -o $@ 

$(SENKERNEL) : $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(SENKERNEL)
