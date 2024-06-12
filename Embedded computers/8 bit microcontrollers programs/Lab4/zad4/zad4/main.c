/*
 * zad4.c
 *
 * Created: 03/05/2024 23:36:54
 * Author : vjera
 */ 

#define F_CPU 3333333
#include <avr/io.h>


int main(void)
{
	uint8_t period = (F_CPU  / (16 * 1000)) - 1;
	
    PORTF.DIR |= 0x20;
	
	TCA0.SPLIT.CTRLD |= 0x01;
	TCA0.SPLIT.CTRLB |= 0x40;
	
	TCA0.SPLIT.CTRLA |= 0x60;
	
	TCA0.SPLIT.HPER = period;
	TCA0.SPLIT.HCMP2 = (uint8_t)((float)period * 0.1f);
	PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTF_gc;
	
	TCA0.SPLIT.CTRLA |= 0x01;
		
    while (1) 
    {
    }
}

