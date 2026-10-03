CC      = i686-elf-gcc
LD      = i686-elf-ld
NASM    = nasm
OBJCOPY = objcopy

BUILD   = ../build/user

CFLAGS  = -ffreestanding -nostdlib -O2 -m32 -fno-pic -fno-stack-protector
ASFLAGS = -f elf32

all: $(BUILD)/hello_embed.o

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/crt0.o: crt0.asm | $(BUILD)
	$(NASM) $(ASFLAGS) $< -o $@

$(BUILD)/hello.o: hello.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/hello: $(BUILD)/crt0.o $(BUILD)/hello.o linker.ld
	$(LD) -T linker.ld -o $@ $(BUILD)/crt0.o $(BUILD)/hello.o

$(BUILD)/hello_embed.o: $(BUILD)/hello
	$(OBJCOPY) -I binary -O elf32-i386 -B i386 $< $@

clean:
	rm -f $(BUILD)/crt0.o $(BUILD)/hello.o $(BUILD)/hello $(BUILD)/hello_embed.o

.PHONY: all clean