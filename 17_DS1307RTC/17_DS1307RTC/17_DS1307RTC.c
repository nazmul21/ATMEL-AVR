/*
 * ---------------------------------------------------------------- *
 	Project Name: Real Time Clock (RTC) on AVR MCU using DS1307
	
	Brief: 	This project aims to design a RTC clock and print time and data on a 
			16x2 LCD module. This project also features the time and date setting
			from Smartphone wirelessly using HC-05 Bluetooth Module
			
	Note: Set Command Format:
		  Time: "Tamhhmmss" where am (1 byte) = '0' for AM, and '1' for PM
								  hh* (1 byte) is hour range 01 - 12 
								  mm* (1 byte) is minute range 00 - 59 
								  ss* (1 byte) is second range 00 - 59 
								  
		  Date: "Ddaydatemonyear" where day* (1 byte) range 01 - 07 (0x01 = Sat, 0x02 = Sun and so on)
										data* (1 byte) range 01 -31 
										mon* (1 byte) range 01 - 12 
										year* (1 byte) range 00 - 99 
										
		  * BCD for like DS1307
		  It's better to send hex value for those command 
		  
		  Example: 1) 0x54 0x01 0x11 0x59 0x30 (No space allowed, 5 bytes of command)
				   This command will set time of DS1307 module as follows
				   Hour = 11, Minutes = 59, Second = 30 and AM/PM = PM
					
				   2) 0x44 0x02 0x28 0x02 0x16 (No space allowed, 5 bytes of command)
				   // This command will set date of DS1307 module as follows
				   Day = Sun, date = 28, month = Feb, Year = 16 (we can treat 2016 in our software)
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: TWI (PC0, PC1) USART (PD0, PD1), PORTD (PD6:PD4), PORTC (PC7:PC4)
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
		
		PC0	(Ext pull-up)					DS1307 SCL
		PC1 (Ext pull-up)					DS1307 SDA
		PD4									LCD RS 
		PD5									LCD R/W
		PD6									LCD E
		PC7:PC4								LCD Data (D7:D4)
		PD0									TXD of HC-05
		PD1									RXD of HC-05
		GND									GND of HC-05
			
	NB: MCU Power pins, RESET pin should connect properly
		
 * ---------------------------------------------------------------- *
 
 */ 

/* --------------------- Defined Symbols --------------------------*/
#define CMD_SIZE	5
/* ----------------------------------------------------------------*/ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "userlib/i2c_lib.h"
#include "userlib/ds1307_lib.h"
#include "userlib/lcd_lib.h"

/* ------------------ Function Prototypes -------------------------*/
void UART_SendString(unsigned char *str);
void USART_Transmit(unsigned char c);
void Set_DS1307(void);
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
volatile unsigned char setFlag = 0;
unsigned char cmd[CMD_SIZE] = {0};
unsigned char rx_index_ctn = 0;
/* ----------------------------------------------------------------*/

