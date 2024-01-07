#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>

static const char KEYS[][4] =
{
	{'1', '4', '7', '*'},
	{'2', '5', '8', '0'},
	{'3', '6', '9', '#'},
	{'A', 'B', 'C', 'D'}
};
static char key[2];

static int8_t getKeyPosition(uint8_t);
static bool isPositionValid(const int8_t, const int8_t);

void initKeyboard(void)
{
	/*inverted bits*/
	DDRB = 0xF0;
	PORTB = 0x0F;
}

const char *const getKeyPressed(void)
{
	const uint8_t MSB_ROW = 7, MSB_COL = 3;
	
	int8_t row = getKeyPosition(MSB_ROW);
	int8_t col = getKeyPosition(MSB_COL);
	
	if(isPositionValid(row, col))
	{
		sprintf(key, "%c", KEYS[row][col]);
		return key;
	}
	
	return NULL;
}

static int8_t getKeyPosition(uint8_t msb)
{
	DDRB = ~DDRB;
	PORTB = ~PORTB;
	_delay_us(10);
	
	for(int8_t i = 3; i >= 0; --i)
	{
		if(bit_is_clear(PINB, msb))
			return i;
		
		--msb;
	}
	
	return -1;
}

static bool isPositionValid(const int8_t rw, const int8_t cl)
{
	if(rw != -1 && cl != -1)
	{
		loop_until_bit_is_set(PINB, cl);
		return true;
	}
	
	return false;
}