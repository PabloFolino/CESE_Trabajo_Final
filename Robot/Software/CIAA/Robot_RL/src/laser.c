/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2022/08/17
 * Archivo: laser.c
 * Version: 1
 *===========================================================================*/
/*Descripción: este módulo maneja los dos lunteros laser del Robot
 * Laser derecho se encuentra conectado a GPIO3
 * Laser izquierdo se encuentra conectado a GPIO5
 *===========================================================================*/

/*==================[inclusions]=============================================*/

#include "laser.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
// Función que inicializa la tarea
void laser_init( void ){
	// Seteo como puertos de salida
	gpioConfig(LASER_DER, GPIO_OUTPUT);				//Motor derecho<-----------------------
	gpioConfig(LASER_IZQ, GPIO_OUTPUT);
	// Apago los lasers
	gpioWrite(LASER_DER,OFF);
	gpioWrite(LASER_IZQ,OFF);

	printf("Laser              --> inicializado correctamente.\r\n" );
}


void laser_set(uint8_t *laser, bool_t valor)
{
	if(!strcmp(laser,"LASER_DER")) {
		gpioWrite(LASER_DER,valor);
	   }
	if(!strcmp(laser,"LASER_IZQ")) {
		gpioWrite(LASER_IZQ,valor);
	}
}

