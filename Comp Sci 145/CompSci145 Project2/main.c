#include "avr.h"
#include "lcd.h"
#include <stdio.h>

enum Clock_States {Clock_Init, Clock_Set, Clock_Run} Clock_State;

struct dt{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
};

void init_dt(struct dt *dt)
{
	dt->year = 2022;
	dt->month = 1;
	dt->day = 1;
	dt->hour = 0;
	dt->minute = 0;
	dt->second = 0;
}

int get_days(struct dt *dt)
{
	int days;
	switch(dt->month)
	{
		case 1:
			days = 31;
			break;
		case 2: // leap year is taken into account
			if (dt->year % 400 == 0)
			{
				days = 29;
			}
			else if (dt->year % 4 == 0 && dt->year % 100 != 0)
			{
				days = 29;
			}
			else // not a leap year
			{
				days = 28;
			}
			break;
		case 3:
			days = 31;
			break;
		case 4:
			days = 30;
			break;
		case 5:
			days = 31;
			break;
		case 6:
			days = 30;
			break;
		case 7:
			days = 31;
			break;
		case 8:
			days = 31;
			break;
		case 9:
			days = 30;
			break;
		case 10:
			days = 31;
			break;
		case 11:
			days = 30;
			break;
		case 12:
			days = 31;
			break;
		default: // This should never be executed
			days = 0;
			break;
	}
	return days;
}

void check_dt(struct dt *dt)
{
	if (dt->second == 60)
	{
		++dt->minute;
		dt->second = 0;
	}
	else if (dt->second > 60)
	{
		dt->second = 0;
	}

	if (dt->minute == 60)
	{
		++dt->hour;
		dt->minute = 0;
	}
	else if (dt->minute > 60)
	{
		dt->minute = 0;
	}

	if (dt->hour == 24)
	{
		dt->hour = 0;
		(dt->day) += 1;
	}
	else if (dt->hour > 24)
		dt->hour = 0;

	int maxDays = get_days(dt);
	if (dt->day == maxDays + 1)
	{
		++dt->month;
		dt->day = 1;
	}
	else if (dt->day == 0)
	{
		dt->day = 1;
	}
	else if (dt->day > maxDays + 1)
	{
		dt->day = 1;
	}

	if (dt->month > 13)
	{
		dt->month = 1;
	}
	else if (dt->month == 13)
	{
		++dt->year;
		dt->month = 1;
	}
	else if (dt->month == 0)
	{
		dt->month = 1;
	}

	if (dt->year == 0 || dt->year > 9999)
		dt->year = 2022;
}

void kp_init()
{
	DDRC = 0;
	PORTC = 0;
}

