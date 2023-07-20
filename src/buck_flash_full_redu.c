/*
 * File:   newmain.c
 * Author: Jakub
 *
 * Created on 7. ?ervna 2020, 20:54
 */
//#include <xc.h>
#include <avr/io.h>
//#include <avr/wdt.h>
//#define F_CPU 1000000UL
//#include <util/delay.h>

//CLEAR means 1 - unprogrammed!
FUSES = {
	//0xFF, // BYTE0 {RSTDISBL=CLEAR, WDTON=CLEAR, CKOUT=CLEAR}
    .byte = 0xFD // BYTE0 {RSTDISBL=CLEAR, WDTON=SET, CKOUT=CLEAR} //NOT WORKING!!! have to set WDT by in FW
//    .byte = 0xFC // BYTE0 {RSTDISBL=SET, WDTON=SET, CKOUT=CLEAR} //NOT WORKING!!! have to set WDT by in FW, HVP 12V needed!
};

//__fuse_t __fuse __attribute__((section (".fuse"))) = 
//{
//    .byte = 0xFC // BYTE0 {RSTDISBL=SET, WDTON=SET, CKOUT=CLEAR} //NOT WORKING!!! have to set WDT by in FW
//};

LOCKBITS = {
	LB_MODE_3, // LOCKBIT   //WRITE and READ disabled
};

//#pragma config RSTDISBL = CLEAR
//#pragma config WDTON = SET
//#pragma config CKOUT = CLEAR

////#pragma config LB = BODLEVEL_4V3

//#define SLEEPmode       asm("SLEEP")
#define cNOP            asm("NOP")
#define cWDR            asm("WDR")

#define press_change    10     //n*10ms
#define press_off       90     //n*10ms

//#define Curr_100        83     //full power 135mA (255-n)  SINGLE PLED    1.275*(200-current)
//#define Curr_100        64     //full power 150mA (255-n)  SINGLE PLED
#define Curr_100        50     //full power 160mA (255-n)  SINGLE PLED def.
//#define Curr_100        24     //full power 180mA (255-n)  SINGLE PLED
//#define Curr_100        0     //full power 200mA (255-n)  SINGLE PLED

//#define Curr_redu       137     //80mA
#define Curr_redu       190     //50mA

                                //MODES: 0 = strobe, 1 = full, 2 = redu
#define WaUpMode        2       //set mode before targeted wake-up mode (0->full, 1->redu, 2->strobe)
#define WaUpState       0       //set wake up state 0=OFF, 1=ON


void my_delay_10ms(uint8_t set_ms);


