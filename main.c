/*-----------------------------------------------------------------------------/
 /	IFOBS - main.c															   /
 /-----------------------------------------------------------------------------/
 /	Bowie Gian
 /	Created: 2023-06-30
 /	Modified: 2023-06-30
 /
 /	This file contains the main function for the IFOBS.
 /----------------------------------------------------------------------------*/

/*--------------------------------------------------------------*/
/* Include Files												*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "pico/stdio.h"
#include "pico/time.h"
#include "accelerometer.h"
#include "lcd.h"

/*--------------------------------------------------------------*/
/* Main Function												*/
/*--------------------------------------------------------------*/
int main()
{
	// Initialize serial port
	// stdio_init_all();

	// Time to start monitoring serial port
	// sleep_ms(10000);
	// printf("Serial Monitor On\r\n");
	
	LcdScreen_setup(true);
	LcdScreen_hideCursor();

	char currentString[LCD_COL + 1];

	Accel_setup();

	while (true) {
		Angle angles;

		Accel_poll();
		angles = Accel_getAngle();

		LcdScreen_moveCursor(1, 0);
		snprintf(currentString, LCD_COL + 1, "     Elev:%4d      ", (int)(-angles.alpha));
		currentString[14] = 0xDF;
		LcdScreen_writeString(currentString);

		LcdScreen_moveCursor(2, 0);
		snprintf(currentString, LCD_COL + 1, "     Cant:%4d      ", (int)angles.theta);
		currentString[14] = 0xDF;
		LcdScreen_writeString(currentString);

		sleep_ms(200);
	}
}
