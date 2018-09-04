/*
 * 145Program1.c
 *
 * Created: 4/17/2018 12:25:25 PM
 * Authors : Erick Navarro 51947162(esnavarr),  Edward Chen 88277651(edwardc6)
 */ 

#include <avr/io.h>
#include "avr.h"

int main(void){
	SET_BIT(DDRB,0); //sets output
	CLR_BIT(DDRB,1); //sets input
	//unsigned short waitTime = 500;//adjusted to be 500ms
	while(1)
	{
		if(GET_BIT(PINB,1)){//if the button is not pressed
			CLR_BIT(PORTB,0);//turn the led off
		}
		else
		{
			SET_BIT(PORTB,0);//turn led on
			wait_avr(500);//wait
			CLR_BIT(PORTB,0);//turn led off
			wait_avr(500);//wait
		}
	}
}
