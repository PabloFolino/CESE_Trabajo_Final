/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2021/08/24
 * Archivo: bumper.h
 * Version: 1
 *===========================================================================*/

#ifndef _BUMPERS_H_
#define _BUMPERS_H_

/*=====[Inclusions of public function dependencies]==========================*/
#include "sapi.h"

/*=====[Definition macros of public constants]===============================*/
#define PERIODE_TEST_MSEG	250 //Cargo el per�odo 100Mseg
#define STOP_FRONT GPIO7		// Bumpers frontales
#define STOP_BACK  GPIO8		// Bumpers traseros
/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/


/*=====[Prototypes (declarations) of public functions]=======================*/
void bumpers_init(void);
void task_bumper_front( void* taskParmPtr );
void task_bumper_back( void* taskParmPtr );

#endif /* End of File H */
