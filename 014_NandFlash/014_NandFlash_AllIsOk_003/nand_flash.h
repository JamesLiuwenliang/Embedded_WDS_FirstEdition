
#ifndef _NAND_FLASH_H
#define _NAND_FLASH_H


void wait_ready ();
unsigned char nand_data();
unsigned char nand_write_data(unsigned char val);
void nand_flash_init(void);
void nand_deselect(void);
void nand_select(void);
void nand_cmd(unsigned char cmd);
void nand_addr_byte(unsigned char addr);

void nand_chipid();

void nand_read(unsigned int addr , unsigned char *buf ,unsigned int len);
int nand_erase(unsigned int addr,  unsigned int len);
void nand_write(unsigned int addr , unsigned char *buf,unsigned int len );

void do_read_nand_flash(void);
void do_erase_nand_flash(void);
void do_write_nand_flash(void);

void nand_flash_test();

#endif


