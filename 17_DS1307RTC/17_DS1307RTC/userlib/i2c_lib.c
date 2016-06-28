/****************************************************
Low Level I2C Data Trans-receiving APIs
****************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "i2c_lib.h"

void I2CInit() {
	// Set up TWI Module
	TWBR = 30;
	TWSR |= (1 << TWPS1);
	
	// Enable the TWI Module
	TWCR |= (1 << TWEN);
}

void I2CClose() {
	//Disable the module
	TWCR &= (~(1 << TWEN));
}

uint8_t I2CStart() {
	//Put Start Condition on Bus
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

	//Poll Till Done
	while(!(TWCR & (1 << TWINT)));
	
	if ( ((TWSR & 0xF8) == 0x08) || ((TWSR & 0xF8) == 0x10) )
		return TRUE;
	else
		return FALSE;
}

void I2CStop() {
	//Put Stop Condition on bus
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	
	//Wait for STOP to finish
	while(TWCR & (1 << TWSTO));
}

uint8_t I2CWriteByte(uint8_t data) {	
	TWDR = data;

	//Initiate Transfer
	TWCR = (1 << TWEN) | (1 << TWINT);

	//Poll Till Done
	while(!(TWCR & (1 << TWINT)));

	//Check Status
	if((TWSR & 0xF8) == 0x18 || (TWSR & 0xF8) == 0x28 || (TWSR & 0xF8) == 0x40) {
		//SLA + W Transmitted and ACK received (0x18)
		//or
		//DATA Transmitted and ACK received (0x28)
		// or
		//SLA + R Transmitted and ACK received (0x40)
		return TRUE;
	}
	else {
		return FALSE;	//Error
	}		
}

uint8_t I2CReadByte(uint8_t *data, uint8_t ack) {
	//Set up ACK
	if(ack) {
		//return ACK after reception
		TWCR |= (1 << TWEA);
	}
	else {
		//return NACK after reception
		//Signals slave to stop giving more data
		//usually used for last byte read.
		TWCR &=(~(1 << TWEA));
	}
	
	//Now enable Reception of data by clearing TWINT
	TWCR |= (1 << TWINT);

	//Wait till done
	while(!(TWCR & (1 << TWINT)));

	//Check status
	if((TWSR & 0xF8) == 0x58 || (TWSR & 0xF8) == 0x50 ) {
		//Data received and NACK returned (0x58)
		//	or
		//Data received and ACK returned (0x50)
		//Read the data

		*data = TWDR;
		return TRUE;
	}
	else {
		return FALSE;	//Error
	}	
}