/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2020/08/17
 * Archivo: motor.h
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of public function dependencies]==========================*/
#include "sapi.h"

/*=====[Definition macros of public constants]===============================*/
#define LASER_DER GPIO3				// Laser derecho
#define LASER_IZQ GPIO5				// Laser izquierdo

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[Prototypes (declarations) of public functions]=======================*/
void laser_init( void );
void laser_set(uint8_t *, bool_t valor);










