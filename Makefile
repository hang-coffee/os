CC = i686-elf-gcc
LD = i686-elf-ld
NASM = nasm
CFLAGS = -std=gnu11 -ffreestanding -O2 -Wall -Wextra -fno-stack-protector -fno-pic
ASFLAGS = -f elf32 -g -F dwarf
BUILD = build
TARGET = $(BUILD)/congestus.elf
ISO = $(BUILD)/congestus.iso
ISODIR = $(BUILD)/iso

SRCS_C := $(shell find . -name '*.c' -not -path './$(BUILD)/*' -not -path './user/*')
SRCS_ASM := $(shell find . -name '*.asm' -not -path './$(BUILD)/*' -not -path './user/*')
SRCS_C := $(patsubst ./%,%,$(SRCS_C))
SRCS_ASM := $(patsubst ./%,%,$(SRCS_ASM))
OBJS := $(addprefix $(BUILD)/,$(SRCS_C:.c=.o)) $(addprefix $(BUILD)/,$(SRCS_ASM:.asm=.o))
DEPS := $(OBJS:.o=.d)

USER_EMBED := $(BUILD)/user/hello_embed.o

.PHONY: all iso run debug clean user

all: $(TARGET)

$(TARGET): $(OBJS) $(USER_EMBED) linker.ld
	@mkdir -p $(@D)
	$(CC) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $(OBJS) $(USER_EMBED) -lgcc

$(BUILD)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/%.o: %.asm
	@mkdir -p $(@D)
	$(NASM) $(ASFLAGS) $< -o $@

# --- 用户程序 ---

$(BUILD)/user/crt0.o: user/crt0.asm
	@mkdir -p $(@D)
	$(NASM) $(ASFLAGS) $< -o $@

$(BUILD)/user/hello.o: user/hello.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/user/hello: $(BUILD)/user/crt0.o $(BUILD)/user/hello.o user/linker.ld
	$(LD) -T user/linker.ld -o $@ $(BUILD)/user/crt0.o $(BUILD)/user/hello.o

$(BUILD)/user/hello_embed.o: $(BUILD)/user/hello
	objcopy -I binary -O elf32-i386 -B i386 $< $@

user: $(USER_EMBED)

# --- ISO ---

$(ISO): $(TARGET) grub.cfg
	@mkdir -p $(ISODIR)/boot/grub
	cp $(TARGET) $(ISODIR)/boot/os.elf
	cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(ISODIR)

iso: $(ISO)

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio -m 128 -no-reboot -no-shutdown

debug: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio -m 128 -s -S -no-reboot -no-shutdown -d int

clean:
	rm -rf $(BUILD)

-include $(DEPS)