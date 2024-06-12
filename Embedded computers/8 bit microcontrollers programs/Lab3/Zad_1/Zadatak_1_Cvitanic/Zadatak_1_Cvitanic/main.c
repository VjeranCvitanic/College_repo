/*
 * GccApplication1.c
 *
 * Created: 13/04/2024 14:22:45
 * Author : vjeran
 */ 

#define F_CPU 3333333
#define BAUDRATE 19200

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*#define PA1_LOW							!(PORTA.IN & PIN7_bm)
#define PA1_INTERRUPT					PORTA.INTFLAGS & PIN7_bm
#define PA1_CLEAR_INTERRUPT_FLAG		PORTA.INTFLAGS |= PIN7_bm

#define PA2_LOW							!(PORTA.IN & PIN6_bm)
#define PA2_INTERRUPT					PORTA.INTFLAGS & PIN6_bm
#define PA2_CLEAR_INTERRUPT_FLAG		PORTA.INTFLAGS |= PIN6_bm

#define PA3_LOW							!(PORTA.IN & PIN5_bm)
#define PA3_INTERRUPT					PORTA.INTFLAGS & PIN5_bm
#define PA3_CLEAR_INTERRUPT_FLAG		PORTA.INTFLAGS |= PIN5_bm

#define PA4_LOW							!(PORTA.IN & PIN4_bm)
#define PA4_INTERRUPT					PORTA.INTFLAGS & PIN4_bm
#define PA4_CLEAR_INTERRUPT_FLAG		PORTA.INTFLAGS |= PIN4_bm


ISR(PORTA_PORT_vect)
{	
	interrupt_signal = 1;
	if(PA1_INTERRUPT)
	{
		pa[0] = 1;
		pa[1] = 0;
		pa[2] = 0;
		pa[3] = 0;
		PA1_CLEAR_INTERRUPT_FLAG;
	}
	else if(PA2_INTERRUPT)
	{
		pa[0] = 0;
		pa[1] = 1;
		pa[2] = 0;
		pa[3] = 0;
		PA2_CLEAR_INTERRUPT_FLAG;
	}
	else if(PA3_INTERRUPT)
	{
		pa[0] = 0;
		pa[1] = 0;
		pa[2] = 1;
		pa[3] = 0;
		PA3_CLEAR_INTERRUPT_FLAG;
	}
	else if(PA4_INTERRUPT)
	{
		pa[0] = 0;
		pa[1] = 0;
		pa[2] = 0;
		pa[3] = 1;
		PA4_CLEAR_INTERRUPT_FLAG;
	}
	else
	{
		interrupt_signal = 0;
	}
}


volatile uint8_t pa[4] = {0};
volatile uint8_t interrupt_signal = 0;*/

void turn_all_off()
{
	PORTD.OUT &= ~(1 << 7);
	PORTD.OUT &= ~(1 << 6);
	PORTD.OUT &= ~(1 << 5);
	PORTD.OUT &= ~(1 << 4);
}

int main(void)
{
	PORTD.DIRSET = 0xF0;
		
	PORTA.DIRCLR = 0xF0;
	
	PORTA.PIN7CTRL |= PORT_PULLUPEN_bm;
	PORTA.PIN6CTRL |= PORT_PULLUPEN_bm;
	PORTA.PIN5CTRL |= PORT_PULLUPEN_bm;
	PORTA.PIN4CTRL |= PORT_PULLUPEN_bm;
	
	while(1)
	{
		if((PORTA.IN & (1 << 7)) == 0)
		{
			turn_all_off();
			PORTD.OUT |= (1 << 7);
		}
		else if((PORTA.IN & (1 << 6)) == 0)
		{
			turn_all_off();
			PORTD.OUT |= (1 << 6);
		}
		else if((PORTA.IN & (1 << 5)) == 0)
		{
			turn_all_off();
			PORTD.OUT |= (1 << 5);
		}
		else if((PORTA.IN & (1 << 4)) == 0)
		{
			turn_all_off();
			PORTD.OUT |= (1 << 4);
		}
	}
	
	//sei();
	/*while(1)
	{
		if(interrupt_signal == 1)
		{
			if(pa[0] == 1)
			{
				PORTD.OUT |= (1 << 7);
				pa[0] = 0;
			}
			else if(pa[1] == 1)
			{
				PORTD.OUT |= (1 << 6);
				pa[1] = 0;
			}
			else if(pa[2] == 1)
			{
				PORTD.OUT |= (1 << 5);
				pa[2] = 0;
			}
			else if(pa[3] == 1)
			{
				PORTD.OUT |= (1 << 4);
				pa[3] = 0;
			}
			interrupt_signal = 0;
		}
	}*/
}

