 /*
 * 145Program2.c
 *
 * Created: 4/20/2018 12:25:25 PM
 * Authors : Erick Navarro 51947162(esnavarr),  Edward Chen 88277651(edwardc6)
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "avr.h"
#include "lcd.h"

int clockValues[14] = {0}; //Global clock values (left global for easy access)

// A month has d1d2 days
int daysInAMonthd1[12] = {3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}; //first digit of day
int daysInAMonthd2[12] = {1, 8, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1}; //second digit of day

char shouldIncrement = 1; //controls second

char a_or_p = 'A'; //refers to am or pm
char displayMilitary = 1;//military time no am or pm -> 0:00 -24:59

/*

We assume our clockValues are arranged as follows:

[M1, M2, D1, D2, Y1, Y2, Y3, Y4, H1, H2, M3, M4, S1, S2]

and displayed on the LCD as:

MM/DD/YYYY
HH:MM:SS

*/

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

void values_ini()
{ //for clearing
	clockValues[1] = 1; //second digit of month
	clockValues[3] = 1;//second digit of day 
}

void reset_date() //Reset the date to 0s
{//everything goes to 0
	int i = 0;
	while(i < 8)
	{
		clockValues[i] = 0;
		i++;
	}
	
	return;
}

void reset_time() //Reset the time to 0s
{
	int i = 8;
	while(i < 14)
	{
		clockValues[i] = 0;
		i++;
	}
	
	return;
}

void display_date_and_time(char* buffer)
{
	//clr_lcd();
	pos_lcd(0,0);
	sprintf(buffer, "%i%i/%i%i/%i%i%i%i", clockValues[0], clockValues[1], clockValues[2], clockValues[3], clockValues[4], clockValues[5], clockValues[6], clockValues[7]); //Display the current state of the date
	puts_lcd2(buffer);

	pos_lcd(1,0); //second row
	
	if(displayMilitary) //checks if military
		sprintf(buffer, "%i%i:%i%i:%i%i", clockValues[8], clockValues[9], clockValues[10], clockValues[11], clockValues[12], clockValues[13]); //Display the current state of the time
	else
	{	
		int myTime = clockValues[8] * 10 + clockValues[9]; //hours 
		
		if(myTime == 0)
		{
			sprintf(buffer, "12:%i%i:%i%i AM", clockValues[10], clockValues[11], clockValues[12], clockValues[13]);
		}
		else
		{
			if(myTime >= 12)
				a_or_p = 'P';
			else
				a_or_p = 'A';
			
			if(myTime == 12)
			{
				sprintf(buffer, "12:%i%i:%i%i PM", clockValues[10], clockValues[11], clockValues[12], clockValues[13]);
			}
			else
				sprintf(buffer, "%02i:%i%i:%i%i %cM", myTime%12, clockValues[10], clockValues[11], clockValues[12], clockValues[13], a_or_p); //Display the current state of the time
		}
	}
	puts_lcd2(buffer);
}

