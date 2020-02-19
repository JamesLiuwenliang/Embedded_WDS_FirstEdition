


#ifndef _INIT_H
#define _INIT_H

void sdram_init(void);
int sdram_test (void);
void clean_bss(volatile unsigned int *start ,volatile unsigned int *end);
void copy2sdram(volatile unsigned int *src ,volatile unsigned int *dest);

#endif


