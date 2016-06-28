
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "lcd_lib.h"

static void LCDMix_4Bit(uint8_t data);

static void LCDMix_4Bit(uint8_t data)
{
  if((data) & (0b10000000)) 
  {
	  LDPD7 |= 1 << LCD_D7;
  }	  
  else
  {
	  LDPD7 &= ~(1 << LCD_D7);
  } 

  if((data) & (0b01000000)) 
  {
	  LDPD6 |= 1 << LCD_D6;
  }
  else
  {
	  LDPD6 &= ~(1 << LCD_D6);
  }
	  
  if((data) & (0b00100000))
  {
	  LDPD5 |= 1 << LCD_D5;
  }
  else
  {
	  LDPD5 &= ~(1 << LCD_D5);
  }
  
  if((data) & (0b00010000))
  {
	  LDPD4 |= 1 << LCD_D4;
  }
  else LDPD4 &= ~(1 << LCD_D4);	
}

void LCDinit(void)	//Initializes LCD 
{
	_delay_ms(15);
		
	//zero to data pins
	LCDMix_4Bit(0b00000000);

	//zero to control pins
	LDPRS &= ~(1 << LCD_RS);
	LDPRW &= ~(1 << LCD_RW);
	LDPE &= ~(1 << LCD_E);
	
	//set data direction
	LDDD4 |= 1 << LCD_D4;
	LDDD5 |= 1 << LCD_D5;
	LDDD6 |= 1 << LCD_D6;
	LDDD7 |= 1 << LCD_D7;
		
	//control direction pins
	LDDRS |= 1 << LCD_RS;
	LDDRW |= 1 << LCD_RW;
	LDDE |= 1 << LCD_E;
	
	//---------one------
	LCDMix_4Bit(0b00110000);
	LDPE |= 1 << LCD_E;
	_delay_ms(1);
	
	LDPE &= ~(1 << LCD_E);
	_delay_ms(1);
		
	//-----------two-----------
	LCDMix_4Bit(0b00110000);
	LDPE |= 1 << LCD_E;
	_delay_ms(1);
	
	LDPE &= ~(1 << LCD_E);
	_delay_ms(1);
	
	//-------three-------------
	LCDMix_4Bit(0b00100000);
	LDPE |= 1 << LCD_E;		
	_delay_ms(1);
		
	LDPE &= ~(1 << LCD_E);
	_delay_ms(1);
		
	//--------4 bit--dual line---------------
	LCDsendCommand(0b00101000);
	
	//-----increment address, invisible cursor shift------
	LCDsendCommand(0b00001100);
}

void LCDsendChar(uint8_t ch)	//Sends Char to LCD
{
	LCDMix_4Bit(ch & 0b11110000);
	LDPRS |= 1 << LCD_RS;
	LDPE |= 1 << LCD_E;
	_delay_ms(1);

	LDPE &= ~(1 << LCD_E);
	LDPRS &= ~(1 << LCD_RS);
	_delay_ms(1);
		
	LCDMix_4Bit((ch & 0b00001111) << 4);
	LDPRS |= 1 << LCD_RS;
	LDPE |= 1 << LCD_E;
	_delay_ms(1);
	
	LDPE &= ~(1 << LCD_E);
	LDPRS &= ~(1 << LCD_RS);
	_delay_ms(1);
}

void LCDsendCommand(uint8_t cmd)	//Sends Command to LCD
{
	LCDMix_4Bit(cmd & 0b11110000);
	LDPE |= 1 << LCD_E;		
	_delay_ms(1);
		
	LDPE &= ~(1 << LCD_E);
	_delay_ms(1);

	LCDMix_4Bit((cmd & 0b00001111) << 4);
	LDPE |= 1 << LCD_E;		
	_delay_ms(1);
		
	LDPE &= ~(1 << LCD_E);
	_delay_ms(1);
}	
	
void LCDclr(void)	//Clears LCD
{
	LCDsendCommand(1 << LCD_CLR);
}

