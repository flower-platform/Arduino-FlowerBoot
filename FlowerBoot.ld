MEMORY
{
  /* First 8KB (0x2000) are used by the bootloader */
  FLOWER_BOOT (rx) : ORIGIN = 0x00000000 + 0x2000, LENGTH = 0x200
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00008000
}

ENTRY(flowerBoot)

SECTIONS
{
	.flower_boot : 
	{
		KEEP(*(.flower_boot_isr_vector)) /* 64 bytes */
		KEEP(*(.flower_boot_version))	/* address 0x40 */
		KEEP(*(.flower_boot))
	} > FLOWER_BOOT

}