int main(void)
{
    uint8_t tmp8_1 = 255;       //to run config for some mode
//    uint8_t strobe_mode = 1;    //for PWM dimming at startup
    uint8_t LIGHT_mode = WaUpMode;    
    uint8_t ONoff = WaUpState;
//    uint8_t mode_change = 1;
//    uint8_t FULLinten = 0;
    
    //CLKs
    //DEF: 8MHz/8 = 1MHz
    CCP = 0xD8; //protection OFF
    CLKMSR = 0; //0=8MHz, 1=128kHz
    CCP = 0xD8; //protection OFF
    CLKPSR = 3; //8/8 = 1MHz, 1:1-1:256 from 8MHz or 128kHz
    
    //IOs
    PORTB = 0b00000000;  //all OFF
    DDRB =  0b00001011;  //data direction, 1 = output, PB1 and PB0 as output
    PUEB =  0b00000100;  //pullup for BT on PB2
    
    //WDT
    if((RSTFLR & 0x01) != 0)    //wait after PowerOn reset and reset again
    {
        RSTFLR = 0;
        CCP = 0xD8; //protection OFF
        WDTCSR = 0b00001011; //0.125s WDT timeout
        
        while(1){};
    }
    
    CCP = 0xD8; //protection OFF
//    WDTCSR = 0b00001101; //0.5s WDT timeout
    WDTCSR = 0b00001110; //0.5s WDT timeout
    
    
//    while(1)
//    {
//        PORTB = 0b00000000;    //MOSFET ON and driver OFF
//        my_delay_10ms(100);
//        PORTB = 0b00000001;    //MOSFET OFF and driver OFF
//        my_delay_10ms(100);
//    }
    
    //INT
    
    
//    //TIMER - PWM for dimming
//    TCCR0B = 0;
//    TCCR0A = 0;
//    TCNT0 = 0;
//    ICR0 = 0;
//    OCR0A = 0;
//    OCR0B = 0;
//
//    PORTB = 0b00000001;    //NMOSFET/LED ON on PB0, driver OFF
//    
//    GTCCR = 0b00000001;     //reset prescaler
//    ICR0 = 40;              //period register
//    TCNT0 = 0;              //timer count
//    OCR0B = Curr_100;       //output compare reg. B (driver)
//    TCCR0A = 0b00110010;    //set on compare match for ch. B, fast PWM 14 (1110), for true 0 on out
//    TCCR0B = 0b00011001;    //timer config reg. B, 1:1 => 1MHz
    
    
    //start
//    _delay_ms(100);
    my_delay_10ms(5);
    
    if(ONoff == 0)      //flash once when connected to OFF state
    {
        PORTB = 0b00000011;    //NMOSFET/LED ON and driver ON
        my_delay_10ms(10);
        PORTB = 0b00000000;    //NMOSFET/LED OFF and driver OFF
    }
    
    
    while(1)
    {
START:
//        if(mode_change != 0)
//        {
//            
//            mode_change = 0;
//        }
        
//        if(strobe_mode == 0)        //in constant mode - rearm only when pin = 1
//        {
        if(((PINB & 0b00000010) != 0)||(ONoff == 0)) //rearm only when EN pin = 1 or OFF
        {
            cWDR;   //reset WDT
        }
//        }
//        else
//        {
//            cWDR;   //reset WDT
//        }
        
//        cWDR;   //reset WDT
        
        if((tmp8_1 > press_change)&&(ONoff != 0))
        {
            //strobe_mode ^= 0b00000001;

            if(LIGHT_mode == 2)
            {
                //TIMER - PWM for strobe
                TCCR0B = 0;
                TCCR0A = 0;
                TCNT0 = 0;
                ICR0 = 0;
                OCR0A = 0;
                OCR0B = 0;
                LIGHT_mode = 0;
                PORTB = 0b00000001;    //NMOSFET/LED ON and driver OFF
//                _delay_ms(250);
                my_delay_10ms(25);
                
                GTCCR = 0b00000001;     //reset prescaler
//                ICR0 = 1627;            //period register 2.4Hz/35%
                ICR0 = 1302;            //period register 3Hz/35%
                TCNT0 = 0;              //timer count
//                OCR0A = 1038;           //output compare reg. B (MOSFET) 1058
//                OCR0B = 1058;           //output compare reg. B (driver)
//                TCCR0A = 0b11110010;    //set on compare match for ch. A and B, fast PWM 14 (1110) - causes spikes to 25V!
//                OCR0A = 600;           //output compare reg. B (MOSFET) to discharge coil
//                OCR0B = 569;           //output compare reg. B (driver)
                OCR0A = 490;           //output compare reg. B (MOSFET) to discharge coil
                OCR0B = 456;           //output compare reg. B (driver)
                TCCR0A = 0b10100010;    //clear on compare match for ch. A and B, fast PWM 14 (1110)
                TCCR0B = 0b00011100;    //timer config reg. B, 1:256 => 3906.25Hz
            }
            else
            {
                //TIMER - PWM for dimming
CONF:           TCCR0B = 0;
                TCCR0A = 0;
                TCNT0 = 0;
                ICR0 = 0;
                OCR0A = 0;
                OCR0B = 0;
//                strobe_mode = 0;
                PORTB = 0b00000000;    //NMOSFET/LED and driver OFF
//                _delay_ms(250);
                my_delay_10ms(5);
                PORTB = 0b00000001;    //NMOSFET/LED ON and driver OFF
//                _delay_ms(10);
                my_delay_10ms(1);
                
                GTCCR = 0b00000001;     //reset prescaler
//                ICR0 = 40;              //period register
                ICR0 = 255;              //period register
                TCNT0 = 0;              //timer count
                if(LIGHT_mode == 0)
                {
                    LIGHT_mode = 1;
                    OCR0B = Curr_100;       //output compare reg. B
//                    FULLinten = 1;
                }
                else
                {
                    LIGHT_mode = 2;
                    OCR0B = Curr_redu;       //output compare reg. B
//                    FULLinten = 0;
                }
                TCCR0A = 0b00110010;    //set on compare match for ch. B, fast PWM 14 (1110)
//                TCCR0A = 0b00100010;    //clear on compare match for ch. B, fast PWM 14 (1110)
//                TCCR0B = 0b00011001;    //timer config reg. B, 1:1 => 1MHz
                TCCR0B = 0b00011010;    //timer config reg. B, 1:8 => 125kHz
            }

            while((PINB & 0b00000100) == 0){cWDR;}
        }
        
        
        tmp8_1 = 0;
        while((PINB & 0b00000100) == 0)
        {
            tmp8_1++;
//            _delay_ms(10);
            my_delay_10ms(1);
            if(tmp8_1 > press_off)
            {
                if(ONoff != 0)
                {
                    ONoff = 0;
                    TCCR0B = 0;
                    TCCR0A = 0;
                    TCNT0 = 0;
                    ICR0 = 0;
                    OCR0A = 0;
                    OCR0B = 0;
                    GTCCR = 0b00000001;     //reset prescaler
                    PORTB = 0b00000000;    //NMOSFET/LED and driver OFF
                    while((PINB & 0b00000100) == 0){cWDR;}

                }
                else
                {
                    ONoff = 1;
//                    strobe_mode = 1;
//                    goto CONF;
                    
//                    if(LIGHT_mode == 0){LIGHT_mode = 2;}
//                    else{LIGHT_mode--;}
                    LIGHT_mode = WaUpMode;
                    tmp8_1 = 255;
                    goto START;
                }
                tmp8_1 = 0;
            }
        }
        
    }
}


void my_delay_10ms(uint8_t set_ms)
{
    uint8_t tmp8_1 = set_ms;
    uint8_t tmp8_2;
    uint8_t tmp8_3 = 0;
    
    while(tmp8_1 != 0)
    {
        tmp8_2 = 13;
        do
        {
            
            do
            {
                tmp8_3--;
            }while(tmp8_3 != 0);

            tmp8_2--;

        }while(tmp8_2 != 0);
        
        tmp8_1--;
    }
    cWDR;   //reset WDT
}
        
        
  