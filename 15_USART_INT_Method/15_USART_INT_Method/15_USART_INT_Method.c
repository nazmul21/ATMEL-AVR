/*
 * ---------------------------------------------------------------- *
 	Projcet Name: Asynchronous Serial Communication of AVR MCU using 
				  USART (INT Method) 
	
	Brief: 	This project aims to transfer data between AVR MCU and
			PC via Serial Communication Peripheral (USART)
			
	Note: We can't connect MCU's Tx, Rx pin directly with PC's DB9 
		  Rx, Tx pin due to different voltage level of two hardware.
		  We need a level translator (such as MAX232) which convert 
		  TIA-232 to TTL signal suitable for MCU. We can also use 
		  USB-Serial (PL2303) converter for this purpose also which 
		  creates a virtual COM port on PC. Moreover, We need a terminal
		  software (putty) on PC.
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTD (PD0, PD1), USART
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
		
		PD0								TXD of PL2303
		PD1								RXD of PL2303
		GND								GND of PL2303
			
	NB: MCU Power pins, RESET pin should connect properly
		
 * ---------------------------------------------------------------- *
 
 */ 

/* --------------------- Defined Symbols --------------------------*/
/* ----------------------------------------------------------------*/ 

#include <avr/io.h>
#include <avr/interrupt.h>

/* ------------------ Function Prototypes -------------------------*/
void UART_SendString(unsigned char *str);
void USART_Transmit(unsigned char c);
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
volatile unsigned char r_data;
volatile unsigned char txc = 1;
volatile unsigned char rxc = 0;
/* ----------------------------------------------------------------*/

int main(void) {
	
	/* ----------------- USART Initialization --------------------*/
	// 	BAUD Rate = 9600, F_CPU = 1000000UL, BAUD = F_CPU / (speed * (UBRR+1)) 
	//  speed = 16 for standard more (U2X = 0) and speed = 8 for double speed mode (U2X = 1)
	// 	So, We can choose UBRR = 6 for standard more (U2X = 0)
	// 	or UBRR = 12 for double speed mode (U2X = 1)
	// 	Since double speed mode (U2X = 1) provides lower error, we are going 
	//	to use it for this configuration. Please check datasheet for more detail
	//
	//	Set BAUD rate register to generate our desired BAUD 9600 (UBRR = 12)
	//	Enable double speed mode (U2X = 1)
	// 	Enable Rx, Tx complete INT, Enable Rx, Tx module
	//	USART as asynchorous mode, No Parity (UPM1:0 = 0), One STOP bit (USBS = 0)
	//	Char Size 8 bit (UCSZ2:0 = 3)
	/* ------------------------------------------------------------*/
	UBRRH = 0;
	UBRRL = 12;
	UCSRA |= 0x02;
	UCSRB |= 0xD8;
	UCSRC |= 0x86;
	
	// Enable Global Interrupt (BIT7 of SREG)
	sei();
	
	// Send a string to PC
	UART_SendString((unsigned char *)"Hello World!");
	
    while (1) {
		// Check if a data is received from PC
		if ( rxc == 1 ) {
			// Clear the user receive complete flag
			rxc = 0;
			// Check if the current transmit is completed
			if (txc == 1) {
				// Clear the user transmit complete flag
				txc = 0;
				// Transmit the received data as echo
				USART_Transmit(r_data);
			}
		}
    }
}

/* ------------------------------------------------------

 *  Function: UART_SendString(unsigned char *str)
 
 *	Used to send a string data buffer by USART Tx Hardware
	
 *						Arguments
		unsigned char *				data string to be transmitted

 *						Return
		None	
 -----------------------------------------------------*/
void UART_SendString(unsigned char *str) {
	// Loop until get a terminate charater in the string
	while (*str) {
		// Check if the current transmit is completed 
		while(!txc);
		// Clear the user transmit complete flag
		txc = 0;
		// Transmit a single charater
		USART_Transmit(*str);
		// Increment the string location by 1
		str++;
	}
}

/* ------------------------------------------------------

 *  Function: void USART_Transmit(unsigned char data)
 
 *	Used to put a single data to be transmitted by USART Tx Hardware
	
 *						Arguments
		unsigned char				8-bit data to be transmitted

 *						Return
		None	
 -----------------------------------------------------*/
void USART_Transmit(unsigned char c) {
	// Put data into buffer, sends the data
	UDR = c;
}

// USART Receive Complete INT Service Routine
ISR(USART_RXC_vect) {
	// Check if receive complete INT flag is set
	if (UCSRA & (1<<RXC)) {
		// Get received data from buffer
		r_data = UDR;
		// Set the user receive complete flag
		rxc = 1;
	}
} 

// USART Transmit Complete INT Service Routine
ISR(USART_TXC_vect) {
	// Check if transmit complete INT flag is set
	if (UCSRA & (1<<TXC)) {
		// Set the user transmit complete flag
		txc = 1;
	}
}