int main(void) {
	unsigned char ch, status;
	unsigned char sec, min, hour, am;
	unsigned char date, mm, day, year;
	
	// Power up delay to stabilize LCD
	_delay_ms(250);
	
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
	
	// Initialization of I2C hardware to communicate with DS1307
	I2CInit();
	
	// Initialize LCD, print welcome message, wait 1 secs
	LCDinit();
	LCDWriteStringXY(2, 0, "DS1307 RTC");
	_delay_ms(1000);
	LCDclr();
	
	// Clock Enable for DS1307 
	status = DS1307Read(0x00, &ch);
	if(status == FALSE) {
		LCDWriteStringXY(2, 0, "Error");
		while(1);
	}
	ch &= ~0x80;
	status = DS1307Write(0x00, ch);
	if(status == FALSE) {
		LCDWriteStringXY(2, 0, "Error");
		while(1);
	}
	
	// Set 12 (bit 6 of hour register) Hour Mode for DS1307
	status = DS1307Read(0x02, &hour);
	if(status == FALSE) {
		LCDWriteStringXY(2, 0, "Error");
		while(1);
	}
	status = DS1307Write(0x00, (hour | 0x40) );
	if(status == FALSE) {
		LCDWriteStringXY(2, 0, "Error");
		while(1);
	}
	
	// Enable Global Interrupt (BIT7 of SREG)
	sei();
	
	// Send a string to Smartphone
	UART_SendString((unsigned char *)"Hello World!\r\n");
	
    while (1) {
		
		// Read second from DS1307 second register (0x00) 
		DS1307Read(0x00, &sec);
		
		// Read minute from DS1307 minute register (0x01)
		DS1307Read(0x01, &min);
		
		// Read hour from DS1307 hour register (0x02)
		DS1307Read(0x02, &hour);
		
		// Read day from DS1307 day register (0x03)
		DS1307Read(0x03, &day);
		
		// Read date from DS1307 date register (0x04)
		DS1307Read(0x04, &date);
		
		// Read month from DS1307 month register (0x05)
		DS1307Read(0x05, &mm);
		
		// Read year from DS1307 yaer register (0x06)
		DS1307Read(0x06, &year);
		
		// Check for AM/PM, bit 5 of hour register (0x02) of DS1307
		if (hour & 0x20)
			am = 1;
		else
			am = 0;
		
		// Convert BCD data to decimal to print on LCD
		sec = (((sec & 0x70) >> 4) * 10) + (sec & 0x0F);
		min = (((min & 0x70) >> 4) * 10) + (min & 0x0F);
		hour = (((hour & 0x10) >> 4) * 10) + (hour & 0x0F);
		date = (((date & 0x30) >> 4) * 10) + (date & 0x0F);
		mm = (((mm & 0x10) >> 4) * 10) + (mm & 0x0F);
		year = (((year & 0xF0) >> 4) * 10) + (year & 0x0F);
		
		// Print Decimal data on LCD
		LCDWriteIntXY( 2,0, hour, 2);
		LCDWriteString(":");
		LCDWriteInt(min, 2);
		LCDWriteString(":");
		LCDWriteInt(sec, 2);
		if(am) {
			LCDWriteString(" PM");
		}
		else {
			LCDWriteString(" AM");
		}
		LCDWriteIntXY( 2, 1, date, 2);
		LCDWriteString("/");
		LCDWriteInt(mm, 2);
		LCDWriteString("/");
		LCDWriteInt(year, 2);
		
		// Print day corresponding value of day register (0x03)
		switch (day & 0x07) {
			case 1:
				LCDWriteString(" Fri");
				break;
			case 2:
				LCDWriteString(" Sat");
				break;
			case 3:
				LCDWriteString(" Sun");
				break;
			case 4:
				LCDWriteString(" Mon");
				break;
			case 5:
				LCDWriteString(" Tue");
				break;
			case 6:
				LCDWriteString(" Wed");
				break;
			case 7:
				LCDWriteString(" Thu");
				break;
		}
		
		// If command receive via USART then call the Set_DS1307 fuction
		// to set date or time for DS1307
		if ( setFlag ) {
			// Set time/date for DS1307
			Set_DS1307();
			// Clear set flag
			setFlag = 0;
			// Enable USART Receive Complete INT
			UCSRB |= (1<<RXCIE);
		}
		_delay_ms(250);
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

 *  Function: void Set_DS1307(void)
 
 *	This function is called upon received a full command (5 bytes data) 
	and process data according to command and set time/date of DS1307
	RTC according to command.
	
	NOTE: This function doesn't check for any invalid input, so send 
		  data to set DS1307 module carefully 
	
 *						Arguments
		None				

 *						Return
		None	
 -----------------------------------------------------*/
void Set_DS1307(void) {
	
	// Check if the command send to set the time for DS1307
	if (cmd[0] == 'T') {
		unsigned char s_hr, s_min, s_ss, s_am;
		
		// copy data from command to local variable
		s_am = cmd[1];
		s_hr = cmd[2];
		s_min = cmd[3];
		s_ss = cmd[4];
		
		// Set AM/PM bit (bit 5) of hour register of DS1307
		if ( s_am == 1 ) {
			s_hr |= 0x20;
		}
		else {
			s_hr &= ~0x20;
		}
			
		// Set 12 (bit 6 of hour register) Hour Mode for DS1307
		s_hr |= 0x40;
		
		// Send data to DS1307 via TWI
		DS1307Write(0x00, s_ss);
		DS1307Write(0x01, s_min);
		DS1307Write(0x02, s_hr);
		
		// Send OK as feedback to Smartphone
		UART_SendString((unsigned char *)"OK\r\n");			 
	}
	
	// Check if the command send to set the date for DS1307
	else if (cmd[0] == 'D') {
		unsigned char s_day, s_date, s_mm, s_year;
			
		// copy data from command to local variable
		s_day = cmd[1];
		s_date = cmd[2];
		s_mm = cmd[3];
		s_year = cmd[4];
			
		// Send data to DS1307 via TWI
		DS1307Write(0x03, s_day);
		DS1307Write(0x04, s_date);
		DS1307Write(0x05, s_mm);
		DS1307Write(0x06, s_year);
			
		// Send OK as feedback to Smartphone
		UART_SendString((unsigned char *)"OK\r\n");
	}
	
	// Invalid command send from Smartphone
	else {
		// Send "Error" code as feedback to Smartphone
		UART_SendString((unsigned char *)"Error\r\n");
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
			// Set data index pointer = 0
			rx_index_ctn = 0;
			// Set setFlag = 1
			setFlag = 1;
		}
	}
}


