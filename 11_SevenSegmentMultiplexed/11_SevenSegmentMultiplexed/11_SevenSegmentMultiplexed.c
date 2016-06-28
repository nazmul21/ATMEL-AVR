/*
 * ---------------------------------------------------------------- *
 	Project Name: Common Anode (CA) 7-segment display multiplexing 
	
	Brief: 	This project aims to multiplex four 7-segment (CA) display
			to print value from 0 to 9999 with fewer GPIOs
			
	Note: Each 7-segment display will refresh at 200 Hz (5 mS) interval
		  which is not observable by human eyes
			
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTC, TIMER0
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
							
		PC0									Segment LED a
		PC1									Segment LED b
		PC2									Segment LED c
		PC3									Segment LED d
		PC4									Segment LED e
		PC5									Segment LED f
		PC6									Segment LED g
		PD0									Digit 3 Transistor Base
		PD1									Digit 2 Transistor Base
		PD2									Digit 1 Transistor Base
		PD3									Digit 0 Transistor Base
			
	NB: MCU Power pins, RESET pin, Transistor base should connect properly
		
 * ---------------------------------------------------------------- *
 
 */ 

/* --------------------- Defined Symbols --------------------------*/
/* ----------------------------------------------------------------*/ 
 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* ------------------ Function Prototypes -------------------------*/
static void DisplayDigit(uint8_t val);
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
volatile uint16_t Count = 0;
/* ----------------------------------------------------------------*/


/* --------------------- Calculation to get 5 mS ------------------*/
//	Timer1 Clock, F = F_CPU / Timer Prescaler = 1000000 / 64 = 15, 625 Hz
// 	Now, T = 1 / F = 64 uS. 
//	So, TCNT1 will increment its value each 64 uS
//  Total Count Required = 5 mS / 64 uS = 78.125 = 78 
// 	Since TCNT0 is 8-bit wide, Maximum possible count = 256
//  We need to set the TOP value of TIMER0 module to 
//	(78 - 1 (since count start from 0) ) to get 5 mS 
//  So we choose mode 2 (WGM01:00 = 2), CTC, TOP defined with OCR0
// 	OCR0 = 78 - 1
/* ----------------------------------------------------------------*/ 

int main(void) {
	
	// PORTC (PC6:PC0) as GPIO Output to drive 7 Segment
	DDRC = 0xFF;
	
	// PORTD (PD3:PD0) as GPIO Output to select individual segment of multiplexed 7 segment
	// PD3  ------ Digit 0
	// PD2  ------ Digit 1
	// PD1  ------ Digit 2
	// PD0  ------ Digit 3
	DDRD = 0x0F;
	
	// De-select all segment to write logic '1' to corresponding bit (PNP transistor)
	// Power down for all segment
	PORTD |= 0x0F;
	
	/* ----------------- TIMER0 Initialization --------------------*/
	// 	TIMER0 as CTC Mode (WGM01:00 = 2), OC0 as GPIO (COM01:00 = 0)
	// 	Clock prescaler: 64 (CS02:00 = 3), Timer0 Clock = F_CPU/64
	// 	Enable TIMER0 Output Compare Match INT(OCIE0 = 1)
	// 	RESET Count register (TCNT0 = 0)
	// 	Start timer with prescaler 
	/* ------------------------------------------------------------*/
	OCR0 = 78 - 1;
	TIMSK |= 0x02;
	TCNT0 = 0;
	TCCR0 = 0x0B;
	
	// Enable Global Interrupt (BIT7 of SREG)
	sei();
	
    while (1) {
		// Check if count val is less than 9999
		// If so then increment count value else set 0
		if (Count < 9999)
			Count = Count + 1;
		else
			Count = 0;
		
		// Delay of 500 mS
		_delay_ms(500);
    }
}

// TIMER0 Output Compare Match Interrupt Service Routine
ISR(TIMER0_COMP_vect) {
	
	// Local variable to select each segment
	static uint8_t ctn = 0;
	
	// Copy Count value to a local variable
	uint16_t temp = Count;
	
	// De-select all segment to write logic '1' to corresponding bit (PNP transistor)
	PORTD |= 0x0F;
	
	switch (ctn) {
		// Digit 0 update time
		case 0:
			// Select Digit 0
			// Power up for Digit 0
			PORTD &= ~0x08;
			// Write Value to Digit 0
			DisplayDigit(temp%10);
			break;
			
		// Digit 1 update time
		case 1:
			// Select Digit 1
			// Power up for Digit 1
			PORTD &= ~0x04;
			// Write Value to Digit 1
			DisplayDigit((temp/10)%10);
			break;
			
		// Digit 2 update time
		case 2: 
			// Select Digit 2
			// Power up for Digit 2
			PORTD &= ~0x02;
			// Write Value to Digit 2
			DisplayDigit((temp/100)%10);
			break;
			
		// Digit 3 update time
		case 3:
			// Select Digit 3
			// Power up for Digit 3
			PORTD &= ~0x01;
			// Write Value to Digit 3
			DisplayDigit((temp/1000)%10);
			break;
	}
	
	// Increment display selector variable
	ctn = ctn + 1;
	
	// Check if All display written (0 ... 3)
	if (ctn == 4)
		// If all written, then restart from 0
		ctn = 0;
}

/* ------------------------------------------------------

 *  Function: static void DisplayDigit(uint8_t num)
 
 *	Used to show digits (0..9) on 7-segment (CA)
	
 *						Arguments
	uint8_t num: digit to show, If num greater than 9 then
				 (0) will be print on display

 *						Return
		None
 --------------------------------------------------------*/
static void DisplayDigit(uint8_t val) {
	
	// Clear only bits (PC6:PC0)
	PORTC &= 0x80;

	switch (val) {
		// Print 0 on 7-segment if num = 0 or num > 9
		default:
		case 0:
			PORTC |= 0xC0;
			break;
		case 1:
			PORTC |= 0xF9;
			break;
		case 2:
			PORTC |= 0xA4;
			break;
		case 3:
			PORTC |= 0xB0;
			break;
		case 4:
			PORTC |= 0x99;
			break;
		case 5:
			PORTC |= 0x92;
			break;
		case 6:
			PORTC |= 0x82;
			break;
		case 7:
			PORTC |= 0xF8;
			break;
		case 8:
			PORTC |= 0x80;
			break;
		case 9:
			PORTC |= 0x90;
			break;
	}
}