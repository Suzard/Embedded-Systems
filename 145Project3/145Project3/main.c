/*
 * 145Project3.c
 *
 * Created: 5/9/2018 12:06:51 PM
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "avr.h"
#include "lcd.h"
#include "Notes.h"

char debug_buf[20];	//Sufficient for one line of LCD output w/ extra space (just in case)

#define DEBUG_LCD 1
#define WAIT_RESOLUTION 100000lu

struct note
{
	int frequency;
	int duration;
};

const struct note song1[] = {{523, 2}, {587, 2}, {698, 2}, {659, 2}, {523, 1}, {0, 2}, {523, 2}, {587, 2}, {698, 2}, {659, 2}, {988, 2}, {784, 1}, {0, 2},
							 {698, 2}, {659, 2}, {698, 2}, {659, 2}, {523, 1}, {0, 2}, {233, 4}, {220,4}, {247, 1}, {0, 2}, {494, 4}, {587,4}, {523, 1}, {0, 2}};
const struct note song2[] = {{D4, 3}, {D4, 8}, {E4, 4}, {D4, 4}, {G4, 4}, {Fs4, 2}, {D4, 3}, {D4, 8}, {E4, 4}, {D4, 4}, {A4, 4}, {G4, 2}, {D4, 3}, {D4, 8}, {D5, 4},
							 {B4, 4}, {G4, 4}, {Fs4, 4}, {E4, 4}, {C5, 3}, {C5, 8}, {B4, 4}, {G4, 4}, {A4, 4}, {G4, 2}, {0, 2}};
const struct note song3[] = {{659, 1}, {784, 2}, {988, 2}, {932, 2}, {622, 1}, {0, 2}, {587, 1}, {739, 3}, {932, 3}, {880, 2}, {554, 1}, {0, 2}, {C5, 4}, {B4, 4}, {C5, 2},
							 {E5, 3}, {C6, 3}, {B5, 4}, {B4, 4}, {Bb4, 4}, {B4, 2}, {E5, 3}, {B5, 3}, {Bb5, 4}}; //{B4, 6}, {Eb5, 6}, {F5, 6}, {E5, 6}, {F5, 6}, {A5, 6},
							 //{F5, 6}, {A5, 6}, {C6, 6}, {A5, 6}, {C6, 6}, {Eb6, 6}, {F6, 6}, {E6, 6}, {C6, 6}, {A5, 6}, {C6, 8}, {A5, 8}, {F5, 8}, {Eb5, 8}};
const struct note song4[] = {{Bb4, 1}, {0, 8}, {0, 8}, {Bb4, 8}, {Bb4, 8}, {Bb4, 8}, {Bb4, 8}, {Bb4, 3}, {Ab4, 8}, {Bb4, 4}, {0, 8}, {0, 8}, {Bb4, 8}, {Bb4, 8}, {Bb4, 8}, 
							{Bb4, 8}, {Bb4, 3}, {Ab4, 8}, {Bb4, 4}, {0, 2}, {Bb4, 8}, {Bb4, 8}, {Bb4, 8}, {Bb4, 8}, {Bb4, 4}, {F4, 8}, {F4, 8}, {F4, 4}, {F4, 8}, {F4, 8}, {F4, 4},
							{F4, 8}, {F4, 8}, {F4, 4}, {F4, 4}, {Bb4, 3}, {0, 8}, {F4, 2}, {Bb4, 8}, {0, 8}, {Bb4, 8}, {C5, 8}, {D5, 8}, {Eb5, 8}, {F5, 2}, {0, 2} };//, {0, 8}, {F5, 8}, {0, 8},
							//{F5, 7}, {Gb5, 7}, {A5, 7}, {B5, 1}, {0, 2}};
const struct note* songs[] = {song1, song2, song3, song4};
const int songLengths[] = {27, 26, 24, 45};//53};

int is_pressed(int r,int c){
	//set everything to n/c except
	//row string 0 and column weak 1
	DDRC = 0x00;
	PORTC = 0x00;
	//set c to weak "1"
	SET_BIT(PORTC, c+4);
	//set r to strong 0 and clear bit just in case even though redundant
	SET_BIT(DDRC, r);
	CLR_BIT(PORTC,r);
	wait_avr(10);
	//check state of button
	if(GET_BIT(PINC,c+4)){
		return 0;
	}
	return 1;
}
int get_key(){
	int r,c;
	for(r=0; r<4;++r){
		for(c=0; c<4;++c){
			if(is_pressed(r,c)){
				return r*4+c+1;
			}
		}
	}
	return 0;

}

void play_note(int frequency, int d)
{
	SET_BIT(DDRA,0);
	
	if(frequency == 0)
	{
		wait_avr(WAIT_RESOLUTION / d);
	}
	
	unsigned long period = (WAIT_RESOLUTION / frequency);
	unsigned long t = period / 2;
	unsigned long duration = WAIT_RESOLUTION / (d * period);
	
	if(DEBUG_LCD)
	{
		clr_lcd();
		sprintf(debug_buf, "%lu", t);
		puts_lcd2(debug_buf);
	}

	for(int i = 0; i < duration; ++i)
	{
		SET_BIT(PORTA,0);
		wait_avr(t);
		CLR_BIT(PORTA,0);
		wait_avr(t);
	}
}

void play_song(int song)
{
	struct note* song_to_play = songs[song];
	int length = songLengths[song];
	
	for(int i = 0; i < length; ++i)
	{
		play_note(song_to_play[i].frequency, song_to_play[i].duration);
		wait_avr(1000);
	}
}

int main(void)
{
	SET_BIT(DDRA,0);
	
	ini_lcd();

	while(1)
	{
		pos_lcd(0, 0);
		sprintf(debug_buf, "Nothing Playing...");
		puts_lcd2(debug_buf);
		pos_lcd(1, 0);
		sprintf(debug_buf, "Select Song 1-4");
		puts_lcd2(debug_buf);
		
		int x = get_key();
		
		if(x != 0)
		{
			pos_lcd(0, 0);
			clr_lcd();
			switch(x)
			{
				case 1: //-> Play Song 0
					sprintf(debug_buf, "Deku Tree - OOT");
					puts_lcd2(debug_buf);
					play_song(0);
					break;
				case 2: //-> Play Song 1
					sprintf(debug_buf, "Birthday Song");
					puts_lcd2(debug_buf);
					play_song(1);
					break;
				case 3: //-> Play Song 2
					sprintf(debug_buf, "LOZ - Dungeon");
					puts_lcd2(debug_buf);
					play_song(2);
					break;
				case 5: //-> Play Song 3
					sprintf(debug_buf, "LOZ - Main Theme");
					puts_lcd2(debug_buf);
					play_song(3);
					break;
				case 6:
					sprintf(debug_buf, "Wait Debug");
					puts_lcd2(debug_buf);
					wait_avr(WAIT_RESOLUTION);
				default:
					//Not valid button press!
					break;
			}
		}
	}
}

