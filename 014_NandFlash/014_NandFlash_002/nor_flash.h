
#ifndef _NOR_FLASH_H
#define _NOR_FLASH_H

void nor_flash_test();
void do_write_nor_flash();
void do_read_nor_flash();
void wait_ready(unsigned int addr);

void do_erase_nor_flash();

void do_scan_nor_flash();

unsigned int nor_data(unsigned int offset );
unsigned int  nor_read_word(unsigned int base,unsigned short offset  );
void nor_cmd(unsigned int offset , unsigned int cmd);
void nor_write_word(unsigned int base,unsigned short offset ,unsigned int val );

#endif



