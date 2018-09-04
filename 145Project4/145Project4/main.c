/*
 * 145Project4.c
 *
 * Created: 5/21/2018 12:10:12 PM
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "avr.h"
#include "lcd.h"

#define WAIT_RESOLUTION 100000lu
char shouldSample = 0;

struct stats{
	unsigned short min;
	unsigned short max;
	unsigned short curr;
	unsigned short avg;
	unsigned short count;
} myStats;

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

unsigned short get_a2c(){

	SET_BIT(ADCSRA,7);
	SET_BIT(ADCSRA,6);
	while(GET_BIT(ADCSRA,6)){
	}
	return ADC;
}

ISR(TIMER1_COMPA_vect)
{
	if(shouldSample)
	{
		unsigned short result = get_a2c();

		if(result < myStats.min)
		myStats.min = result;

		if(result > myStats.max)
		myStats.max = result;
		myStats.curr = result;

		myStats.avg += result;
		myStats.count += 1;
	}
}

int main(void)
{
    /* Replace with your application code */
	SET_BIT(ADMUX, 6);
	
	//Enable timer
	TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
	TIMSK |= (1 << OCIE1A); // Enable CTC interrupt specific to one timer
	
	//cli(); //  Disable global interrupts
	sei(); //  Enable global interrupts
	
	OCR1A   = 31249; // Set CTC compare value
	TCCR1B |= (1 << CS12); // Start timer at Fcpu/256
	//so you divide the value you're comparing with by 8 mhz.
	//OCR1A*TCCR1B/8mhz

	//Default values
	myStats.min = 1023;
	myStats.max = 0;
	myStats.avg = 0;
	myStats.count = 1;
	myStats.curr = 0;

	ini_lcd();
	pos_lcd(0,0);
	char buff[20];
	myStats.count = 0;
    while (1) 
    {
		pos_lcd(0,0);
		//clr_lcd();
		
		float start = (myStats.curr * 5) / 1024.0f;
		int whole = start;
		int part = (start - whole) * 100;
		
		float avg = ((myStats.avg / 1024.0f) * 5) / myStats.count;
		int aWhole = avg;
		int aPart = (avg - aWhole) * 100; 
		
		float max = myStats.max * 5 / 1024.0f;
		int fWhole = max;
		int fPart = (max - fWhole) * 100;
		
		float min = myStats.min * 5 / 1024.0f;
		int mWhole = min;
		int mPart = (min - mWhole) * 100;
		if (mWhole == 0)
			mPart = 0;
		
		sprintf(buff, "%i.%i %i.%i", whole, part, aWhole, aPart);
		puts_lcd2(buff);
		
		pos_lcd(1,0);
		
		sprintf(buff, "%i.%i %i.%i", fWhole, fPart, mWhole, mPart);
		puts_lcd2(buff);
		
		int key = get_key();
		
		if(key != 0)
		{
			if(key < 8)
				shouldSample = !shouldSample;
			else
			{
				myStats.min = 1023;
				myStats.max = 0;
				myStats.avg = 0;
				myStats.count = 1;
				myStats.curr = 0;
				clr_lcd();
			}
		}

		wait_avr(1000);
    }
}

 