/*-----------------------------------------------------------------------------/
 /	IFOBS - lcd.c															   /
 /-----------------------------------------------------------------------------/
 /	Created by: Bowie Gian
 /	Created: 2022-12-07
 /	Modified: 2023-06-30
 /
 /	This file contains the functions to operate a 20x4 LCD screen.
 /	This is modified to run on a Raspberry Pi Pico from a Beaglebone Green.
 /----------------------------------------------------------------------------*/

/*--------------------------------------------------------------*/
/* Include Files												*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "lcd.h"

/*--------------------------------------------------------------*/
/* Definitions													*/
/*--------------------------------------------------------------*/
// Number of GPIO pins used
#define NUM_GPIO_PINS 10

// Wait times during LCD execution
#define LCD_EN_PULSE_DELAY_US 	1
#define LCD_WRITE_DELAY_US		50
#define LCD_RTN_HOME_DELAY_US	2000

// RS (Register Select) values
#define LCD_RS_COMD 0
#define LCD_RS_DATA 1

/*--------------------------------------------------------------*/
/* Enums														*/
/*--------------------------------------------------------------*/
// Index of gpioPins
enum GpioPins {
	LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_D4, LCD_D5, LCD_D6, LCD_D7, LCD_RS, LCD_EN
};

// List of common commands
// Refer to HD44780U documentation to add more
enum LcdScreen_Commands {
	LCD_RTN_HOME	= 0x02, // Resets shift & cursor pos (also used to init in 4-bit mode first)
	LCD_INIT_4BIT	= 0x28, // 2 lines, 5x8 dots, 4-bit mode
	LCD_INIT_8BIT	= 0x38, // 2 lines, 5x8 dots, 8-bit mode
	LCD_DISP_CLEAR	= 0x01, // Clear display, reset cursor
	LCD_DISP_C_ON	= 0b1111, // Bits: Opcode, Display on/off, Cursor on/off, Blinking on/off
	LCD_DISP_C_OFF	= 0b1100,
	LCD_AUTO_R		= 0x06, // Auto increment address right

	LCD_SHIFT_L		= 0x18, // Shifts display left (does not shift data registers)
	LCD_SHIFT_R		= 0x1C, // Shifts display right

	LCD_CSTM_CHAR	= 0x40, // +=8 for next char (0-7)
	LCD_ROW0		= 0x80, // +=1 for next position (0-15)
	LCD_ROW1		= 0xC0, // +=1 for next position (0-15)
	LCD_ROW2		= 0x94, // +=1 for next position (0-15)
	LCD_ROW3		= 0xD4  // +=1 for next position (0-15)
};

/*--------------------------------------------------------------*/
/* Global Variables				 								*/
/*--------------------------------------------------------------*/
static int gpioPins[NUM_GPIO_PINS] = {0, 1, 2, 3, 21, 20, 19, 18, 17, 16};
static bool isNibbleMode = true; // true = 4-bit, false = 8-bit mode

/*--------------------------------------------------------------*/
/*  Function Implemetations										*/
/*--------------------------------------------------------------*/
static void gpioSetup(void)
{
	for (int i = 0; i < NUM_GPIO_PINS; i++) {
		gpio_init(gpioPins[i]);
		gpio_set_dir(gpioPins[i], GPIO_OUT);
		gpio_put(gpioPins[i], 0);
	}
}

static void pulseEnAndSleepUs(long long delayInUs)
{	// Sends enable pulse to write to register (latches on falling edge)
	gpio_put(gpioPins[LCD_EN], 1);
	sleep_us(LCD_EN_PULSE_DELAY_US);

	gpio_put(gpioPins[LCD_EN], 0);
	sleep_us(delayInUs);
}

static void LcdScreen_writeNibble(char* nibble)
{	// Split bits to write to the 4 pins
	for (int i = 0; i < 4; i++) {
		gpio_put(gpioPins[i + 4], (*nibble >> i) & 0x01);
	}
}

