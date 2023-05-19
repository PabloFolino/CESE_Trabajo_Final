/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2020/08/24
 * Archivo: led.h
 * Version: 1
 *===========================================================================*/

#ifndef _LED_H_
#define _LED_H_

/*=====[Inclusions of public function dependencies]==========================*/
#include "sapi.h"

/*=====[Definition macros of public constants]===============================*/
#define PERIODE_LED2_MSEG	2000	//Cargo el período 2seg--> originalmente es de 1seg
#define DUTY_CYCLE_LED1		0.5

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/
typedef struct {
	TickType_t periodo;
	TickType_t t_on;
} t_led_data;


/*=====[Prototypes (declarations) of public functions]=======================*/
void led_init(void);
void task_led1( void* taskParmPtr );
void task_led2( void* taskParmPtr );

#endif /* End of File H */
