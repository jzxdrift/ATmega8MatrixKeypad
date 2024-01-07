#include "lcd.h"
#include "timer.h"
#include "keyboard.h"

int main(void)
{
	initLCD();
	initTimer();
	initKeyboard();
	
	startTimer();
	
	while(1)
	{
		
	}
}