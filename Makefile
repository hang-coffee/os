CC = i686-elf-gcc
LD = i686-elf-ld
NASM = nasm
CFLAGS = -std=gnu11 -ffreestanding -O2 -Wall -Wextra -fno-stack-protector -fno-pic
ASFLAGS = -f elf32 -g -F dwarf
BUILD = build
TARGET = $(BUILD)/os.elf
ISO = $(BUILD)/os.iso
ISODIR = $(BUILD)/iso

SRCS_C := $(shell find . -name '*.c' -not -path './$(BUILD)/*')
SRCS_ASM := $(shell find . -name '*.asm' -not -path './$(BUILD)/*')
SRCS_C := $(patsubst ./%,%,$(SRCS_C))
SRCS_ASM := $(patsubst ./%,%,$(SRCS_ASM))
OBJS := $(addprefix $(BUILD)/,$(SRCS_C:.c=.o)) $(addprefix $(BUILD)/,$(SRCS_ASM:.asm=.o))
DEPS := $(OBJS:.o=.d)

.PHONY: all iso run debug clean

all: $(TARGET)

$(TARGET): $(OBJS) linker.ld
	@mkdir -p $(@D)
	$(CC) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $(OBJS) -lgcc

$(BUILD)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/%.o: %.asm
	@mkdir -p $(@D)
	$(NASM) $(ASFLAGS) $< -o $@

$(ISO): $(TARGET) grub.cfg
	@mkdir -p $(ISODIR)/boot/grub
	cp $(TARGET) $(ISODIR)/boot/os.elf
	cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(ISODIR)

iso: $(ISO)

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio -m 128 -no-reboot -no-shutdown

debug: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio -m 128 -s -S -no-reboot -no-shutdown

clean:
	rm -rf $(BUILD)

-include $(DEPS)
