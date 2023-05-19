/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2021/09/24
 * Archivo: mpu9250.c
 * Version: 1
 *===========================================================================*/
/*Descripci�n: Este m�dulo inicializa y lee la variable global "led_data[i]"
 * 			y la muestra cambios de los leds seg{un el ejercicio D4.
 *===========================================================================*/

/*==================[inclusions]=============================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "mpu9250.h"

/*==================[macros and definitions]=================================*/
MPU9250_address_t addr = MPU9250_ADDRESS_0; // If MPU9250 AD0 pin is connected to GND

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/
void task_mpu9250( void* taskParmPtr );
/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void mpu9250_init(void)
{
	int8_t status;
	BaseType_t res1;

	// Inicializar la IMU
	//printf("Inicializando IMU MPU9250..." );

	status = freeRTOS_mpu9250Init(addr);

	if( status < 0 ){
		printf("IMU MPU9250--> inicializado NO fue inicializada.\r\n" );
		while(1);
		}
	else{
		printf("IMU MPU9250--> inicializado correctamente.\r\n" );
		}

	// Crear tareas en freeRTOS
	res1 = xTaskCreate (
			  task_mpu9250,					// Funcion de la tarea a ejecutar
			  ( const char * )"task_mpu9250",// Nombre de la tarea como String amigable para el usuario
			  configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
			  0,							// Parametros de tarea
			  tskIDLE_PRIORITY+1,			// Prioridad de la tarea
			  0								// Puntero a la tarea creada en el sistema
		  );

	// Gestion de errores
	configASSERT( (res1 == pdPASS));

   }


void task_mpu9250( void* taskParmPtr )
{
	TickType_t xPeriodicity = pdMS_TO_TICKS( MPU9250_MS ); // Tarea periodica
	TickType_t xLastWakeTime = xTaskGetTickCount();

	while( 1 )
	{
		taskENTER_CRITICAL();				// disable ISR
		//Leer el sensor y guardar en estructura de control
		mpu9250Read();

		//    // Imprimir resultados
//		printf( "Giroscopo:      (%f, %f, %f)   [rad/s]\r\n",
//		           mpu9250GetGyroX_rads(),
//		           mpu9250GetGyroY_rads(),
//		           mpu9250GetGyroZ_rads()
//		         );
		//
		//		printf( "Acelerometro:   (%f, %f, %f)   [m/s2]\r\n",
		//            mpu9250GetAccelX_mss(),
		//            mpu9250GetAccelY_mss(),
		//            mpu9250GetAccelZ_mss()
		//          );
		//
		//		printf( "Magnetometro:   (%f, %f, %f)   [uT]\r\n",
		//            mpu9250GetMagX_uT(),
		//            mpu9250GetMagY_uT(),
		//            mpu9250GetMagZ_uT()
		//          );
		//
		//		printf( "Temperatura:    %f   [C]\r\n\r\n",
		//            mpu9250GetTemperature_C()
		//          );
		//


		taskEXIT_CRITICAL();				// enable ISR

		// Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

