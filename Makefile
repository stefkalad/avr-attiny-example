all : app.bin
# hex : app.hex

AVRPART=attiny10
AVRCFLAGS=-g0 -Wall -O1 -mmcu=$(AVRPART) -DF_CPU=8000000UL
AVRASFLAGS:=$(AVRCFLAGS)

RPI_APP_PATH=/home/pi/fw/prod
RPI_TPIFLASH_PATH=/home/pi/tpi-flash/out


app.bin : src/app.c 
	avr-gcc -I $(AVRCFLAGS) -Wl,-Map,app.map -o app.elf $^
	avr-objdump app.elf -S > app.lst
	avr-objcopy -j .text -j .data -O binary app.elf app.bin 

app.hex : src/app.c 
	avr-gcc -I $(AVRCFLAGS) -Wl,-Map,app.map -o app.elf $^
	avr-objdump app.elf -S > app.lst
	avr-objcopy -O ihex app.elf app.hex 

burn : app.bin
	scp app.bin pi@raspberrypi.local:$(RPI_APP_PATH)
	ssh pi@raspberrypi.local 'sudo $(RPI_TPIFLASH_PATH)/tpiflash w 1e9003 $(RPI_APP_PATH)/app.bin'

transfer : app.bin
	scp app.bin pi@raspberrypi.local:$(RPI_APP_PATH)


clean :
	rm -rf *.o *.elf *.bin *.map *.lst


