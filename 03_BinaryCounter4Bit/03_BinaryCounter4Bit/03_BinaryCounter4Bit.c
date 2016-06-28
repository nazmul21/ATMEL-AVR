/*
 * ---------------------------------------------------------------- *
 	Project Name: 4-bit Binary Up-Down Counter
	
	Brief: 	4-bit Binary Up-Down counter, input from two switches and
			Output the count value on Output PORT
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTC, PORTD
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
		
		PD2 (EXT Pull-up)             		SW1 (Up Count)
		PD3 (EXT Pull-up)             		SW2 (Down Count)
		PC3:PC0								Output LEDs
			
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
	
	// PORTD (PD3:PD2) as GPIO Input with High-Z
    DDRD &= ~0x0C;
	// PORTC (PC3:PC0) as GPIO Output
	DDRC = 0x0F;
	
	// Variable that counts the Input
	uint8_t count = 0;
	
    while (1) {
		
		// Check if SW1 is pressed (Up Count)
		if ( (PIND & 0x04) == 0 ) {
			// Switch De-bounce delay of 250 ms
			_delay_ms(250);
			
			// Check if count value is less than MAX value (15 for 4-bit)
			// If true increment the count value, otherwise continue
			if (count < 15)
				count = count + 1;
			else
				continue; 
			
			// Clear PC3:PC0
			PORTC &= 0xF0;
			// Show the count value to Output PORT (PC3:PC0) 
			PORTC |= (count & 0x0F);
		}
		
		// Check if SW2 is pressed (Down Count)
		if ( (PIND & 0x08) == 0 ) {
			// Switch De-bounce delay of 250 ms
			_delay_ms(250);
			
			// Check if count value is greater than MIN value (0)
			// If true decrement the count value, otherwise continue
			if (count > 0)
				count = count - 1;
			else
				continue;
			
			
			// Clear Only bits (PC3:PC0)
			PORTC &= 0xF0;
			// Show the count value to Output PORT (PC3:PC0) 
			PORTC |= (count & 0x0F);
		}
    }
	
	return 0;
}

