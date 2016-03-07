/* license-start
 * 
 * Copyright (C) 2016 Crispico Resonate, <http://www.crispico.com/>.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 3.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details, at <http://www.gnu.org/licenses/>.
 * 
 * license-end
 */

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
		KEEP(*(.flower_boot_isr_vector)) /* 64 (0x40) bytes */
		KEEP(*(.flower_boot_version))	/* address 0x40 */
		KEEP(*(.flower_boot))
	} > FLOWER_BOOT

}
