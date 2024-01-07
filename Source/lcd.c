#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

#define LCD_DATA_DDR		DDRD
#define LCD_CTRL_DDR		DDRC

#define LCD_DATA_PORT		PORTD
#define LCD_CTRL_PORT		PORTC

#define RS			2
#define E			3
#define D4			4
#define D5			5
#define D6			6
#define D7			7

#define LCD_DATA_TX		LCD_CTRL_PORT |= _BV(RS)
#define LCD_INST_TX		LCD_CTRL_PORT &= ~_BV(RS)

#define LCD_EN			LCD_CTRL_PORT |= _BV(E)
#define LCD_DIS			LCD_CTRL_PORT &= ~_BV(E)

#define SET_DDRAM_ADDR		0x80

typedef enum
{
	DATA,
	INSTRUCTION
} LCD_Flags;

static void clearLCD(void);
static void setCursor(const uint8_t, uint8_t);
static void sendDataCommandLCD(const uint8_t, const LCD_Flags *const);
static void switchEnableLCD(void);

void initLCD(void)
{
	const LCD_Flags FLAG = INSTRUCTION;
	
	LCD_DATA_DDR |= _BV(D7) | _BV(D6) | _BV(D5) | _BV(D4);
	LCD_CTRL_DDR |= _BV(E) | _BV(RS);
	
	_delay_ms(20);
	
	/*lower 4 bits only*/
	sendDataCommandLCD(0x03, &FLAG);
	_delay_ms(10);
	
	/*lower 4 bits only*/
	sendDataCommandLCD(0x03, &FLAG);
	_delay_ms(1);
	
	/*lower 4 bits only*/
	sendDataCommandLCD(0x03, &FLAG);
	_delay_ms(1);
	
	/*lower 4 bits only*/
	/*initial function set to change interface*/
	sendDataCommandLCD(0x02, &FLAG);
	_delay_ms(1);
	
	/*function set: 4 bits, 2 lines, 5x8 font*/
	sendDataCommandLCD(0x28, &FLAG);
	_delay_ms(1);
	
	/*display on/off control: display on, cursor on, cursor blink on*/
	sendDataCommandLCD(0x0F, &FLAG);
	_delay_ms(1);
	
	/*clear display*/
	clearLCD();
	
	/*entry mode set: DDRAM address increments, display shift off*/
	sendDataCommandLCD(0x06, &FLAG);
	_delay_ms(1);
}

void displayLCD(const char buff[])
{
	const uint8_t MAX_CHARS = 16;
	const LCD_Flags FLAG = DATA;
	
	static uint8_t cursor = 0;
	
	if(cursor < MAX_CHARS)
		setCursor(1, cursor);
	else
	{
		setCursor(2, cursor - MAX_CHARS);
		
		if(cursor > MAX_CHARS * 2 - 1)
		{
			clearLCD();
			
			cursor = 0;
			setCursor(1, cursor);
		}
	}
	++cursor;
	
	for(uint8_t i = 0; buff[i] != '\0'; ++i)
		sendDataCommandLCD(buff[i], &FLAG);
}

static void clearLCD(void)
{
	const LCD_Flags FLAG = INSTRUCTION;
	
	sendDataCommandLCD(0x01, &FLAG);
	_delay_ms(5);
}

static void setCursor(const uint8_t ln, uint8_t pos)
{
	const LCD_Flags FLAG = INSTRUCTION;
	
	switch(ln)
	{
		case 1:
			pos |= SET_DDRAM_ADDR;
			break;
		case 2:
			/*LCD 2nd line DDRAM starting address 0x40*/
			pos = (pos + 0x40) | SET_DDRAM_ADDR;
			break;
		default:
			return;
	}
	
	sendDataCommandLCD(pos, &FLAG);
}

static void sendDataCommandLCD(const uint8_t dataInst, const LCD_Flags *const flg)
{
	switch(*flg)
	{
		case DATA:
			LCD_DATA_TX;
			break;
		case INSTRUCTION:
			LCD_INST_TX;
			break;
	}
	
	LCD_DATA_PORT = dataInst;
	switchEnableLCD();
	
	LCD_DATA_PORT = dataInst << 4;
	switchEnableLCD();
	
	_delay_ms(1);
}

static void switchEnableLCD(void)
{
	LCD_EN;
	LCD_DIS;
}
