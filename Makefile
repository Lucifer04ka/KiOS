.PHONY: all build iso run clean

# Находим все файлы .c в папке src
C_SOURCES = $(wildcard src/*.c)
# Превращаем их в список файлов .o в папке build
OBJ = $(patsubst src/%.c, build/%.o, $(C_SOURCES))

all: build iso run

# Собираем ядро из всех файлов
build: $(OBJ)
	ld -m elf_x86_64 -T linker.ld $(OBJ) -o build/kiOS.bin --build-id=none -z max-page-size=0x1000

# Универсальное правило: как из любого .c сделать .o
build/%.o: src/%.c
	@mkdir -p build
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

clean:
	rm -rf build/*