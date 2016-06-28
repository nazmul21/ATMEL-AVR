/*
 * ---------------------------------------------------------------- *
 	Project Name: AVR Digital Voltmeter
				  AVR Thermometer using LM35 Temperature Sensor
	
	Brief: 	This project aims to measure analog voltage (MAX Input 5 VDC)
			as well as LM35 based thermometer using built-in ADC 
			peripheral and print the measured value on 16x2 LCD display
			
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
 	Target MCU: ATMega32A, F_CPU=1000000UL 
	High Fuse: 0xD9, Low Fuse: 0xE1
	Resource Used: ADC
 * ---------------------------------------------------------------- *
 
 * ---------------------------------------------------------------- *
							Connections  
		
		PA0 							Input voltage to be measured
		PA1								LM35 Output
		PD4									LCD RS 
		PD5									LCD R/W
		PD6									LCD E
		PC7:PC4								LCD Data (D7:D4)
			
	NB: MCU Power pins, RESET pin, Vref pin, LCD Power pins, LCD Contrast pin
		LCD Back-light LED pins should connect properly
		
 * ---------------------------------------------------------------- *
 
 */ 

/* --------------------- Defined Symbols --------------------------*/
#define SAMPLE 32
/* ----------------------------------------------------------------*/ 
 
#include <avr/io.h>
#include <util/delay.h>
#include "lcdlib/lcd_lib.h"

/* ------------------ Function Prototypes -------------------------*/
static uint16_t ReadADC(uint8_t chn);
/* ----------------------------------------------------------------*/

/* -------------------- Global Variables --------------------------*/
/* ----------------------------------------------------------------*/

int main(void) {
	
	// power up delay to stabilize LCd module
	_delay_ms(100);
	
	// Required local variables
	uint8_t vin_i = 0, vin_f = 0, temperature = 0;
	uint16_t adcres = 0, adc_lm35 = 0;
	double vin = 0.0;
	
	
	/* --------------------- ADC Initialization --------------------*/
	//	AVcc as ADC Ref (REFS1:0 = 1), Right Justified Result (ADLAR = 0)
	//  ADC Prescaler division factor 8 (ADPS2:0 = 3), ADC Clock = F_CPU / 8
	//  ADC Clock should be between 50 KHz to 200 KHz to get the MAX resolution 
	//  Enable ADC (ADEN = 1)
	/* ------------------------------------------------------------*/
	ADMUX = 0x40;
	ADCSRA = 0x83;
	
	// Initialize the LCD module, print welcome message and wait for 2 seconds
	LCDinit();
	LCDWriteStringXY(0, 0, "ADC Example");
	_delay_ms(2000);
	
    while (1) {
		
		// Voltmeter Example @ Channel 0 (PA0)
		// Collect ADC Samples
		adcres = 0;
		for (uint8_t s = 0; s < SAMPLE; s++) {
			adcres += ReadADC(0);
		}
		// Get the Average Value of samples
		adcres = adcres/SAMPLE;
		
		// Convert ADC value to original input (Analog) voltage
		//					ADC * Vref (5.0)	
		//			Vin = --------------------
		// 						1024
		
		vin = (double) ((adcres * 5.0)/1024);
		
		// Copy integer part of the result
		vin_i = (uint8_t) vin;
		// Copy fraction part of the result (2 digits) 
		vin_f = (uint8_t)((vin - vin_i) * 100);
		
		// Print values on the LCD
		LCDWriteIntXY(2, 1, vin_i, 1);
		LCDWriteString(".");
		LCDWriteIntXY(4, 1, vin_f, 2);
		LCDWriteStringXY(7, 1, "V");
		_delay_ms(100);
		
		
		// Temperature Sensor LM35 @ Channel 1 (PA1)
		// Collect ADC Samples
		adc_lm35 = 0;
		for (uint8_t s = 0; s < SAMPLE; s++) {
			adc_lm35 += ReadADC(1);
		}
		
		// Get the Average Value of samples
		adc_lm35 = adc_lm35/SAMPLE;
		
		// Convert ADC to corresponding temperature (T) value (10 mV/*C)
		//			    Vin		  ADC * Vref 	   ADC * 5.0		 ADC   ~   ADC
		//  	T  = -------- = -------------- =  ------------- = -------- =  -----
		// 			   10 mV 	 10 mV * 1024	   0.01 * 1024		2.048	    2
		
		temperature = (uint8_t) (adc_lm35/2);
		
		// Print values on the LCD
		LCDWriteIntXY(9, 1, temperature, 2);
		LCDWriteStringXY(12, 1, "*C");
		_delay_ms(100);
    }
}

/* ------------------------------------------------------

 *  Function: static uint16_t ReadADC(uint8_t chn)
 
 *	Used to get ADC value from desired Input channel
	
 *						Arguments
		uint8_t chn (7:0) 			Input Channel (AIN7:AIN0)

 *						Return
		uint16_t result 			ADC result
		0 							if chn > 7
 --------------------------------------------------------*/

static uint16_t ReadADC(uint8_t chn) {
	
	uint16_t result = 0;
	
	// If Channel greater than 7 then return 0
	// Only for single channel operation
	if (chn > 7)
		return result;

	// Select the desired channel with ADMUX register 
	ADMUX |= (chn & 0x07);
	
	// Start the conversion
	ADCSRA |= (1<<ADSC);
	
	// Wait until the conversion finished 
	while( (ADCSRA & (1<<ADSC)));
	
	// Copy ADC result to local variable
	result = ADCL;
	result |= (uint16_t) (ADCH<<8);
	
	
	// De-select the ADC channel
	ADMUX &= ~0x1F;
	
	// Return the ADC result
	return result;
}
