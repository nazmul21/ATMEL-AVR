/*
 * ---------------------------------------------------------------- *
 	Project Name: 16x2 LCD Interfacing
	
	Brief: 	This project aims to print user data on a LCD
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTC, PORTD
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
		
		PD4									LCD RS 
		PD5									LCD R/W
		PD6									LCD E
		PC7:PC4								LCD Data (D7:D4)
		
	NB: MCU Power pins, RESET pin, LCD Power pins, LCD Contrast pin
		LCD Back-light LED pins should connect properly
			
 * ---------------------------------------------------------------- *
 
 */ 

/* --------------------- Defined Symbols --------------------------*/
/* ----------------------------------------------------------------*/ 
 
#include <avr/io.h>

// Include user library for LCD
#include "lcdlib/lcd_lib.h"

/* ------------------ Function Prototypes -------------------------*/
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
/* ----------------------------------------------------------------*/

int main(void) {
	// Power up delay to stabilize LCD
	_delay_ms(100);
	
	// Initialize LCD, print welcome message
	LCDinit();
	LCDWriteStringXY(2, 0, "Hello World!");
	
    while (1) {
	}
}