// Sends a byte to the LCD screen's rsInt register
static void LcdScreen_writeByte(unsigned char byte, int rsInt)
{
	gpio_put(gpioPins[LCD_RS], rsInt);

	if (isNibbleMode) {
		char nibble = (byte & 0xF0) >> 4;
		LcdScreen_writeNibble(&nibble);
		pulseEnAndSleepUs(LCD_EN_PULSE_DELAY_US);

		nibble = (byte & 0x0F);
		LcdScreen_writeNibble(&nibble);
	} else {
		for (int i = 0; i < 8; i++) {
			gpio_put(gpioPins[i], (byte >> i) & 0x01);
		}
	}

	// 0x02 command needs more time to execute
	if ((byte == LCD_DISP_CLEAR || byte == LCD_RTN_HOME) && rsInt == LCD_RS_COMD) {
		pulseEnAndSleepUs(LCD_RTN_HOME_DELAY_US);
	} else {
		pulseEnAndSleepUs(LCD_WRITE_DELAY_US);
	}
}

static void LcdScreen_command(enum LcdScreen_Commands command)
{
	LcdScreen_writeByte(command, LCD_RS_COMD);
}

void LcdScreen_sendData(unsigned char data)
{
	LcdScreen_writeByte(data, LCD_RS_DATA);
}

void LcdScreen_moveCursor(int row, int col)
{
	if (col < 0 || col >= LCD_COL) {
		printf("Error: LcdScreen_moveCursor col must be in range [0,%d]\n", LCD_COL);
		return;
	}

	if (row == 0) {
		LcdScreen_command(LCD_ROW0 + col);
	} else if (row == 1) {
		LcdScreen_command(LCD_ROW1 + col);
	} else if (row == 2) {
		LcdScreen_command(LCD_ROW2 + col);
	} else if (row == 3) {
		LcdScreen_command(LCD_ROW3 + col);
	} else {
		printf("Error: LcdScreen_moveCursor row must be in range [0,3]\n");
	}
}

void LcdScreen_hideCursor()
{
	LcdScreen_command(LCD_DISP_C_OFF);
}

void LcdScreen_showCursor()
{
	LcdScreen_command(LCD_DISP_C_ON);
}

void LcdScreen_writeString(char* message)
{
	for(int i = 0; i < LCD_COL; i++) {
		LcdScreen_writeByte(*(message + i), LCD_RS_DATA);
	}
}

void LcdScreen_placeChar(int row, int col, unsigned char data)
{
	LcdScreen_moveCursor(row, col);
	LcdScreen_sendData(data);
}

void LcdScreen_loadCstmChar(int location, customChar_t customChar)
{
	if (location < 0 || location >= LCD_CSTM_CHAR_MEM) {
		printf("Error: LcdScreen_loadCstmChar location must be in range [0,%d]\n", LCD_CSTM_CHAR_MEM);
		return;
	}
	LcdScreen_writeByte(LCD_CSTM_CHAR + 8 * location, LCD_RS_COMD);
	for (int i = 0; i < LCD_CHAR_HEIGHT; i++) {
		LcdScreen_writeByte(customChar.bitPattern[i], LCD_RS_DATA);
	}
}

void LcdScreen_setup(bool isNibbleModeIn)
{
	isNibbleMode = isNibbleModeIn;
	gpioSetup();
	
	if (isNibbleMode) {
		LcdScreen_command(LCD_RTN_HOME); // 4-bit mode init
		LcdScreen_command(LCD_INIT_4BIT); // 2 lines, 5x8 dots, 4-bit mode
	} else {
		LcdScreen_command(LCD_INIT_8BIT); // 2 lines, 5x8 dots, 8-bit mode
	}
	LcdScreen_command(LCD_DISP_CLEAR); // Clear display, reset cursor
	LcdScreen_command(LCD_DISP_C_ON); // Display settings
	LcdScreen_command(LCD_AUTO_R); // auto increment address right
}

void LcdScreen_clear(void) 
{
	LcdScreen_command(LCD_DISP_CLEAR);
}
