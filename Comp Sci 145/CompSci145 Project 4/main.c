#include <avr/io.h>
#include "avr.h"
#include "lcd.h"

enum VoltMeter_States {VM_Init, VM_Off, VM_On, VM_Reset} VM_State;

void kp_init()
{
	DDRC = 0;
	PORTC = 0;
}

int kp_is_pressed(int r, int c)
{
	// all 8 GPIO to N/C
	DDRC = 0;
	PORTC = 0;

	// set r to "0"
	// set c to "w1"
	SET_BIT(DDRC, r);
	SET_BIT(PORTC, 4 + c);
	if (GET_BIT(PINC, 4 + c) == 0)
	{
		return 1;
	}
	return 0;
}

int kp_get_key()
{
	int i, j;
	for (i = 0; i < 4; ++i)
	{
		for (j = 0; j < 4; ++j)
		{
			if (kp_is_pressed(i, j))
			{
				return i * 4 + j + 1;
			}
		}
	}
	return 0;
}

int get_sample()
{
	// configure the ADC
	ADMUX = 0b01000000; // Use A0 as pin input

	// start conversion
	ADCSRA = 0b11000000;

	// wait for conversion to compute
	while(GET_BIT(ADCSRA,6));

	//return result;
	return ADC;
}

void printLCD(int currVolt, int maxVolt, int minVolt, int sumVolt, int counter)
{
	char buf[5];

	if(counter > 0)
	{
		lcd_pos(0,0);
		dtostrf(currVolt * 5.0 / 1023.0, 4, 2, buf); // Instant Voltage
		lcd_puts2(buf);
		lcd_pos(0,7);
		dtostrf(maxVolt * 5.0 / 1023.0, 4, 2, buf); // Maximum Voltage
		lcd_puts2(buf);
		lcd_pos(1,0);
		dtostrf(minVolt * 5.0 / 1023.0, 4, 2, buf); // Minimum Voltage
		lcd_puts2(buf);
		lcd_pos(1,7);
		dtostrf(sumVolt / (float)counter * 5.0 / 1023.0, 4, 2, buf); // Average Voltage
		lcd_puts2(buf);
	}
	else
	{
		sprintf(buf, "-.--");
		lcd_pos(0,0);
		lcd_puts2(buf);
		lcd_pos(0,7);
		lcd_puts2(buf);
		lcd_pos(1,0);
		lcd_puts2(buf);
		lcd_pos(1,7);
		lcd_puts2(buf);
	}
}

void voltMeterRun()
{
	static int keyPress;

	static int minVolt = 1023;
	static int currVolt = 0;
	static int maxVolt = 0;
	static int counter = 0;
	static long sumVolt = 0;

	keyPress = kp_get_key();

	// KeyPad functions
	// 4(A) = run
	// 8(B) = pause
	// 12(C) = reset
	switch(VM_State) // Transition logic
	{
		case VM_Init:
			VM_State = VM_Off;
			break;
		case VM_Off:
			if (keyPress == 4)
			{
				VM_State = VM_On;
			}
			else if (keyPress == 12) // Reset mode
			{
				minVolt = 1023;
				sumVolt = 0;
				maxVolt = 0;
				counter = 0;
				VM_State = VM_Off;
			}
			else
			{
				VM_State = VM_Off;
			}
			break;
		case VM_On:
			if (keyPress == 8)
			{
				VM_State = VM_Off;
			}
			else if (keyPress == 12)
			{
				minVolt = 1023;
				sumVolt = 0;
				maxVolt = 0;
				counter = 0;
				VM_State = VM_Off;
			}
			else
			{
				VM_State == VM_On;
			}
			break;
		case VM_Reset: // This should not execute
			break;
		default:
			break;
	}

	switch(VM_State) // State logic
	{
		case VM_Init:
			break;
		case VM_Off:
			break;
		case VM_On:
			currVolt = get_sample();
			sumVolt += currVolt;
			counter += 1;
			if (currVolt < minVolt)
			{
				minVolt = currVolt;
			}
			if (currVolt > maxVolt)
			{
				maxVolt = currVolt;
			}
			break;
		case VM_Reset:
			break;
		default:
			break;
	}

	printLCD(currVolt, maxVolt, minVolt, sumVolt, counter);
}

int main(void)
{
    lcd_init();
	kp_init();
	lcd_pos(0,0);
	SET_BIT(DDRB,3);
    while (1) 
    {
		voltMeterRun();
		avr_wait(500);
    }
	return 0;
}

