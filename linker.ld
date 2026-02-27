OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS {
    /* Современные 64-битные ядра живут высоко в памяти! */
    . = 0xffffffff80000000;

    .text : ALIGN(4K) {
        *(.text*)
    }

    .rodata : ALIGN(4K) {
        *(.rodata*)
    }

    .data : ALIGN(4K) {
        *(.data*)
    }

    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss*)
    }

    /DISCARD/ : {
        *(.note.gnu.build-id)
    }
}