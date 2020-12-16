# N64 toolchain
CC      = mips64-gcc
LD      = mips64-ld
OBJCOPY = mips64-objcopy
OBJDUMP = mips64-objdump

# default compilation flags
CFLAGS = -DNDEBUG -Wall -Wno-main -mno-gpopt -fno-toplevel-reorder -fomit-frame-pointer -G 0 -Os --std=gnu99 -mtune=vr4300 -mabi=32 -mips3 -mno-check-zero-division -mno-explicit-relocs -mno-memcpy
LDFLAGS = -L$(Z64OVL_LD) -T z64ovl.ld --emit-relocs

%.bin : %.c
	@echo -n "ENTRY_POINT = " > entry.ld
	@echo -n $(shell cat $(basename $<).c | grep ENTRY_POINT | sed -e 's/\s.*$$//') >> entry.ld
	@echo -n ";" >> entry.ld
	@$(CC) -c $< $(CFLAGS)
	@$(LD) -o $(basename $<).elf $(basename $<).o $(LDFLAGS) $(LDFILE)
	@$(OBJCOPY) -R .MIPS.abiflags -O binary $(basename $<).elf $@
	@$(OBJDUMP) -t $(basename $<).elf | grep $(basename $<)

all: $(addsuffix .bin, $(basename $(wildcard *.c)))
	mkdir -p bin
	mv *.bin *.elf *.o bin

clean:
	rm -f *.bin *.elf *.o