void LCDhome(void)	//LCD cursor home
{
	LCDsendCommand( 1 << LCD_HOME);
}

void LCDstring(uint8_t* data, uint8_t nBytes)	//Outputs string to LCD
{
	register uint8_t i;		//check to make sure we have a good pointer
	
	if(!data)
	{
		return;
	}
	// print data
	for(i = 0; i < nBytes; i++)
	{
		LCDsendChar(data[i]);
	}
}

void LCDGotoXY(uint8_t x, uint8_t y)	//Cursor to X Y position
{
	register uint8_t DDRAMAddr;		//remap lines into proper order
	
	switch(y)
	{
		case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR + x; break;
		case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR + x; break;
		case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR + x; break;
		case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR + x; break;
		default: DDRAMAddr = LCD_LINE0_DDRAMADDR + x;
	}
	
	LCDsendCommand((1 << LCD_DDRAM) | DDRAMAddr);	//set data address
}

//Copies string from flash memory to LCD at x y position
//const uint8_t welcomeln1[] PROGMEM="AVR LCD DEMO\0";
//CopyStringtoLCD(welcomeln1, 3, 1);
void CopyStringtoLCD(const uint8_t *FlashLoc, uint8_t x, uint8_t y)
{
	uint8_t i;

	LCDGotoXY(x, y);
	
	for(i = 0; (uint8_t)pgm_read_byte(&FlashLoc[i]); i++)
	{
		LCDsendChar((uint8_t)pgm_read_byte(&FlashLoc[i]));
	}
}

void LCDdefinechar(const uint8_t *pc,uint8_t char_code)
{
	uint8_t a, pcc;
	uint16_t i;

	a = (char_code << 3) | 0x40;
	
	for(i = 0; i < 8; i++)
	{
		pcc = pgm_read_byte(&pc[i]);
		LCDsendCommand(a++);
		LCDsendChar(pcc);
	}
}

void LCDshiftLeft(uint8_t n)	//Scroll n of characters Right
{
	for(uint8_t i = 0; i < n; i++)
	{
		LCDsendCommand(0x1E);
	}
}

void LCDshiftRight(uint8_t n)	//Scroll n of characters Left
{
	for(uint8_t i = 0; i < n; i++)
	{
		LCDsendCommand(0x18);
	}
}

void LCDcursorOn(void)	//displays LCD cursor
{
	LCDsendCommand(0x0E);
}

void LCDcursorOnBlink(void)		//displays LCD blinking cursor
{
	LCDsendCommand(0x0F);
}

void LCDcursorOFF(void)		//turns OFF cursor
{
	LCDsendCommand(0x0C);
}

void LCDblank(void)		//blanks LCD
{
	LCDsendCommand(0x08);
}

void LCDvisible(void)	//Shows LCD
{
	LCDsendCommand(0x0C);
}

void LCDcursorLeft(uint8_t n)	//Moves cursor by n positions left
{
	for(uint8_t i = 0; i < n; i++)
	{
		LCDsendCommand(0x10);
	}
}

void LCDcursorRight(uint8_t n)	//Moves cursor by n positions left
{
	for(uint8_t i = 0; i < n; i++)
	{
		LCDsendCommand(0x14);
	}
}


void LCDWriteInt(int val, unsigned int field_length)  ///////////////////
{
	char str[5] = {0, 0, 0, 0, 0};
	int i = 4;
	
	while(val)
	{
		str[i] = val % 10;
		val = val / 10;
		i--;
	}
	
	for(i = (5 - field_length); i < 5; i++)
	{
		LCDsendChar(48 + str[i]);
	}	
}

void LCDWriteString(const char *msg)
{
	/*****************************************************************	
	This function Writes a given string to lcd at the current cursor location.
	Arguments:
	msg: a null terminated string to print
	*****************************************************************/
	while(*msg!='\0')
	{
		LCDsendChar(*msg);
		msg++;
	}
}