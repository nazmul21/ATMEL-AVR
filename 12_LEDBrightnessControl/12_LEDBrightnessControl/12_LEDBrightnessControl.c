/*
 * ---------------------------------------------------------------- *
 	Project Name: LED brightness control using PWM
	
	Brief: 	This project aims to control the brightness of LED using 
			PWM
		
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTB (PB3), TIMER0
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
		
		PB3								Output LED
			
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
	// Local variable that used to change the duty cycle of PWM
	uint8_t Duty = 0;
	
	// PORTB (PB3/OC0) as GPIO Output
	DDRB |= 0x08;
	
	/* ----------------- TIMER0 Initialization --------------------*/
	// 	TIMER0 as Fast PWM Mode (WGM01:00 = 3), OC0 as Compare Match Mode 2 (COM01:00 = 2)
	// 	Clock prescaler: 8 (CS02:00 = 2), Timer0 Clock = F_CPU/8
	//  Fpwm = F_CPU/(prescaler * 256) = 488.22 Hz
	//  Load Duty Cycle value to 0 (OCR0 = 0)
	// 	RESET Count register (TCNT0 = 0)
	// 	Start timer with prescaler 
	/* ------------------------------------------------------------*/
	OCR0 = 0;
	TCNT0 = 0;
	TCCR0 = 0x6A;
	
    while (1) {
		
		// Increment the duty cycle from 0 to 100 after 125 mS interval
		// Note 100 doesn't mean 100%
		// Duty Cycle in % (OCR0 = 50) = 50 / 255 = 20%
		for (Duty = 0; Duty <= 100; Duty++) {
			OCR0 = Duty;
			_delay_ms(125);
		}
		
		// Decrement the duty cycle from 100 to 0 after 125 mS interval
		// Note 100 doesn't mean 100%
		// Duty Cycle in % (OCR0 = 50) = 50 / 255 = 20%
		for (Duty = 100; Duty > 0; Duty--) {
			OCR0 = Duty;
			_delay_ms(125);
		}
    }
}

