#include "avr.h"

void
ini_avr(void)
{
	WDTCR = 15;
}

void
wait_avr(unsigned long x)
{
	TCCR0 = 2;
	while (x--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 8) * 0.00001);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}