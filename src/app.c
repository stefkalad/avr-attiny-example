#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define cNOP            asm("NOP")
#define cWDR            asm("WDR")

void my_delay_10ms(uint8_t set_ms);


// CLEAR means 1 - unprogrammed!
// FUSES = {
//     .byte = 0xFD // BYTE0 {RSTDISBL=CLEAR, WDTON=SET, CKOUT=CLEAR} //NOT WORKING!!! have to set WDT by in FW
// };

int main()
{

    CCP = 0xD8; //protection OFF
    CLKMSR = 0; //0=8MHz, 1=128kHz
    CCP = 0xD8; //protection OFF
    CLKPSR = 3; //8/8 = 1MHz, 1:1-1:256 from 8MHz or 128kHz

    //IOs
    PORTB = 0b00000000;  //all OFF
    DDRB =  0b00001011;  //data direction, 1 = output, PB1 and PB0 as output
    PUEB =  0b00000100;  //pullup for BT on PB2

    
    PORTB = 0b0000000;    //NMOSFET/LED ON and driver ON

    while(1){
        PORTB = 0b0000001;    //NMOSFET/LED ON and driver ON
        my_delay_10ms(100);
        PORTB = 0b0000000;    //NMOSFET/LED ON and driver ON
        my_delay_10ms(100);
   }
    return 0;
}



void my_delay_10ms(uint8_t set_ms)
{
    uint8_t tmp8_1 = set_ms;
    uint8_t tmp8_2;
    uint8_t tmp8_3 = 0;
    
    while(tmp8_1 != 0)
    {
        cWDR;   //reset WDT
        tmp8_2 = 13;
        do
        {
            
            do
            {
                tmp8_3--;
            }while(tmp8_3 != 0);

            tmp8_2--;

        } while(tmp8_2 != 0);
        
        tmp8_1--;
    }
}