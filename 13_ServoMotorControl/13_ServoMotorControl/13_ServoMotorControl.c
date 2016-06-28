/*
 * ---------------------------------------------------------------- *
 	Project Name: Position control of Servo Motor using PWM
	
	Brief: 	This project aims to control the brightness of LED using 
			PWM
			
	Note: Generally Servo motor require 50 Hz (20 ms Period) signal for its
		  operation. 1.5 ms of duty cycle (high pulse width) represent the 
		  neutral position of motor. 2.0 ms of duty cycle rotates the motor 
		  90 degree clock wise (CW) direction respect to its neutral position 
		  and 1.0 ms duty cycle rotates the motor 90 degree counter clock wise (CCW)
		  direction respect to its neutral position.
		  
		  For my case, 0.6 ms represents 90 degree CCW
		  1.2 ms represents neutral and 1.8 ms represents 90 degree CW
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTD (PD5), TIMER1
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
		
		PD5								Servo Motor Signal Pin
			
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

/* ---------- Calculation to generate 50Hz PWM Signal -------------*/
//	Timer1 Clock, F = F_CPU / Timer Prescaler = 1000000 / 1 = 1000000 Hz
// 	Now, T = 1 / F = 1 uS. 
//	So, TCNT1 will increment its value each 1 uS
//  Fpwm = F_CPU / (prescaler * (1+TOP)) 
// 	So, TOP = 19, 999
//	We can't use WGM Mode 5, 6, 7 as they have fixed TOP value 
//	of 255, 511, 1023 respectively that doesn't meet our requirement
// 	Similarly not possible to use Mode 15, since we need to change 
//	duty cycle also, So only mode 14 is suitable for our requirement
//  ICR1 = TOP = 19, 999
/* ----------------------------------------------------------------*/ 

/* ---------- Duty Cycle Calculation for Servo Positions -------------*/
//	Timer1 Clock, F = F_CPU / Timer Prescaler = 1000000 / 1 = 1000000 Hz
// 	Now, T = 1 / F = 1 uS. 
//	So, TCNT1 will increment its value each 1 uS
//  To generate a pulse of 1.2 ms duty, Total Cycle required = 1.2 ms / 1 us
// 	Total Cycle required = 1200, So, we need to set OCR1A = 1200 to get a pulse
// 	1.2 ms duty, similarly for 0.6 ms OCR1A = 600 and for 1.8 ms OCR1A = 1800
/* ----------------------------------------------------------------*/ 

int main(void) {
	
	// PORTD (PD5) as GPIO Output to generate PWM Signal
	DDRD |= 0x20;
	
	/* ----------------- TIMER1 Initialization --------------------*/
	// 	TIMER1 as Fast PWM Mode 14 (WGM13:00 = 14), OC1A as non-inverting output(COM1A1:COM1A0 = 2)
	// 	OC1B as GPIO (COM1B1:COM1B0 = 0), Clock prescaler: 1 (CS12:10 = 1)
	// 	Set TIMER1 TOP value (ICR1 = 19999)
	// 	Set Duty Cycle of 1.2 ms (OCR1A = 1200)
	// 	RESET Count register TCNT1 = 0;
	// 	Start timer with prescaler 
	/* ------------------------------------------------------------*/
	ICR1 = 19999;
	OCR1A = 1200;
	TCNT1 = 0;
	TCCR1A = 0x82;
	TCCR1B = 0x19;
	
	
    while (1) {
		
		// Rotate servo motor at its neutral position
		// A delay is required to move the motor at its desired position 
		// defined by some ms / 60 degree (check datasheet of servo for exact timing)
		// Here 2 sec is given 
		OCR1A = 1200;
		_delay_ms(2000);
		
		// Rotate servo motor 90 degree CCW
		OCR1A = 600;
		_delay_ms(2000);
		
		// Rotate servo motor at its neutral position
		OCR1A = 1200;
		_delay_ms(2000);
		
		// Rotate servo motor 90 degree CW
		OCR1A = 1800;
		_delay_ms(2000);	
    }
}

