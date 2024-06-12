/*
 * Zadatak_3_Cvitanic.c
 *
 * Created: 13/04/2024 16:25:38
 * Author : vjeran
 */ 

#define F_CPU 3333333

#include <avr/io.h>
#include <util/delay.h>

#define SEG_A  (1 << 1)
#define SEG_B  (1 << 0) 
#define SEG_C  (1 << 4) 
#define SEG_D  (1 << 7) 
#define SEG_E  (1 << 5) 
#define SEG_F  (1 << 2) 
#define SEG_G  (1 << 3) 
#define SEG_DP (1 << 6) 



int main(void)
{
	const unsigned char segment_codes[10] = { SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // 0
		SEG_B | SEG_C,         // 1
		SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,  // 2
		SEG_A | SEG_B | SEG_C | SEG_D | SEG_G,  // 3
		SEG_F | SEG_G | SEG_B | SEG_C,    // 4
		SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,   // 5
		SEG_A | SEG_F | SEG_G | SEG_C | SEG_D | SEG_E, // 6
		SEG_A | SEG_B | SEG_C,       // 7
		SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, // 8
		SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G}; // 9
		
		
		PORTE.DIRSET = PIN2_bm | PIN3_bm;
		PORTC.DIR = 0xff;
		
		while(1)
		{
			for(uint8_t cnt_1 = 0; cnt_1 < 10; cnt_1++)
			{
				for(uint8_t cnt_2 = 0; cnt_2 < 10; cnt_2++)
				{
					for(uint8_t i = 0; i < 100; i++)
					{
						PORTE.OUT = PIN2_bm;
						PORTC.OUT = segment_codes[cnt_1];
						_delay_ms(5);
						PORTE.OUT = PIN3_bm;
						PORTC.OUT = segment_codes[cnt_2];
						_delay_ms(5);
					}
				}
			}
		}
}

