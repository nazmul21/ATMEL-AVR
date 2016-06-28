/*
 * ---------------------------------------------------------------- *
 	Project Name: Counter with External (EXT) Interrupt (INT)
	
	Brief: 	This project aims to show numeric values (0...9) that 
			count with two input switches on a 7-segment (CC) module
			Reading of switches based on EXT INT on corresponding 
			MCU pins. Also Change the default blinking rate (1Hz) of 
			LEDs to illustrate the effect of EXT INT
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTB, PORTC, PORTD, INT0, INT1
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
		
		PD2 (EXT Pull-up)             		SW1 (Up Count)
		PD3 (EXT Pull-down)             	SW2 (Down Count)
		PB0									Segment LED a
		PB1									Segment LED b
		PB2									Segment LED c
		PB3									Segment LED d
		PB4									Segment LED e
		PB5									Segment LED f
		PB6									Segment LED g
		PC7:PC0								Output LEDs
			
	NB: MCU Power pins, RESET pin should connect properly
		
 * ---------------------------------------------------------------- *
 
 */ 

/* --------------------- Defined Symbols --------------------------*/
/* ----------------------------------------------------------------*/  
  
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* ------------------ Function Prototypes -------------------------*/
static void DisplayDigit(uint8_t num);
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
volatile uint8_t int0_dectected = 0;
volatile uint8_t int1_dectected = 0;
volatile uint8_t count = 0;
/* ----------------------------------------------------------------*/

int main(void) {
	
	// PORTB (PB6:PB0) as GPIO Output 
	DDRB = 0x7F;
	// PORTC (PC7:PC0) as GPIO Output
	DDRC = 0xFF;
	
	// INT0 as Raising Edge (ISC11:ISC10 = 3), INT1 as falling edge (ISC01:ISC00 = 2)
	MCUCR |= 0x0B;
	// Enable Interrupt INT0, INT1
	GICR |= 0xC0;
	
	// Show Initial count value (0) on Display
	DisplayDigit(count);
	
	// Enable Global Interrupt (BIT7 of SREG)
	sei(); 
	
    while (1) {
		
		// Make PORTC (PC7:PC0) as Output High 
		PORTC = 0xFF;
		// Delay for 1 sec
		_delay_ms(1000);
		
		// Make PORTC (PC7:PC0) as Output Low
		PORTC = 0x00;
		// Delay for 1 sec
		_delay_ms(1000);
		
		// Check if INT0 detected
		if ( int0_dectected == 1 ) {
			// Clear INT0 flag variable
			int0_dectected = 0;
			
			// Show the count value on Display
			DisplayDigit(count);
			
			// Delay 5 more secs to illustrate the effect of INT0
			_delay_ms(5000);
		}
		
		// Check if INT1 detected
		if ( int1_dectected == 1 ) {
			// Clear INT1 flag variable
			int1_dectected = 0;
			
			// Show the count value on Display
			DisplayDigit(count);
			
			// Delay 10 more secs to illustrate the effect of INT1
			_delay_ms(10000);	
		}
		
    }
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

// INT0 Service routine
ISR(INT0_vect) {
	
	// Set INT0 flag variable
	int0_dectected = 1;
	
	// Check if count value is less than MAX value (9)
	// If true increment the count value, otherwise return
	if (count < 9) {
		count++;
	}
	else
		return;
	
}

// INT1 service routine
ISR(INT1_vect) {
	
	// Set INT0 flag variable 
	int1_dectected = 1;
	
	// Check if count value is greater than MIN value (0)
	// If true decrement the count value, otherwise continue
	if (count > 0) {
		count--;
	}
	else
		return;
}