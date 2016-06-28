/*
 * ---------------------------------------------------------------- *
 	Project Name: 1Hz signal generation using Timer0
	
	Brief: 	This project aims to blink LEDs with 1Hz interval using
			TIMER peripheral on AVR
			
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTC, TIMER0
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
volatile uint8_t ovf_count = 0;
/* ----------------------------------------------------------------*/

/* --------------------- Calculation to get 1 Sec -----------------*/
//	Timer0 Clock F = F_CPU / Timer Prescaler = 1000000 / 64 = 15, 625 Hz
// 	Now, T = 1 / F = 64 uS. 
//	So, TCNT0 will increment its value each 64 uS
//  Total Count Required = 1 / 64 uS = 15, 625 
// 	Since TCNT0 is 8 bit wide, Maximum possible count = 256 
// 	So we need to enable TIMER0 overflow INT to get 1 Sec
// 	Overflow Count = Total Count Required / Count with one Overflow
// 	Overflow Count = 15625 / 256 (8-bit) = 61.035 = 61
/* ----------------------------------------------------------------*/ 

int main(void) {
	
	// PORTC (PC7:PC0) as GPIO Output
	DDRC = 0xFF;
	
	/* ----------------- TIMER0 Initialization --------------------*/
	// 	TIMER0 as Normal Mode (WGM01:00 = 0), OC0 as GPIO (COM01:00 = 0)
	// 	Clock prescaler: 64 (CS02:00 = 3), Timer0 Clock = F_CPU/64
	// 	Enable TIMER0 Overflow INT(TOIE0 = 1)
	// 	RESET Count register TCNT0 = 0;
	// 	Start timer with prescaler 
	/* ------------------------------------------------------------*/
	TIMSK |= 0x01;
	TCNT0 = 0;
	TCCR0 = 0x03;
	
	// Enable Global Interrupt (BIT7 of SREG)
	sei();
	
    while (1) {
		
    }
}

// TIMER0 Overflow Interrupt Service Routine
ISR(TIMER0_OVF_vect) {
	// Increment overflow counter
	ovf_count = ovf_count + 1;
	
	// Check if overflow count is equal to 61, which required to get 1 Sec
	if (ovf_count == 61) {
		// If so, then toggle the output LEDs
		PORTC ^= 0xFF;
		
		// Reset the counter to start again
		ovf_count = 0;
	}
}
