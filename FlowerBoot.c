#include "sam.h"

void flowerBoot();

#define FLOWER_BOOT_SIZE 0x200
#define VERSION_OFFSET 0x40

#define TEXT_START (void*)(0x00000000 + 0x2000 + FLOWER_BOOT_SIZE) /* 0x2300 */
#define MAX_APPLICATION_SIZE (0x40000 - 0x2000) / 2   /* 0x1F000 (includes FLOWER_BOOT) */
#define FLASH_UPDATE_ADDRESS (void*)(0x00000000 + 0x2000 + MAX_APPLICATION_SIZE) /* 0x21000 */


__attribute__ ((section(".flower_boot"))) void dummyEmptyHandler() { }

__attribute__ ((section(".flower_boot"))) void dummyBlockingHandler() {
  while (1);
}

__attribute__ ((section(".flower_boot_isr_vector")))
const void* fb_exception_table[] = {
  (void*) (0x20008000), // stack
  (void*) flowerBoot, // reset vector
  (void*) dummyBlockingHandler,
  (void*) dummyBlockingHandler,
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) dummyBlockingHandler,
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) dummyBlockingHandler,
  (void*) dummyEmptyHandler  /* sysTickHandler */
};

__attribute__ ((section(".flower_boot_version"))) const void* FLOWER_BOOT_VERSION = (void*) 0x42460001;
__attribute__ ((section(".flower_boot"))) void flowerBoot();
__attribute__ ((section(".flower_boot"))) void fb_flashWrite(const volatile void* address, const volatile void* data, uint32_t size);

/* entry point */
void flowerBoot() {
  // get flower boot version from update
  uint32_t version = *(uint32_t*)(FLASH_UPDATE_ADDRESS + VERSION_OFFSET);
  if (version >> 16 == 0x4246) { // signature 0x4642 exists
    // copy application (excl. FLOWER_BOOT to execution address)
    fb_flashWrite(TEXT_START, FLASH_UPDATE_ADDRESS + FLOWER_BOOT_SIZE, MAX_APPLICATION_SIZE - FLOWER_BOOT_SIZE);

    // erase first row of update
    uint32_t tmp = 0xFFFFFFFF;
    fb_flashWrite(FLASH_UPDATE_ADDRESS, (void*)&tmp, 4);
  } 

  // Reset stack pointer
  __set_MSP(*(uint32_t*)(TEXT_START));

  //Reset vector table address
  SCB->VTOR = ((uint32_t)(TEXT_START) & SCB_VTOR_TBLOFF_Msk);
  
  // address of Reset_Handler is written by the linker at the beginning of the .text section (see linker script)
  uint32_t resetHandlerAddress = *(uint32_t*)(TEXT_START + 4);
  // jump to Reset_Handler
  asm("bx %0"::"r"(resetHandlerAddress));
}

/**
 * This code is based on the Arduino bootloader code for writing into flash memory. 
 */
void fb_flashWrite(const volatile void* address, const volatile void* data, uint32_t size) {
  const uint32_t pageSizes[] = { 8, 16, 32, 64, 128, 256, 512, 1024 };
  const uint32_t PAGE_SIZE = pageSizes[NVMCTRL->PARAM.bit.PSZ];
  const uint32_t ROW_SIZE = PAGE_SIZE * 4;
  const uint32_t NUM_PAGES = NVMCTRL->PARAM.bit.NVMP;

  size = (size + 3) / 4; // size <-- number of 32-bit integers
  volatile uint32_t* src_addr = (volatile uint32_t*) data;
  volatile uint32_t* dst_addr = (volatile uint32_t*) address;

  // Disable automatic page write
  NVMCTRL->CTRLB.bit.MANW = 1;

  // Do writes in pages
  while (size) {
    // if we are at the beginning of a row, erase it first
    if (((uint32_t)dst_addr & (ROW_SIZE - 1)) == 0) {
      NVMCTRL->ADDR.reg = ((uint32_t) dst_addr) / 2;
      NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
      while (!NVMCTRL->INTFLAG.bit.READY) { }
    }
    
    // Execute "PBC" Page Buffer Clear
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
    while (NVMCTRL->INTFLAG.bit.READY == 0) { }

    // Fill page buffer
    uint32_t i;
    for (i=0; i<(PAGE_SIZE/4) && size; i++) {
      *dst_addr = *src_addr;
      src_addr++;
      dst_addr++;
      size--;
    }

    // Execute "WP" Write Page
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
    while (NVMCTRL->INTFLAG.bit.READY == 0) { }
  }

}
