arm-linux-gcc -c -o led.o led.c
arm-linux-gcc -c -o start.o start.c
arm-linux-ld -Ttext 0 start.o led.o -o led.elf
arm-linux-objcopy -0 binary -S led.ef led.bin
arm-linux-objdump -D led.elf > led.dis