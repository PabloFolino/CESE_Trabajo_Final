/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2021/09/24
 * Archivo: bumper.c
 * Version: 1
 *===========================================================================*/
/*Descripci�n: Este m�dulo inicializa y lee la variable global "led_data[i]"
 * 			y la muestra cambios de los leds seg{un el ejercicio D4.
 *===========================================================================*/

/*==================[inclusions]=============================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "fsm_robot.h"
#include "sapi.h"
#include "keys.h"
#include "bumpers.h"
#include "lcd.h"
#include "motor.h"

/*==================[macros and definitions]=================================*/
#define DECREMENTO 100			// Decremento del LED2
/*==================[internal data declaration]==============================*/
//gpioMap_t  led_config[] = { LED1,LED2 } ;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/
extern estadoRobot_t estado;
/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void bumpers_init(void)
{
	BaseType_t res1,res2;

	// Seteo como puertos de entrada sin pull-up
	gpioConfig(STOP_FRONT, GPIO_INPUT);				//Motor derecho<-----------------------
	gpioConfig(STOP_BACK, GPIO_INPUT);


	// Crear tareas en freeRTOS
	res1 = xTaskCreate (
			  task_bumper_front,					// Funcion de la tarea a ejecutar
			  ( const char * )"task_bumper_front",	// Nombre de la tarea como String amigable para el usuario
			  configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
			  0,							// Parametros de tarea
			  tskIDLE_PRIORITY+1,			// Prioridad de la tarea
			  0								// Puntero a la tarea creada en el sistema
		  );
	res2 = xTaskCreate (
			  task_bumper_back,						// Funcion de la tarea a ejecutar
			  ( const char * )"task_bumper_back",	// Nombre de la tarea como String amigable para el usuario
			  configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
			  0,							// Parametros de tarea
			  tskIDLE_PRIORITY+1,			// Prioridad de la tarea
			  0								// Puntero a la tarea creada en el sistema
		  );

	// Gestion de errores
	configASSERT( (res1 == pdPASS)|| (res2 == pdPASS));

	printf("Bumper's        --> inicializado correctamente.\r\n" );
}


void task_bumper_front( void* taskParmPtr )
{
	TickType_t xPeriodicity = pdMS_TO_TICKS(PERIODE_TEST_MSEG); // Tarea periódica
	TickType_t xLastWakeTime = xTaskGetTickCount();

	while( 1 )
	{
		taskENTER_CRITICAL();				// disable ISR
		xPeriodicity = pdMS_TO_TICKS( PERIODE_TEST_MSEG);
		taskEXIT_CRITICAL();				// enable ISR

		/*  Programa  */

		if(!gpioRead( STOP_FRONT )) {
			estado=BUMBERS_FRONT;
			//Paro Motores
			motorEnable(Motor_OFF,MOT_DER);
			motorEnable(Motor_OFF,MOT_IZQ);
			//Retrocedo
			motorSpeed(-VEL_ESCAPE, MOT_DER);
			motorSpeed(-VEL_ESCAPE, MOT_IZQ);
			motorEnable(Motor_ON,MOT_DER);
			motorEnable(Motor_ON,MOT_IZQ);
			// Espero 500 msegs
			vTaskDelay(500/portTICK_PERIOD_MS);
			// Velocidad cero en los motores
			motorSpeed(0, MOT_DER);
			motorSpeed(0, MOT_IZQ);
			motorEnable(Motor_ON,MOT_DER);
			motorEnable(Motor_ON,MOT_IZQ);

		}

		// Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

void task_bumper_back( void* taskParmPtr )
{
	TickType_t xPeriodicity = pdMS_TO_TICKS( PERIODE_TEST_MSEG ); // Tarea periódica
	TickType_t xLastWakeTime = xTaskGetTickCount();

	while( 1 )
	{
		taskENTER_CRITICAL();				// disable ISR
		xPeriodicity = pdMS_TO_TICKS( PERIODE_TEST_MSEG );
		taskEXIT_CRITICAL();				// enable ISR

		/*  Programa  */

		/******Test*****/
		//gpioToggle(LED3);
		/***************/
		if(!gpioRead( STOP_BACK )){
			estado=BUMPERS_BACK;
			//Paro Motores
			motorEnable(Motor_OFF,MOT_DER);
			motorEnable(Motor_OFF,MOT_IZQ);
			//Retrocedo
			motorSpeed(VEL_ESCAPE, MOT_DER);
			motorSpeed(VEL_ESCAPE, MOT_IZQ);
			motorEnable(Motor_ON,MOT_DER);
			motorEnable(Motor_ON,MOT_IZQ);
			// Espero 500 msegs
			vTaskDelay(500/portTICK_PERIOD_MS);
			// Velocidad cero en los motores
			motorSpeed(0, MOT_DER);
			motorSpeed(0, MOT_IZQ);
			motorEnable(Motor_ON,MOT_DER);
			motorEnable(Motor_ON,MOT_IZQ);
		}

		// Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}
