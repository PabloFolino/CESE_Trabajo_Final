/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2020/08/24
 * Archivo: led.h
 * Version: 1
 *===========================================================================*/

#ifndef _LCD_H_
#define _LCD_H_

/*=====[Inclusions of public function dependencies]==========================*/
#include "sapi.h"

/*=====[Definition macros of public constants]===============================*/
#define CLOCK_RATE_I2C	100000		// Clock de I2C
#define LCD_STARTUP_WAIT_MS	1000	// Tiempo de arranque del LCD
#define DISPLAY_MS	1000			// Tiempo que actualiza el display

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/
typedef struct {
	char tOkString[6];
	char countsOk[5];
	uint16_t cuenta;
	char cmdString[6];
	char comamdo[5];
	char motor0String[4];
	char speedM0[5];
	char motor1String[4];
	char speedM1[5];
} t_lcd_data;
/*=====[Prototypes (declarations) of public functions]=======================*/
void lcd_init( t_lcd_data * display );

#endif /* End of File H */
