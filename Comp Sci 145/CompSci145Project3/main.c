/*
 * Project 3.c
 *
 * Created: 2/13/2022 8:38:46 PM
 * Author : willb
 */ 
#include "avr.h"
#include "lcd.h"
#include <avr/io.h>
#include <stdio.h>

// Only doing octaves 4 and 5
// Octave 4
#define A4 440
#define B4 493
#define C4 261
#define D4 293
#define E4 329
#define F4 349
#define G4 392

// Octave 5
#define A5 880
#define B5 988
#define C5 523
#define D5 587
#define E5 659
#define F5 698
#define G5 784

// Note durations
#define W 1024 // 1000 Whole Note
#define H 512 // 500 Half Note
#define Q 256 // 250 Quarter Note
#define E 128 // 125 Eighth Note
enum Music_Player_States{ Player_Init, Player_Pause, Player_Play} Player_State;

struct note {
	int freq;
	int duration;
};


struct note SongOfTime[] =
{
	{A4, H},
	{D4, W},
	{F4, H},
	{A4, H},
	{D4, W},
	{F4, H},
	{A4, Q},
	{C5, Q},
	{B4, H},
	{G4, H},
	{F4, Q},
	{G4, Q},
	{A4, H},
	{D4, H},
	{C4, Q},
	{E4, Q},
	{D4, W},
	{D4, W},
	{A4, H},
	{D4, W},
	{F4, H},
	{A4, H},
	{D4, W},
	{F4, H},
	{A4, Q},
	{C5, Q},
	{B4, H},
	{G4, H},
	{F4, Q},
	{G4, Q},
	{A4, H},
	{D4, H},
	{C4, Q},
	{E4, Q},
	{D4, W},
	{D4, W},
	{D4, Q}, // NEW PART
	{C4, Q},
	{E4, H},
	{C4, H},
	{E4, Q},
	{F4, Q},
	{D4, W}, // PAUSE
	{D4, W},
	{D4, H},
	{D4, Q},
	{C4, Q},
	{E4, H},
	{C4, H},
	{F4, Q},
	{G4, Q},
	{D4, W},
	{D4, W},
	{D4, H},
	{A4, Q},
	{C5, Q},
	{B4, H},
	{C5, H},
	{A4, H},
	{C5, H},
	{G4, H},
	{A4, H},
	{D4, Q},
	{C4, Q},
	{E4, H},
	{D4, W},
	{D4, W},
	{F4, Q},
	{G4, Q},
	{F4, H},
	{G4, H},
	{E4, Q},
	{F4, H},
	{E4, H},
	{D4, W},
	{D4, W}
}; 

void playNote(const struct note n)
{
	int i, k;
	float P = 1 / (float)n.freq;
	int Period = P * 10000;
	int TH = Period / 2;
	int TL = TH;
	k = (n.duration / P) / 1000;

	for (i = 0; i < k; ++i)
	{
		// I'm using PORTA since it has not been used yet.
		SET_BIT(PORTA, 0);
		avr_wait(TH);
		CLR_BIT(PORTA, 0);
		avr_wait(TL);
	}
}

// Debugging purposes. Ignore this
/*
void playSong(const struct note *song, int n)
{
	int i;
	for (i = 0; i < n; ++i)
	{
		playNote(song[i]);
	}
	
}

// Debugging purposes. Ignore this
void test_speaker()
{
	SET_BIT(PORTA, 0);
	avr_wait(1);
	CLR_BIT(PORTA, 0);
	avr_wait(1);
}
*/

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

void printLCD(int mode)
{
	static int newMode = 0;
	if (mode != newMode)
		lcd_clr();
	char buf[17];
	lcd_pos(0,0);
	if (mode == 0)
	{
		sprintf(buf, "Song Paused");
	}
	else
	{
		sprintf(buf, "Song of Time");
	}
	lcd_puts2(buf);
	newMode = mode;
}

void musicPlayer()
{
	static int keyPress;
	static int i = 0;

	int notes = 76; // notes for song of time

	keyPress = kp_get_key();
	switch (Player_State) // Transition
	{
		case Player_Init:
			Player_State = Player_Play;
			break;
		case Player_Pause:
			if (keyPress == 8) // 8 on keypad is play
				Player_State = Player_Play;
			else
				Player_State = Player_Pause;
			break;
		case Player_Play:
			if (keyPress == 4) // 4 on keypad is pause
				Player_State = Player_Pause;
			else
				Player_State = Player_Play;
			break;
		default:
			break;
	}

	switch (Player_State) // State
	{
		case Player_Init:
			break;
		case Player_Pause:
			printLCD(0); 
			break;
		case Player_Play:
			printLCD(1);
			playNote(SongOfTime[i]);
			i++;
			if (i >= notes) // reached end of song
				i = 0;
			break;
		default:
			break;
	}
}

int main(void)
{
    SET_BIT(DDRB,3);
	kp_init();
	lcd_init();
	lcd_pos(0,0);
	SET_BIT(DDRA, 0);

    while (1) 
    {
		musicPlayer();
		//test_speaker(); // For debugging
    }
}

