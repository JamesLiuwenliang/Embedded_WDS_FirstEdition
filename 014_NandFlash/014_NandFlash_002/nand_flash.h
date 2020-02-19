
#ifndef _NAND_FLASH_H
#define _NAND_FLASH_H
void wait_ready ();
void nand_flash_init(void);
void nand_deselect(void);
void nand_select(void);
void nand_cmd(unsigned char cmd);
void nand_addr_byte(unsigned char addr);
unsigned char  nand_data(void);
void nand_chipid();
void nand_read(unsigned int addr , unsigned char *buf ,unsigned int len);
void nand_flash_test();

#endif


