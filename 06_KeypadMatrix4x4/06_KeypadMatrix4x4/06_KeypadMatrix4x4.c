/*
 * ---------------------------------------------------------------- *
 	Project Name: 4x4 Matrix Keypad Interfacing
	
	Brief: 	This project aims to read inputs from 4x4 Matrix keypad
			and show pressed key value on a 16x2 LCD display
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTA, PORTC, PORTD
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
		
		PD4									LCD RS 
		PD5									LCD R/W
		PD6									LCD E
		PC7:PC4								LCD Data (D7:D4)
		PA3:PA0								Keypad Rows (3:0)
		PA7:PA4								Keypad Columns (3:0)
		
	NB: MCU Power pins, RESET pin, LCD Power pins, LCD Contrast pin
		LCD Back-light LED pins should connect properly
			
 * ---------------------------------------------------------------- *
 
 */ 

/* --------------------- Defined Symbols --------------------------*/
#define NO_KEY			0xFF

#define ROW_SIZE		4
#define COLUMN_SIZE		4
/* ----------------------------------------------------------------*/ 
 
 
#include <avr/io.h>

// Include user library for LCD
#include "lcdlib/lcd_lib.h"

/* ------------------ Function Prototypes -------------------------*/
static uint8_t getKeyPressed(void);
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
/* ----------------------------------------------------------------*/

int main(void) {
	uint8_t key_value = NO_KEY;
	// Power up delay to stabilize LCD
	_delay_ms(100);
	
	// PORTA (PA7:PA0) as GPIO Input
	// Internal Pull-up enable @ matrix rows (PA3:PA0)
	// High-Z enable @ matrix columns (PA7:PA4)
    DDRA = 0x00;
	PORTA |= 0x0F;
	
	// Initialize LCD, print welcome message, wait 2 secs
	LCDinit();
	LCDWriteStringXY(2, 0, "Matrix Keypad");
	_delay_ms(2000);
	
	// Clear LCD, Show the "Key Pressed = " message
	LCDclr();
	LCDWriteStringXY(0, 0, "Key");
	LCDWriteStringXY(2, 1, "Pressed = "); 
	
    while (1) {
		// Call getKeyPress to see if any key is pressed
		key_value = getKeyPressed();
		
		// If key is pressed then show its value on LCD
		if(key_value != NO_KEY) {
			LCDWriteIntXY(12, 1, key_value, 2);
		}
		
		// Delay of 100 ms
		_delay_ms(100);
	}
}

/* ------------------------------------------------------

 *  Function: static uint8_t getKeyPressed(void)
 
 *	Used to get input from keypad
	
 *						Arguments
		None

 *						Return
		15:0 					corresponding key pressed 
		0xFF					No key pressed
 --------------------------------------------------------*/

static uint8_t getKeyPressed(void) {
	uint8_t row, column;
	
	// Internal Pull-up enable @ matrix rows (PA3:PA0)
	// High-Z enable @ matrix columns (PA7:PA4)
	PORTA |= 0X0F;
	
	for(column = 0; column < COLUMN_SIZE; column++) {
		// De-select column, i,e all are inputs
		DDRA &= 0x00;
		
		//Select a column to make corresponding pin OUTPUT LOW
		DDRA |= (0X80 >> column);
		
		// Scan Rows
		for(row = 0; row < ROW_SIZE; row++) {
			
			// Check if key pressed 
			if(! (PINA & ( 0X08 >> row)) ) {
				// if pressed return the corresponding value
				return ( (row*COLUMN_SIZE)+column);
			}
		}
	}
	
	// If no key pressed, return NO_KEY (0xFF) which indicated no key pressed
	return NO_KEY;
}
