/*-----------------------------------------------------------------------------/
 /	IFOBS - lcd.h															   /
 /-----------------------------------------------------------------------------/
 /	Created by: Bowie Gian
 /	Created: 2022-12-07
 /	Modified: 2023-06-30
 /
 /	This file contains the functions to operate a 20x4 LCD screen.
 /	This is modified to run on a Raspberry Pi Pico from a Beaglebone Green.
 /----------------------------------------------------------------------------*/
#ifndef LCD_SCREEN_H
#define LCD_SCREEN_H

/*--------------------------------------------------------------*/
/* Include Files												*/
/*--------------------------------------------------------------*/
#include <stdbool.h>

/*--------------------------------------------------------------*/
/* Definitions													*/
/*--------------------------------------------------------------*/
#define LCD_ROW 4
#define LCD_COL 20
#define LCD_CHAR_HEIGHT 8
#define LCD_CSTM_CHAR_MEM 8

/*--------------------------------------------------------------*/
/* Structs														*/
/*--------------------------------------------------------------*/
typedef struct {
	unsigned char bitPattern[LCD_CHAR_HEIGHT];
} customChar_t;

/*--------------------------------------------------------------*/
/* Function Prototypes	    									*/
/*--------------------------------------------------------------*/

// Exports gpio pins and configures them to out direction,
// then initializes the LCD screen in (true=4bit, false=8bit) mode
void LcdScreen_setup(bool isNibbleModeIn);

// Clears Screen
void LcdScreen_clear(void);

// Writes the char at the cursor location
void LcdScreen_sendData(unsigned char data);

// Move the cursor starting at 0, 0
void LcdScreen_moveCursor(int row, int col);
void LcdScreen_hideCursor();
void LcdScreen_showCursor();

// Writes a string starting at the cursor location to the whole row.
// The message must be filled with ' ' for blanks until the end of the
// row because 0 is the customChar location, not the '\0'.
// This function currently assumes the cursor is at the start of a row.
void LcdScreen_writeString(char* message);

// Places a char at location (row, col)
void LcdScreen_placeChar(int row, int col, unsigned char data);

// Loads a custom character at mem location 0-7
void LcdScreen_loadCstmChar(int location, customChar_t customChar);

#endif
