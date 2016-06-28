/*
 * ---------------------------------------------------------------- *
 	Project Name: 1Hz signal generation using Timer1
	
	Brief: 	This project aims to blink LEDs with 1Hz interval using
			TIMER peripheral on AVR
			
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTC, TIMER1
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
		
		PC7:PC0								Output LEDs
			
	NB: MCU Power pins, RESET pin should connect properly
		
 * ---------------------------------------------------------------- *
 
 */ 

/* --------------------- Defined Symbols --------------------------*/
/* ----------------------------------------------------------------*/ 

#include <avr/io.h>
#include <avr/interrupt.h>

/* ------------------ Function Prototypes -------------------------*/
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
/* ----------------------------------------------------------------*/

/* --------------------- Calculation to get 1 Sec -----------------*/
//	Timer1 Clock, F = F_CPU / Timer Prescaler = 1000000 / 64 = 15, 625 Hz
// 	Now, T = 1 / F = 64 uS. 
//	So, TCNT1 will increment its value each 64 uS
//  Total Count Required = 1 / 64 uS = 15, 625 
// 	Since TCNT1 is 16-bit wide, Maximum possible count = 65, 535
//  We need to set the TOP value of TIMER1 module to 
//	(15, 625 - 1 (since count start from 0) ) to get 1 Sec 
//  So we choose mode 4 (WGM13:00 = 4), CTC, TOP defined with OCR1A
// 	OCR1A = 15 625 - 1
/* ----------------------------------------------------------------*/  

int main(void) {
	
	// PORTC (PC7:PC0) as GPIO Output
	DDRC = 0xFF;
	
	/* ----------------- TIMER1 Initialization --------------------*/
	// 	TIMER0 as CTC Mode 4 (WGM13:00 = 4), OC1A as GPIO (COM1A1:COM1A0 = 0)
	// 	OC1B as GPIO (COM1B1:COM1B0 = 0), Clock prescaler: 64 (CS12:10 = 3)
	// 	Enable TIMER1 Output Compare A Match INT (OCIE1A = 1)
	// 	RESET Count register TCNT1 = 0;
	// 	Start timer with prescaler 
	/* ------------------------------------------------------------*/
	OCR1A = 15625 -1;
	TCNT1 = 0;
	TIMSK |= 0x10;
	TCCR1A = 0x00;
	TCCR1B = 0x0B;
	
	// Enable Global Interrupt (BIT7 of SREG)
	sei();
	
    while (1) {
		
    }
}

// TIMER1 Output Compare Match A Interrupt Service Routine
ISR(TIMER1_COMPA_vect) {
	
	// Toggle Output LEDs
	PORTC ^= 0xFF;
}

