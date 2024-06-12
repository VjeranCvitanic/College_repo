/*
 * Zadatak_2_cvitanic.c
 *
 * Created: 13/04/2024 15:42:04
 * Author : vjeran
 */ 

#define SAMPLE_NUM 100
#define F_CPU 3333333
#define BAUDRATE 115200

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

int usart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar, NULL, _FDEV_SETUP_WRITE);

int usart_putchar(char c, FILE * stream)
{
	while(!(USART3.STATUS & USART_DREIF_bm));
	USART3.TXDATAL = c;
	return 0;
}


int main(void)
{
    float fp_val = 0.f;
	uint32_t measures, result = 0;
	
	stdout = &mystdout;
	
	PORTD.DIRCLR = PIN0_bm;					//input
	
	ADC0.CTRLA = ADC_RESSEL_10BIT_gc;
	ADC0.CTRLA = ADC_ENABLE_bm;
	ADC0.CTRLC = ADC_PRESC_DIV4_gc;
	ADC0.CTRLC = ADC_REFSEL_VDDREF_gc;
	ADC0.MUXPOS = ADC_MUXPOS_AIN0_gc;
	
	USART3.BAUD = ((uint32_t)F_CPU * 64) / (16 * (uint32_t)BAUDRATE);	
	PORTB.DIR |= PIN0_bm;					// output
	USART3.CTRLB = USART_TXEN_bm;
	
    while (1) 
    {
		measures = 0;
		
		for(uint8_t i = 0; i < SAMPLE_NUM; i++)
		{
			ADC0.COMMAND = ADC_STCONV_bm;
			while(!(ADC0.INTFLAGS & ADC_RESRDY_bm));
			ADC0.INTFLAGS = ADC_RESRDY_bm;
			measures += ADC0.RES;
			_delay_ms(10);
		}
		
		fp_val = ((float)measures / SAMPLE_NUM);
		result = ((fp_val * 3.3 / (1023)) * 1000);
		printf("RAW value: %4d Voltage: %d mV\r\n", (int)fp_val, result);
		_delay_ms(2000);
    }
}

