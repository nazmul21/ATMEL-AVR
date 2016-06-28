/*
 * ---------------------------------------------------------------- *
 	Project Name: Hello World (LED Blinking)
	
	Brief: Blinking LED on a Output PORT at 1 sec interval
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTC
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
							
		PC7:PC0									Output LEDs
			
	NB: MCU Power pins, RESET pin should connect properly
 * ---------------------------------------------------------------- *
 
 */ 
 
/* --------------------- Defined Symbols --------------------------*/
/* ----------------------------------------------------------------*/ 

#include <avr/io.h>
#include <util/delay.h>

/* ------------------ Function Prototypes -------------------------*/
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
/* ----------------------------------------------------------------*/

int main(void) {
	
	// Set PORTC (PC7:PC0) as GPIO output
	DDRC = 0xFF;
	
    while (1)  {
		
		// Make PORTC (PC7:PC0) as Output High 
		PORTC = 0xFF;
		// Delay for 1 sec
		_delay_ms(1000);
		
		// Make PORTC (PC7:PC0) as Output Low
		PORTC = 0x00;
		// Delay for 1 sec
		_delay_ms(1000);
		
    }
	
	return 0;
}

