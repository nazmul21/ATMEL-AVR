/*
 * ---------------------------------------------------------------- *
 	Project Name: Controlling LEDs wirelessly from Smartphone 
				  using Bluetooth Module (HC-05)
	
	Brief: 	This project aims to control LEDs connected with AVR MCU from
			Smartphone wirelessly. 
			
	Note: Command Format: "Ln=d" where n = '1' (LED1 @ PC0) and '2' (LED2 @ PC1)
		  d = '1' to turn corresponding LED ON, other value will turn corresponding 
		  LED OFF
		  
		  Example: 1) "L1=1" will turn LED1 ON and send "OK" as feedback
				   2) "L2=0" will turn LED2 OFF	and send "OK" as feedback
				   3) "L3=0" will send "E02" error code as feedback since LED3 undefined here
				   4) "A1=1" will send "E01" error code as feedback since we must send 'L' to control LEDs
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: PORTC (PC0, PC1), PORTD (PD0, PD1), USART
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
		
		PD0								TXD of HC-05
		PD1								RXD of HC-05
		GND								GND of HC-05
			
	NB: MCU Power pins, RESET pin should connect properly
		
 * ---------------------------------------------------------------- *
 
 */ 

/* --------------------- Defined Symbols --------------------------*/
#define CMD_SIZE	4
/* ----------------------------------------------------------------*/ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

/* ------------------ Function Prototypes -------------------------*/
void UART_SendString(unsigned char *str);
void USART_Transmit(unsigned char c);
void ExecuteCommand(void);
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
unsigned char cmd[CMD_SIZE] = {0};
unsigned char rx_index_ctn = 0;
/* ----------------------------------------------------------------*/

int main(void) {
	
	// PORTC (PC1:PC0) as GPIO Output to drive LEDs
	DDRC |= 0x03;
	
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
	// 	Enable Rx complete INT, Disable Tx Complete Interrupt, Enable Rx, Tx module
	//	USART as asynchronous mode, No Parity (UPM1:0 = 0), One STOP bit (USBS = 0)
	//	Char Size 8 bit (UCSZ2:0 = 3)
	/* ------------------------------------------------------------*/
	UBRRH = 0;
	UBRRL = 12;
	UCSRA |= 0x02;
	UCSRB |= 0x98;
	UCSRC |= 0x86;
	
	// Enable Global Interrupt (BIT7 of SREG)
	sei();
	
	// Send a string to Smartphone
	UART_SendString((unsigned char *)"Hello World!\r\n");
	
    while (1) {
		
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
	while (*str) {
		USART_Transmit(*str);
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
	// Wait for empty transmit buffer 
	while (!( UCSRA & (1<<UDRE)));
	
	// Put data into buffer, sends the data
	UDR = c;
}

/* ------------------------------------------------------

 *  Function: void ExecuteCommand(void)
 
 *	This function is called upon received a full command (4 bytes data) 
	and process data according to command and change the state of LEDs
	according to command
	
 *						Arguments
		None

 *						Return
		None	
 -----------------------------------------------------*/
void ExecuteCommand(void) {
	// Check if the first data of command is 'L'
	if (cmd[0] == 'L') {
		// n i.e cmd[1] should be '1' to control LED1, so check it first
		if ( cmd[1] == '1' ) {
			// If d i.e. cmd[3] is equal to '1' then turn LED1 ON
			if ( cmd[3] == '1' )
				PORTC |= 0x01;
			// Else turn LED1 OFF
			else 
				PORTC &= ~0x01;
			
			// Send OK as feedback to Smartphone			
			UART_SendString((unsigned char *)"OK\r\n");
		}
		
		// n i.e cmd[1] should be '2' to control LED2, so check it first
		else if (cmd[1] == '2') {
			// If d i.e. cmd[3] is equal to '1' then turn LED2 ON
			if ( cmd[3] == '1' )
				PORTC |= 0x02;
			// Else turn LED2 OFF
			else
				PORTC &= ~0x02;
			
			// Send OK as feedback to Smartphone	
			UART_SendString((unsigned char *)"OK\r\n");
		}
		
		// n i.e cmd[1] is neither '1' nor '2', so Its a bad 
		// command for this implementation
		else {
			// Send "E02" error code as feedback to Smartphone
			UART_SendString((unsigned char *)"E02\r\n");
		}
		
	}
	
	// First character is not 'L', so Its a bad 
	// command for this implementation
	else {
		// Send "E01" error code as feedback to Smartphone
		UART_SendString((unsigned char *)"E01\r\n");
	}
}

// USART Receive Complete INT Service Routine
ISR(USART_RXC_vect) {
	// Check if receive complete INT flag is set
	if (UCSRA & (1<<RXC)) {
		// Copy data from USART buffer
		cmd[rx_index_ctn] = UDR;
		// Increment data index pointer
		rx_index_ctn++;
		// Check if all data received for a valid command
		if (rx_index_ctn == CMD_SIZE) {
			// Disable receive complete INT
			UCSRB &= ~(1<<RXCIE);
			// Execute the command send from Smartphone
			ExecuteCommand();
			// Set data index pointer = 0
			rx_index_ctn = 0;
			// Enable receive complete INT
			UCSRB |= (1<<RXCIE);
		}
	}
} 