void input_time(char *buffer){
	int i = 0;
	int x = 0;
	
	int cursorFix = 0;
	
	reset_time();
	
	while(i < 6){
		clr_lcd();
		pos_lcd(1,0);
		sprintf(buffer, "%i%i:%i%i:%i%i", clockValues[8], clockValues[9], clockValues[10], clockValues[11], clockValues[12], clockValues[13]); //Display the current state of the time
		puts_lcd2(buffer);
		
		pos_lcd(0,i + cursorFix);
		sprintf(buffer, "|");
		puts_lcd2(buffer);
		
		wait_avr(300);
		x = get_key();
		
		if(x == 0 || x%4 == 0 || x == 13 || x == 15){ //Ignore MOST button pushes, letter buttons, *, and #
			if(x == 12){ //C -> start inputting the date again
				i = 0;
				cursorFix = 0;
				reset_time();
				clr_lcd();
			}
			continue;
		}
		
		if(x == 14){
			x = 0;
		}
		
		if( x > 4){
			x = x-(x/4);
		}
		
		clockValues[i+8] = x;
		i = i+1;
		
		if(i != 0 && i <= 4 && i%2 == 0) //moves cursor again when touching :
			++cursorFix;
	}
	
	clr_lcd();
	pos_lcd(1,0);
	sprintf(buffer, "%i%i:%i%i:%i%i", clockValues[8], clockValues[9], clockValues[10], clockValues[11], clockValues[12], clockValues[13]); //Display the current state of the time
	puts_lcd2(buffer);
	
	if(!displayMilitary)
	{
		sprintf(buffer, " A(*)P(#)");
		puts_lcd2(buffer);
		
		while(1)
		{
			wait_avr(300);
			x = get_key();
			
			if(x != 13 && x != 15) //must select am or pm * or #
				continue;
			
			int inputedValue = clockValues[8] * 10 + clockValues[9];
			
			if(x == 13)
			{
				a_or_p = 'A';
				if(inputedValue == 12)
				{
					clockValues[8] = 0;
					clockValues[9] = 0;
				}
				break;
			}
			
			a_or_p = 'P';
			if(!(inputedValue == 12))
			{
				int newValue = 12 + inputedValue;
				clockValues[8] = newValue / 10;
				clockValues[9] = newValue % 10;
			}
			break;
		}
		
		clr_lcd();
		pos_lcd(1,0);
		sprintf(buffer, "%i%i:%i%i:%i%i", clockValues[8], clockValues[9], clockValues[10], clockValues[11], clockValues[12], clockValues[13]); //Display the current state of the time
		puts_lcd2(buffer);
	}
}

void input_date(char *buffer){
		int i = 0;
		int x = 0;
		
		int cursorFix = 0;
		
		reset_date();
		
		while(i < 8){
			clr_lcd();
			pos_lcd(0,0);
			sprintf(buffer, "%i%i/%i%i/%i%i%i%i", clockValues[0], clockValues[1], clockValues[2], clockValues[3], clockValues[4], clockValues[5], clockValues[6], clockValues[7]); //Display the current state of the date
			puts_lcd2(buffer);
			
			pos_lcd(1,i + cursorFix);
			sprintf(buffer, "^");
			puts_lcd2(buffer);
			
			wait_avr(500);
			x = get_key();
			
			if(x == 0 || x%4 == 0 || x == 13 || x == 15){ //Ignore MOST button pushes, letter buttons, *, and #
				if(x == 12){ //C -> start inputting the date again
					i = 0;
					cursorFix = 0;
					reset_date();
					clr_lcd();
				}
				continue;
			}
			
			if(x == 14){ 
				x = 0;
			}
			
			if( x > 4){
				x = x-(x/4);
			}
			
			clockValues[i] = x;
			i = i+1;
			
			if(i != 0 && i <= 4 && i%2 == 0)
				++cursorFix;
		}
		
		clr_lcd();
		pos_lcd(0,0);
		sprintf(buffer, "%i%i/%i%i/%i%i%i%i", clockValues[0], clockValues[1], clockValues[2], clockValues[3], clockValues[4], clockValues[5], clockValues[6], clockValues[7]); //Display the current state of the date
		puts_lcd2(buffer); //Final Date Display
}

char greaterThan(int index, int valueToCompare)
{
	return clockValues[index] > valueToCompare;
}

int isLeapYear(int month)
{
	if (month != 1)
	return 0;

	int currentYear = clockValues[6] * 10 + clockValues[7];
	if (currentYear%4 != 0)
	return 0;

	return 1;
}

