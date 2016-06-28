/*
 * ---------------------------------------------------------------- *
 	Project Name: Input-Output Test
	
	Brief: Reading the status of a Input PORT and Writing the status 
		   to an Output PORTC
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTC, PORTD
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
							
		PC7:PC0 (EXT pull-down)					SW7:SW0
		PD7:PD0 								Output LEDs
		
	NB: MCU Power pins, RESET pin should connect properly
 * ---------------------------------------------------------------- *
 
 */ 

/* --------------------- Defined Symbols --------------------------*/
/* ----------------------------------------------------------------*/  
 
#include <avr/io.h>

/* ------------------ Function Prototypes -------------------------*/
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
/* ----------------------------------------------------------------*/

int main(void) {
	
	// PORTC (PC7:PC0) as GPIO Input with High - Z
	DDRC = 0x00;
	// PORTD (PD7:PD0) as GPIO Output
	DDRD = 0xFF;
	
    while (1) {
		
		// Reading data from Input PORTC (PC7:PC0)
		// Write the data to Output PORTD (PD7:PD0)
		PORTD = PINC;
    }
	
	return 0;
}