int kp_is_pressed(int r, int c)
{
	// all 8 GPIOs to N/C
	DDRC = 0;
	PORTC = 0;

	// set r to "0"
	// set c to "w1"
	SET_BIT(DDRC, r);
	SET_BIT(PORTC, 4 + c);
	if(GET_BIT(PINC, 4 + c) == 0) // if button is pressed, wait 1 sec for returning
	{
		avr_wait(1000);
		return 1;
	}
	else
	{
		return 0;
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

void advance_dt(struct dt *dt)
{
	dt->second += 1;
	check_dt(dt);
}

void print_dt(const struct dt *dt)
{
	char buf[17];
	lcd_pos(0,0);
	sprintf(buf, "%02d/%02d/%04d", dt->month, dt->day, dt->year);
	lcd_puts2(buf);
	lcd_pos(1,0);
	sprintf(buf, "%02d:%02d:%02d", dt->hour, dt->minute, dt->second);
	lcd_puts2(buf);
}

void moveUp(int *cursorLoc)
{
	switch (*cursorLoc)
	{
		case 0:
			*cursorLoc = 8;
			break;
		case 1: 
			*cursorLoc = 9;
			break;
		case 2:
			*cursorLoc = 10;
			break;
		case 3:
			*cursorLoc = 11;
			break;
		case 4:
			*cursorLoc = 12;
			break;
		case 5:
			*cursorLoc = 13;
			break;
		case 6:
			*cursorLoc = 13;
			break;
		case 7:
			*cursorLoc = 13;
			break;
		case 8:
			*cursorLoc = 0;
			break;
		case 9:
			*cursorLoc = 1;
			break;
		case 10:
			*cursorLoc = 2;
			break;
		case 11:
			*cursorLoc = 3;
			break;
		case 12:
			*cursorLoc = 4;
			break;
		case 13:
			*cursorLoc = 5;
			break;
		default:
			break;
	}
}

void moveLeft(int *cursorLoc)
{
	switch (*cursorLoc)
	{
		case 0:
			*cursorLoc = 7;
			break;
		case 8:
			*cursorLoc = 13;
			break;
		default:
			*cursorLoc -= 1;
			break;

	}
}


void moveRight(int *cursorLoc)
{
	switch (*cursorLoc)
	{
		case 7:
			*cursorLoc = 0;
			break;
		case 13:
			*cursorLoc = 8;
			break;
		default:
			*cursorLoc += 1;
			break;
	}
}

void moveDown(int *cursorLoc)
{
	switch (*cursorLoc)
	{
		case 0:
			*cursorLoc = 8;
			break;
		case 1:
			*cursorLoc = 9;
			break;
		case 2:
			*cursorLoc = 10;
			break;
		case 3:
			*cursorLoc = 11;
			break;
		case 4:
			*cursorLoc = 12;
			break;
		case 5:
			*cursorLoc = 13;
			break;
		case 6:
			*cursorLoc = 13;
			break;
		case 7:
			*cursorLoc = 13;
			break;
		case 8:
			*cursorLoc = 0;
			break;
		case 9:
			*cursorLoc = 1;
			break;
		case 10:
			*cursorLoc = 2;
			break;
		case 11:
			*cursorLoc = 3;
			break;
		case 12:
			*cursorLoc = 4;
			break;
		case 13:
			*cursorLoc = 5;
			break;
		default:
			break;
	}
}

/*
	The LCD is positioned like this:
	| MM / DD / YYYY |
	| HH : MM : SS |
	
	Row 1 starts at position 0 for cursor or (0,0) for LCD and ends at position 7 for the cursor or (0, 9) for LCD
	
	Row 2 starts at position 8 for cursor or (1,0) for LCD and ends at position 13 for the cursor or (1, 5) for LCD

	The /'s and :'s are ignored when determining the LCD position
*/
void change_LCD_Pos(int cursorLoc)
{
	switch(cursorLoc)
	{
		case 0:
			lcd_pos(0,0); // upper digit of month
			break;
		case 1:
			lcd_pos(0,1); // lower digit of month
			break;
		case 2:
			lcd_pos(0,3); // upper digit of day
			break;
		case 3:
			lcd_pos(0,4); // lower digit of day
			break;
		case 4:
			lcd_pos(0,6); // millennium
			break;
		case 5:
			lcd_pos(0,7); // century
			break;
		case 6:
			lcd_pos(0,8); // decade
			break;
		case 7:
			lcd_pos(0,9); // year
			break;
		case 8:
			lcd_pos(1,0); // upper hour digit
			break;
		case 9:
			lcd_pos(1,1); // lower hour digit
			break;
		case 10:
			lcd_pos(1,2); // upper minute digit
			break;
		case 11:
			lcd_pos(1,3); // lower minute digit
			break;
		case 12:
			lcd_pos(1,4); // upper second digit
			break;
		case 13:
			lcd_pos(1,5); // lower second digit
			break;
		default:
			break;
	}
}

// Note that any digit cannot be incremented beyond the display limit 
// (seconds cannot be incremented to 100 and beyond since it is only 2 digits)
void incDigit(struct dt *dt, int cursorLoc)
{
	switch (cursorLoc)
	{
		case 0: // upper digit of month
			if (dt->month + 10 < 100)
				dt->month += 10;
			break;
		case 1: // lower digit of month
			if (dt->month + 1 < 100)
				dt->month += 1;
			break;
		case 2: // upper digit of days
			if (dt->day + 10 < 100)
				dt->day += 10;
			break;
		case 3: // lower digit of days
			if (dt->day + 1 < 100)
				dt->day += 1;
			break;
		case 4: // millennium
			if (dt->year + 1000 < 10000)
				dt->year += 1000;
			break;
		case 5: // century
			if (dt->year + 100 < 10000)
				dt->year += 100;
			break;
		case 6: // decade
			if (dt->year + 10 < 10000)
				dt->year += 10;
			break;
		case 7: // year
			if (dt->year + 1 < 10000)
				dt->year += 1;
			break;
		case 8: // upper digit of hours
			if (dt->hour + 10 < 100)
				dt->hour += 10;
			break;
		case 9: // lower digit of hours
			if (dt->hour + 1 < 100)
				dt->hour += 1;
			break;
		case 10: // upper digit of minutes
			if (dt->minute + 10 < 100)
				dt->minute += 10;
			break;
		case 11: // lower digit of minutes
			if (dt->minute + 1 < 100)
				dt->minute += 1;
			break;
		case 12: // upper digit of seconds
			if (dt->second + 10 < 100)
				dt->second += 10;
			break;
		case 13: // lower digit of seconds
			if (dt->second + 1 < 100)
				dt->second += 1;
			break;
		default: // This should never be executed
			break;
	}
}

// Note that values of digits cannot be decremented below 0
void decDigit(struct dt *dt, int cursorLoc)
{
	switch (cursorLoc)
	{
		case 0:  // upper digit of month
			if (dt->month - 10 >= 0)
				dt->month -= 10;
			break;
		case 1: // lower digit of month
			if (dt->month - 1 >= 0)
				dt->month -= 1;
			break;
		case 2: // upper digit of day
			if (dt->day - 10 >= 0)
				dt->day -= 10;
			break;
		case 3: // lower digit of day
			if (dt->day - 1 >= 0)
				dt->day -= 1;
			break;
		case 4: // millennium
			if (dt->year - 1000 >= 0)
				dt->year -= 1000;
			break;
		case 5: // century
			if (dt->year - 100 >= 0)
				dt->year -= 100;
			break;
		case 6: // decade
			if (dt->year - 10 >= 0)
				dt->year -= 10;
			break;
		case 7: // year
			if (dt->year - 1 >= 0)
				dt->year -= 1;
			break;
		case 8: // upper digit of hours
			if (dt->hour - 10 >= 0)
				dt->hour -= 10;
			break;
		case 9: // lower digit of hours
			if (dt->hour - 1 >= 0)
				dt->hour -= 1;
			break;
		case 10: // upper digit of minutes
			if (dt->minute - 10 >= 0)
				dt->minute -= 10;
			break;
		case 11: // lower digit of minutes
			if (dt->minute - 1 >= 0)
				dt->minute -= 1;
			break;
		case 12: // upper digit of seconds
			if (dt->second - 10 >= 0)
				dt->second -= 10;
			break;
		case 13: // lower digit of seconds
			if (dt->second - 1 >= 0)
				dt->second -= 1;
			break;
		default:
			break;
	}
}
/*
		  KeyPad
	  _   _   _   _
	| 1 | 2 | 3 | A |
	  _   _   _   _
	| 4 | 5 | 6 | B |
	  _   _   _   _
	| 7 | 8 | 9 | C |
	  _   _   _   _
	| * | 0 | # | D |
	  _   _   _   _
	  
	When using the keypad, these buttons perform specific functions
	
	A - Sets computer to set clock mode where you can use 
		the keypad buttons of 2, 4, 6, 8

	2 - Move the LCD cursor position up by 1 row. If the position 
		is already at the top row, then it will simply wrap to the bottom row. 
		Does nothing unless in Set mode

	4 - Move the LCD cursor position to the left by 1 spot. If it is already 
		at the leftmost column of the LCD, it will simply wrap to the other side
		Does nothing unless in Set mode

	6 - Move the LCD cursor position to the right by 1 spot. If it is already
		at the rightmost column of the LCD, it will simply wrap to the other side
		Does nothing unless in Set mode

	8 - Move the LCD cursor down by 1 row. If the cursor is already at the bottom row,
		it will wrap to the top row. If the cursor is at the decade and year position, it will
		wrap to the seconds position.
		Does nothing unless in Set mode
	
	C - Increment the digit corresponding to the cursor position on the clock by 1.
		Does nothing unless in Set mode

	D - Decrement the digit corresponding to the cursor position on the clock by 1.
		Does nothing unless in Set mode

	B - Sets the clock to clock mode where it runs out like a regular clock. 
		This only works if the device is in Set mode
		
	Every other button is ignored 
*/
void setTime(struct dt *dt, int keyPress, int *cursorLoc)
{
	switch (keyPress)
	{
		case 2: // move up
			moveUp(cursorLoc);
			change_LCD_Pos(*cursorLoc);
			break;
		case 5: // move left
			moveLeft(cursorLoc);
			change_LCD_Pos(*cursorLoc);
			break;
		case 7: // move right
			moveRight(cursorLoc);
			change_LCD_Pos(*cursorLoc);
			break;
		case 10: // move down
			moveDown(cursorLoc);
			change_LCD_Pos(*cursorLoc);
			break;
		case 12: // increment digit by 1
			incDigit(dt, *cursorLoc);
			print_dt(dt);
			break;
		case 16: // decrement digit by 1
			decDigit(dt, *cursorLoc);
			print_dt(dt);
			break;
		default:
			break;
	}
}

void clockRun(struct dt *dt)
{
	static int keyPress;
	static int cursorLoc;

	switch(Clock_State)	// Transition
	{
		case Clock_Init:
			Clock_State = Clock_Run;
			break;
		case Clock_Set:
			if (keyPress == 8) // If B is pressed, switch to run mode
			{
				check_dt(dt);
				Clock_State = Clock_Run;
				cursorLoc = 0;
			}
			else // Remain in same state
				Clock_State = Clock_Set;

			break;
		case Clock_Run:
			if (keyPress == 4) // If A is pressed, switch to to Set mode
			{
				Clock_State = Clock_Set;
				cursorLoc = 0;
			}
			else // Remain in same state
				Clock_State = Clock_Run;
			break;
		default:
			init_dt(dt);
	}

	switch(Clock_State) // State
	{
		case Clock_Init:
			break;
		case Clock_Set:
			avr_wait(5);
			keyPress = kp_get_key();
			setTime(dt, keyPress, &cursorLoc);
			break;
		case Clock_Run:
			advance_dt(dt);
			avr_wait(1000);
			keyPress = kp_get_key();
			print_dt(dt);
			break;
		default:
			break;
	}
}

int main(void)
{
    struct dt dt;
	init_dt(&dt);
	Clock_State = Clock_Init;
	lcd_init();
	kp_init();
	lcd_pos(0,0);
    while (1) 
    {
		clockRun(&dt);
    }
	return 0;
}
