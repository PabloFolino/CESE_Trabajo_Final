/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2021/09/24
 * Archivo: uart232_esp01.c
 * Version: 1
 *===========================================================================*/
/*Descripci�n: Este m�dulo inicializa y lee la variable global "led_data[i]"
 * 			y la muestra cambios de los leds seg{un el ejercicio D4.
 *
 * Para mandar, tengo que escribir:
 *     				dataToSendToUart232 = uartRxRead(UART_USB);
 * 					dataToSendToUart232Pending = TRUE;
 *===========================================================================*/

/*==================[inclusions]=============================================*/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "sapi.h"
#include "uart232_esp01.h"

/*==================[macros and definitions]=================================*/


/*==================[internal data declaration]==============================*/
uart232_rx_t data_uart232_rx;

/*==================[internal functions declaration]=========================*/
void uart232ReceiveCallback( void *unused );
void uart232SendCallback( void *unused );
/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
void task_uart_232( void* taskParmPtr );
/*==================[external functions definition]==========================*/
void uart_esp01_init(void)
{
	BaseType_t res1;
//	/* Inicializar la UART_232 */
//	uartConfig(UART_232, 115200);
//	// Seteo un callback al evento de recepcion y habilito su interrupcion
//	uartCallbackSet(UART_232, UART_RECEIVE, uart232ReceiveCallback, NULL);
//	// Seteo un callback al evento de transmisor libre y habilito su interrupcion
//	uartCallbackSet(UART_232, UART_TRANSMITER_FREE, uart232SendCallback, NULL);
//	// Habilito todas las interrupciones de UART_232
//	uartInterrupt(UART_232, true);
//
//	// Envío para identificar cual es cual y para arrancar la secuencia (si no
//	// hasta que no se envie un byte desde cada terminal no funciona)
//	// uartWriteByte(UART_232, '2');
//
//	 // inicializo el buffer
//	 // buffer_rx_clean();

	uartConfig(UART_232, 115200);
	//limpio el buffer de rx
	buffer_rx_clean(data_uart232_rx.buffer_rx);
	// Seteo el estado
	data_uart232_rx.status=SIN_DATOS;

	// Crear tareas en freeRTOS
		res1 = xTaskCreate (
				  task_uart_232,					// Funcion de la tarea a ejecutar
				  ( const char * )"task_uart_232",	// Nombre de la tarea como String amigable para el usuario
				  configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
				  0,							// Parametros de tarea
				  tskIDLE_PRIORITY+1,			// Prioridad de la tarea
				  0								// Puntero a la tarea creada en el sistema
			  );
		// Gestion de errores
		configASSERT( (res1 == pdPASS));

		printf("UART232        --> inicializada correctamente.\r\n" );
}


void task_uart_232( void* taskParmPtr )
{
	TickType_t xPeriodicity = pdMS_TO_TICKS(UART232_MS); // Tarea periodica
	TickType_t xLastWakeTime = xTaskGetTickCount();

	while( 1 )
	{
		if(uartRxReady(UART_232)){
			data_uart232_rx.cuenta=0;
			taskENTER_CRITICAL();		    // disable ISR
			while(uartRxReady(UART_232)){
						uartReadByte(UART_232,&data_uart232_rx.buffer_rx[data_uart232_rx.cuenta]);
						data_uart232_rx.cuenta++;
					}
			taskEXIT_CRITICAL();		   // enable ISR
			//gpioToggle(LED3);
			data_uart232_rx.status=DATO_RCV;
		}

		// Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}


















//**********************************************   Por interrupción  NO FUNCA  ****************************************
//// Recibo de la PC en la UART_232
//void uart232ReceiveCallback( void *unused )
//{
//	//Actualizo estado del Robot
//	gpioToggle(LED3);
//	printf("INTERRUPT" );
//	buffer_rx[cuenta] = uartRxRead(UART_232);
//	cuenta++;
//
//}
//
//// Envio a la PC desde la UART_232
//void uart232SendCallback( void *unused )
//{
////   if(dataToSendToUart232Pending){
////      uartTxWrite(UART_232, dataToSendToUart232);
////      dataToSendToUart232 = 0;LONG_BUFFER_RX
////      dataToSendToUart232Pending = FALSE;
////   }
//}

void buffer_rx_clean(uint8_t *buffer1)
{
	uint8_t index;
	if(&buffer1==&data_uart232_rx.buffer_rx) data_uart232_rx.cuenta=0;
	for (index=0;index<LONG_BUFFER_RX;index++){
		buffer1[index]=' ';
	}
}
