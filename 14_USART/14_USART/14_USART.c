/*
 * ---------------------------------------------------------------- *
 	Project Name: Asynchronous Serial Communication of AVR MCU using 
				  USART (Blocking Method) 
	
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

/* ------------------ Function Prototypes -------------------------*/
unsigned char USART_Receive(void);
void USART_Transmit(unsigned char data);
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
/* ----------------------------------------------------------------*/

int main(void) {
	
	unsigned char c;
	
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
	// 	Disable Rx, Tx complete INT, Enable Rx, Tx module
	//	USART as asynchronous mode, No Parity (UPM1:0 = 0), One STOP bit (USBS = 0)
	//	Char Size 8 bit (UCSZ2:0 = 3)
	/* ------------------------------------------------------------*/
	UBRRH = 0;
	UBRRL = 12;
	UCSRA |= 0x02;
	UCSRB |= 0x18;
	UCSRC |= 0x86;
	
    while (1) {
		// wait until a data receive from PC
		c = USART_Receive();
		// Send the received data back to PC 
		USART_Transmit(c);
    }
}

/* ------------------------------------------------------

 *  Function: unsigned char USART_Receive(void)
 
 *	Used to get a single data received by USART Rx Hardware
	
 *						Arguments
		None

 *						Return
		unsigned char 			8 bit received data
 --------------------------------------------------------*/

unsigned char USART_Receive(void) {
	// Wait for data to be received
	while ( !(UCSRA & (1<<RXC)) );
	
	// Get and return received data from buffer 
	return UDR;
}

/* ------------------------------------------------------

 *  Function: void USART_Transmit(unsigned char data)
 
 *	Used to put a single data to be transmitted by USART Tx Hardware
	
 *						Arguments
		unsigned char				8-bit data to be transmitted

 *						Return
		None	
 --------------------------------------------------------*/
void USART_Transmit(unsigned char data) {
	// Wait for empty transmit buffer 
	while ( !( UCSRA & (1<<UDRE)) );
	
	// Put data into buffer, sends the data 
	UDR = data;
}

