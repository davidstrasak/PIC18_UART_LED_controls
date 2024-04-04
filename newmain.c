/*
 * File:   newmain.c
 * Author: David
 *
 * Created on March 12, 2024, 6:07 PM
 */

#include <xc.h>
#include<stdint.h>
#include<string.h>
#include<stdio.h>

#pragma config FOSC = HSHP      // Oscillator Selection bits (HS oscillator (high power > 16 MHz))
#pragma config PLLCFG = ON      // 4X PLL Enable (Oscillator multiplied by 4)
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)

#define LED1 LATDbits.LATD2
#define LED2 LATDbits.LATD3
#define LED3 LATCbits.LATC4
#define LED4 LATDbits.LATD4
#define LED5 LATDbits.LATD5
#define LED6 LATDbits.LATD6
#define _XTAL_FREQ 32E6


void initGPIO(void);
void initUART(void);
void putch(char data){
    while(!TX1IF)
    continue;
    TXREG = data;
    
}
void switchLEDs(unsigned char input);
void blinkLEDs(unsigned char input);


typedef struct{
    char text[5];
    uint8_t full;
} type_command;

volatile type_command command;

void __interrupt() ISR(void){
    if(RC1IE && RC1IF){
        char temp = RCREG1;
        static int i = 0;
        
        if(!command.full){
            if(temp != '.'){
                command.text[i] = temp;
                i++;
            } else {
                command.text[i] = '\0';
                command.full = 1;
                i = 0;
            }
        }
    }
}

void main(void) {
    //Init of LED bits
    initGPIO();
    initUART();
    unsigned char open = 0;
    unsigned char continuous = 0;
    unsigned char blink = 0;
    unsigned char integer = 0;
    unsigned char temporaryblink = 0;
    unsigned char off = 0;
    
    while(1){
        if(command.full == 1){
            if(strcmp(command.text,"AON") == 0){
                off = 0;
                open = 0;
            }
            if(strcmp(command.text,"AOFF") == 0){
                off = 1;
                open = 1;
            }
            
            if(strcmp(command.text,"AC") == 0){
                open = 0;
                blink = 0;
            }
            if(strcmp(command.text,"AB") == 0){
                open = 0;
                blink = 1;
            }
            if(command.text[0] == 'A' && command.text[1] == 'S'){
                open = 0;
                integer = command.text[2] - '0';
                blink = 1;
                temporaryblink = 1;
            }
            
            command.full = 0;
        }
        
        if(off){
            switchLEDs(off);
        }
        
        if(!off){
            if(blink){
                __delay_ms(1000);
                open = !open;
            }

            switchLEDs(open);

            if(integer > 0 && open && temporaryblink){
                integer--;
                __delay_ms(500);
            } else if(integer == 0 && open && temporaryblink){
                blink = 0;
                open = 0;
                temporaryblink = 0;
            }
        }
    }
    
    return;
}

void initGPIO(void){
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISCbits.TRISC4 = 0;
    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD5 = 0;
    TRISDbits.TRISD6 = 0;
    
    ANSELC = 0x00;
        
    LED1 = 1;
    LED2 = 1;
    LED3 = 1;
    LED4 = 1;
    LED5 = 1;
    LED6 = 1;
    
}

void initUART(void){
    PORTCbits.RC6 = 1;
    PORTCbits.RC7 = 1;
    SPBRG1 = 51;
    TXSTA1bits.SYNC = 0;
    RCSTA1bits.SPEN = 1;
    TXSTA1bits.TXEN = 1;
    RCSTA1bits.CREN = 1;
    RC1IE = 1;
    PEIE = 1;
    GIE = 1;
}

void switchLEDs(unsigned char input){
    LED1 = input;
    LED2 = input;
    LED3 = input;
    LED4 = input;
    LED5 = input;
    LED6 = input;
}