/*
 * ---------------------------------------------------------------- *
 	Project Name: Common Cathode (CC) 7-segment display interfacing
	
	Brief: 	This project aims to show numeric values (0...9) that 
			count with two input switches on a 7-segment (CC) module			
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTB, PORTD
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
		
		PD2 (EXT Pull-up)             		SW1 (Up Count)
		PD3 (EXT Pull-up)             		SW2 (Down Count)
		PB0									Segment LED a
		PB1									Segment LED b
		PB2									Segment LED c
		PB3									Segment LED d
		PB4									Segment LED e
		PB5									Segment LED f
		PB6									Segment LED g
			
	NB: MCU Power pins, RESET pin should connect properly
		
 * ---------------------------------------------------------------- *
 
 */ 

/* --------------------- Defined Symbols --------------------------*/
/* ----------------------------------------------------------------*/ 

  
#include <avr/io.h>
#include <util/delay.h>

/* ------------------ Function Prototypes -------------------------*/
static void DisplayDigit(uint8_t num);
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
/* ----------------------------------------------------------------*/

int main(void) {
	// PORTB (PB6:PB0) as GPIO Output
    DDRB = 0x7F;
	// PORTD (PD3:PD2) as GPIO Input with High-Z
	DDRD &= ~0x0C;
	
	// Variable that counts the Input
	uint8_t count = 0;
	// Show Initial count value (0) on Display
	DisplayDigit(count);
	
    while (1) {
		
		// Check if SW1 is pressed (Up Count)
		if( (PIND & 0x04) == 0 ) {
			// Switch De-bounce delay of 250 ms
			_delay_ms(250);
			
			// Check if count value is less than MAX value (9)
			// If true increment the count value, otherwise continue
			if (count < 9 )
				count++;
			else
				continue; 
		
			// Show the count value on Display
			DisplayDigit(count);
		}
		
		// Check if SW2 is pressed (Down Count)
		if( (PIND & 0x08) == 0 ) {
			// Switch De-bounce delay of 250 ms
			_delay_ms(250);
			
			// Check if count value is greater than MIN value (0)
			// If true decrement the count value, otherwise continue
			if (count > 0 )
				count--;
			else
				continue;
			
			// Show the count value on Display
			DisplayDigit(count);
		}
    }
	
	return 0;
}

/* ------------------------------------------------------

 *  Function: static void DisplayDigit(uint8_t num)
 
 *	Used to show digits (0..9) on 7-segment (CC)
	
 *						Arguments
	uint8_t num: digit to show, If num greater than 9 then
				 (0) will be print on display

 *						Return
		None
 --------------------------------------------------------*/
 
static void DisplayDigit(uint8_t num) {
	
	// Clear only bits (PB6:PB0)
	PORTB &= 0x80;
	switch (num) {
		// Print 0 on 7-segment if num = 0 or num > 9
		default:
		case 0:
			PORTB |= 0x3F;
			break;
		
		case 1:
			PORTB |= 0x06;
			break;
			
		case 2:
			PORTB |= 0x5B;
			break;
			
		case 3:
			PORTB |= 0x4F;
			break;
			
		case 4:
			PORTB |= 0x66;
			break;
			
		case 5:
			PORTB |= 0x6D;
			break;
		
		case 6:
			PORTB |= 0x7D;
			break;
		
		case 7:
			PORTB |= 0x07;
			break;
		
		case 8:
			PORTB |= 0x7F;
			break;
		
		case 9:
			PORTB |= 0x6F;
			break;
	}
}
