.section .multiboot
.align 8

/* Multiboot2 header */
.set MB2_MAGIC, 0xE85250D6
.set MB2_ARCH,  0
.set MB2_HDRLEN, (mb2_header_end - mb2_header)

mb2_header:
    .long MB2_MAGIC
    .long MB2_ARCH
    .long MB2_HDRLEN
    .long -(MB2_MAGIC + MB2_ARCH + MB2_HDRLEN)

    /* End tag */
    .align 8
    .short 0
    .short 0
    .long 8
mb2_header_end:

.section .text
.global _start
.type _start, @function

_start:
    cli
    mov $stack_top, %esp

    /* Call C entry. Multiboot places a magic value in EAX and info pointer in EBX.
       We currently ignore those; once you add memory maps/etc, pass them to C. */
    call kmain

halt:
    hlt
    jmp halt

.section .bss
.align 16
stack_bottom:
    .skip 16384
stack_top:
