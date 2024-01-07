#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

#include "lcd.h"
#include "keyboard.h"

ISR(TIMER0_OVF_vect)
{
	const char *const str = getKeyPressed();
	if(str != NULL)
		displayLCD(str);
}

void initTimer(void)
{
	TCCR0 |= _BV(CS01) | _BV(CS00);		/*64 prescaler*/
	TIMSK |= _BV(TOIE0);
	
	sei();
}

void startTimer(void)
{
	TCNT0 = 0x00;
}
