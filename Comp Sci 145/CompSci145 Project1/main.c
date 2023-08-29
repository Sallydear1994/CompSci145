/*
 * GccApplication3.c
 *
 * Created: 1/19/2022 12:15:03 AM
 * Author : willb
 */ 

#include "avr.h"
//#include "avr.c"


int main(void)
{
    /* Replace with your application code */
	DDRB = 1; // 00000001
    while (1) 
    {
		if (!(PINB & 2))
		//if(PINB & 2) // This basically causes it to start blinking regardless of the switch
		{
			PORTB = 1;
			//PORTB |= 1;
			avr_wait(500);
			//PORTB |= ~0;
			PORTB = 0;
			avr_wait(500);
		}
		else
		{
			PORTB = 0;
		}
    }
	return 0;
}