void check_date_and_time()
{
	if(!greaterThan(13, 9)) //Check S2
		return;
		
	clockValues[13] = 0;
	clockValues[12] += 1;
	
	if(!greaterThan(12, 5)) //Check S1
		return;
		
	clockValues[12] = 0;
	clockValues[11] += 1;
	
	if(!greaterThan(11, 9)) //Check M4
		return;
		
	clockValues[11] = 0;
	clockValues[10] += 1;
	
	if(!greaterThan(10, 5)) //Check M3
		return;
		
	clockValues[10] = 0;
	clockValues[9] += 1;
	
	if(greaterThan(8, 1) && greaterThan(9, 3))// hours > 24
	{
		clockValues[3] += 1;
		clockValues[8] = 0;
		clockValues[9] = 0;
	}
	else if(!greaterThan(8, 1) && greaterThan(9, 9))
	{
		clockValues[8] += 1;
		clockValues[9] = 0;
	}
	
	int currentMonth = clockValues[1] + clockValues[0]*10 - 1;
	
	if(greaterThan(2, daysInAMonthd1[currentMonth] - 1) && greaterThan(3, daysInAMonthd2[currentMonth] + isLeapYear(currentMonth)))
	{
	//new month this is where we do leap year
		clockValues[1] += 1;
		clockValues[2] = 0;
		clockValues[3] = 1;
	}
	else if (!greaterThan(2, daysInAMonthd1[currentMonth] - 1) && greaterThan(3, 9))
	{
	//19-20
		clockValues[2] += 1;
		clockValues[3] = 0;
	}
	
	if(greaterThan(0, 0) && greaterThan(1, 2))
	{//incrementing year and month
		clockValues[7] += 1;
		clockValues[0] = 0;
		clockValues[1] = 1;
	}
	else if(!greaterThan(0, 0) && greaterThan(1, 9))
	{//incrementing month
		clockValues[1] = 0;
		clockValues[0] += 1;
	}
	
	if(!greaterThan(7, 9)) //do nothing
		return;
		
	clockValues[7] = 0; //checking the year
	clockValues[6] += 1;
	
	if(!greaterThan(6, 9))
		return;
		
	clockValues[6] = 0;
	clockValues[5] += 1;
	
	if(!greaterThan(5, 9))
		return;
		
	clockValues[5] = 0;
	clockValues[4] += 1;
	
	if(!greaterThan(4, 9)) //9999
		return;
		
	reset_date();
	values_ini();
	
}

void toggle_military_time()
{
	displayMilitary = !displayMilitary;
}

ISR(TIMER1_COMPA_vect) 
{
	if(shouldIncrement)
		clockValues[13] += 1;
}

int main(void)
{
	SET_BIT(DDRB,0); //sets output
	CLR_BIT(DDRB,1); //sets input
	
	TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
	TIMSK |= (1 << OCIE1A); // Enable CTC interrupt specific to one timer
	
	//cli(); //  Disable global interrupts
	sei(); //  Enable global interrupts
	
	OCR1A   = 31249; // Set CTC compare value
	TCCR1B |= (1 << CS12); // Start timer at Fcpu/256 
	//so you divide the value you're comparing with by 8 mhz.
	//OCR1A*TCCR1B/8mhz
	
	ini_lcd();
	char buf[20];	//Sufficient for one line of LCD output w/ extra space (just in case)
	
	reset_date();
	reset_time();
	values_ini();
	
    while (1) 
    {
		pos_lcd(0,0);
		//pos_lcd(1,0) or (0,1);

		int x = get_key();
		
		if(x!= 0){ //If the user pushes a button...
			
			if(x%4 == 0){ //And it is a letter button...
				switch(x){
					case 4: //A -> User sets the date
						cli(); //  Disable global interrupts
						input_date(buf);
						sei(); //  Enable global interrupts
						break;
					case 8: //B -> N/A
						cli(); //  Disable global interrupts
						input_time(buf);
						sei(); //  Enable global interrupts
						break;
					case 12: //C -> Clear the LCD
						reset_time();
						reset_date();
						clr_lcd();
						values_ini();
						break;
					case 16: //D -> Start/Stop the clock
						shouldIncrement = !shouldIncrement;
						break;
					default:
						break;
				}
			}
			
			if(x == 13) //toggle military time *
			{
				toggle_military_time();
				clr_lcd();
			}
		}
		
		check_date_and_time();
		display_date_and_time(buf);
	}
}
	


