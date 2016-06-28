/************************************************************
Library functions to access the popular DS1307 RTC IC with AVR
Micro controller. 
The library has just two functions. One reads the register whose
address is provided. Other writes to the given register with
given data. Please see DS1307 Data sheet for more info on
the registers. 
Requires eXtreme Electronics Low Level I2C Library. 
************************************************************/
#include <avr/io.h>
#include "i2c_lib.h"
#include "ds1307_lib.h"

#define DS1307_Addr	0x68

#define I2C_WRITE	0x00
#define I2C_READ	0x01

/***************************************************
Function To Read Internal Registers of DS1307
---------------------------------------------
address: Address of the register 
data: value of register is copied to this. 

Returns:
0 = Failure
1 = Success
***************************************************/

uint8_t DS1307Read(uint8_t address, uint8_t *data)
{
	uint8_t res;	//result
	
	//Start
	res = I2CStart();
	if (res == FALSE)
		return FALSE;
	
	//SLA + W (for dummy write to set register pointer)
	res = I2CWriteByte( (DS1307_Addr<<1) |  I2C_WRITE);	//DS1307 address + W
	
	//Error
	if(res == FALSE)	
		return FALSE;
	
	//Now send the address of required register
	res = I2CWriteByte(address);
	
	//Error
	if(res == FALSE)	
		return FALSE;
	
	//Repeat Start
	res = I2CStart();
	
	// Error
	if(res == FALSE)
		return FALSE;
		
	//SLA + R
	res = I2CWriteByte((DS1307_Addr<<1) | I2C_READ);	//DS1307 Address + R
	
	//Error
	if(res == FALSE)	
		return FALSE;
	
	//Now read the value with NACK
	res = I2CReadByte(data, 0);
	
	//Error
	if(res == FALSE)	
		return FALSE;
	
	//STOP
	I2CStop();
	
	return TRUE;
}

/***************************************************
Function To Write Internal Registers of DS1307
---------------------------------------------
address: Address of the register
data: value to write.

Returns:
0= Failure
1= Success
***************************************************/

uint8_t DS1307Write(uint8_t address, uint8_t data)
{
	uint8_t res;	//result
	
	//Start
	res = I2CStart();
	
	// Error
	if(res == FALSE)
		return FALSE;
	
	//SLA+W
	res = I2CWriteByte((DS1307_Addr<<1) |  I2C_WRITE);	//DS1307 address + W
	
	//Error
	if(res == FALSE)	
		return FALSE;
	
	//Now send the address of required register
	res = I2CWriteByte(address);
	
	//Error
	if(res == FALSE)	
		return FALSE;
	
	//Now write the value
	res = I2CWriteByte(data);
	
	//Error
	if(res == FALSE)	
		return FALSE;
	
	//STOP
	I2CStop();
	
	return TRUE;
}