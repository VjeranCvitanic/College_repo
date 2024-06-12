/*
 * Zad5.c
 *
 * Created: 04/05/2024 12:20:39
 * Author : vjera
 */ 

#define F_CPU 3333333
#define BAUD_TWI 100000

#define SEG_A  (1 << 1)
#define SEG_B  (1 << 0)
#define SEG_C  (1 << 4)
#define SEG_D  (1 << 7)
#define SEG_E  (1 << 5)
#define SEG_F  (1 << 2)
#define SEG_G  (1 << 3)
#define SEG_DP (1 << 6)

#include <avr/io.h>
#include <util/delay.h>


void display(uint8_t msB, uint8_t lsB)
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
	

	for(uint8_t i = 0; i < 100; i++)
	{
		PORTE.OUT = PIN2_bm;
		PORTC.OUT = segment_codes[msB];
		_delay_ms(5);
		PORTE.OUT = PIN3_bm;
		PORTC.OUT = segment_codes[lsB];
		_delay_ms(5);
	}
}

void I2C_init()
{
	TWI0.MBAUD = (uint8_t)((F_CPU / 2 / BAUD_TWI) - 5);
	TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
	TWI0.MCTRLA = TWI_ENABLE_bm;
}

uint8_t I2C_start(uint8_t address, uint8_t rw)
{
	TWI0.MADDR = address << 1 | rw;				 //read = 1
	if(!rw)
	{
		while(!(TWI0.MSTATUS & (TWI_WIF_bm)));
		TWI0.MSTATUS |= TWI_WIF_bm;
		TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;
		return 2;
	}
	else
	{
		while(!(TWI0.MSTATUS & (TWI_RIF_bm)));
		TWI0.MSTATUS |= TWI_RIF_bm;
		return !(TWI0.MSTATUS & TWI_RXACK_bm);
	}
}

void I2C_stop()
{
	TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;
}

uint8_t I2C_write(uint8_t data)
{
	TWI0.MDATA = data;
	while(!(TWI0.MSTATUS) & (TWI_WIF_bm));
	TWI0.MSTATUS |= TWI_WIF_bm;
	TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;
	return !(TWI0.MSTATUS & TWI_RXACK_bm);
}

uint8_t I2C_read(uint8_t last_byte)
{
	while(!(TWI0.MSTATUS & TWI_CLKHOLD_bm));
	if(last_byte)
	{
		TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;
	}
	else
	{
		TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;
	}
	return TWI0.MDATA;
}

uint16_t read_half(uint8_t addr, uint8_t reg)
{
	uint8_t msB = 0;
	uint8_t lsB = 0;
	
	I2C_start(addr, 0);
	I2C_write(0x00);
	I2C_stop();
	I2C_start(addr, 1);
	msB = I2C_read(0);
	lsB = I2C_read(1);
	I2C_stop();
	
	return ((uint16_t)msB << 8) | lsB;
	//return msB;
}

int main(void)
{
	//heater
	uint8_t period = (F_CPU  / (16 * 1000)) - 1;

	PORTF.DIR |= 0x20;
	
	TCA0.SPLIT.CTRLD |= 0x01;
	TCA0.SPLIT.CTRLB |= 0x40;
	
	TCA0.SPLIT.CTRLA |= 0x60;
	
	TCA0.SPLIT.HPER = period;
	TCA0.SPLIT.HCMP2 = (uint8_t)((float)period * 0.99f);
	
	PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTF_gc;
	
	TCA0.SPLIT.CTRLA |= 0x01;
	//heater end

    uint16_t temp = 0;
	uint8_t msB, lsB;
	
	PORTE.DIRSET = PIN2_bm | PIN3_bm;
	PORTC.DIR = 0xff;
	
	I2C_init();
	
    while (1)
    {
		temp = read_half(0x48, 0x00);
		if(!(temp & 0x8000))
		{
			if (temp & 0x0080) 
			{											
				temp = ((temp & 0xFF00) >> 8) + 1;
			} 
			else 
			{			
				temp = ((temp & 0xFF00) >> 8);
			}
			//temp = (temp & 0xFF00 << 8) + temp & 0x00FF;
			msB = (uint8_t)temp/10;
			lsB = (uint8_t)temp%10;
			display(msB,lsB);
		}

		/*msB = (uint8_t)temp/10;
		lsB = (uint8_t)temp%10;
		display(msB,lsB);*/
    }
}

