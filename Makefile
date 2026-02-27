.PHONY: all build iso run clean

# Find ALL .c files recursively in src/
C_SOURCES = $(wildcard src/**/*.c) $(wildcard src/*.c)
# Convert to .o files
OBJ = $(patsubst src/%.c, build/%.o, $(C_SOURCES))

all: build iso run

# Build kernel from all files
build: $(OBJ)
	ld -m elf_x86_64 -T linker.ld $(OBJ) -o build/kiOS.bin --build-id=none -z max-page-size=0x1000

# Universal rule: how to make .o from any .c
build/%.o: src/%.c
	@mkdir -p $(dir $@)
	gcc -m64 -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -mgeneral-regs-only

iso:
	@if [ ! -d "limine" ]; then git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1; fi
	make -C limine
	mkdir -p build/iso_root/boot
	cp build/kiOS.bin build/iso_root/boot/
	cp limine.conf build/iso_root/
	cp limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin build/iso_root/
	xorriso -as mkisofs -b limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table --efi-boot limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label build/iso_root -o build/kiOS.iso
	./limine/limine bios-install build/kiOS.iso

run:
	qemu-system-x86_64 -M q35 -m 2G -cdrom build/kiOS.iso

debug:
	qemu-system-x86_64 -M q35 -m 2G -cdrom build/kiOS.iso -gdb tcp::26000 -S

clean:
	rm -rf build/*
