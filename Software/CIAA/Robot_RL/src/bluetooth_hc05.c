/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2020/08/17
 * Archivo: bluetooth_hc05.c
 * Version: 1
 *===========================================================================*/
/*Descripci�n:
 *===========================================================================*/
/*==================[inlcusiones]============================================*/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "sapi.h"        // <= Biblioteca sAPI
#include <string.h>
#include "keys.h"
#include "led.h"
#include "lcd.h"
#include "motor.h"
#include "bluetooth_hc05.h"

/*==================[definiciones y macros]==================================*/

#define UART_BLUETOOTH UART_232

#define BAUDRATE 9600

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/
extern estadoMotores_t motores;
extern t_led_data led_data[];
extern t_lcd_data lcd_data;
/*==================[declaraciones de funciones internas]====================*/
void task_bluetooth(  void* taskParmPtr );
/*==================[declaraciones de funciones externas]====================*/

bool_t hc05Test( int32_t uart );
void hc05PrintATCommands( int32_t uart );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
void init_bluetoothHC05( void )
{
   // ---------- CONFIGURACIONES ------------------------------
   // Inicializar UART_232 para conectar al modulo bluetooth
	BaseType_t res;

	uartConfig( UART_BLUETOOTH, BAUDRATE );
	res = xTaskCreate (
			  task_bluetooth,						// Funcion de la tarea a ejecutar
			  ( const char * )"task_bluetooth",	// Nombre de la tarea como String amigable para el usuario
			  configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
			  0,							// Parametros de tarea
			  tskIDLE_PRIORITY+1,			// Prioridad de la tarea
			  0								// Puntero a la tarea creada en el sistema
		  );

	// Gestion de errores
	configASSERT(res == pdPASS);

}


bool_t hc05Test( int32_t uart )
{
   uartWriteString( uart, "AT\r\n" );
   return waitForReceiveStringOrTimeoutBlocking( uart, 
                                                 "OK\r\n", strlen("OK\r\n"),
                                                 1000 );
}

void hc05PrintATCommands( int32_t uart )
{
   uartWriteString( uart, "AT+HELP?\r\n" );
   delay(500);
   uartWriteString( uart, "AT+VERSION?\r\n" );
}

void task_bluetooth( void* taskParmPtr )
{
	TickType_t xPeriodicity = pdMS_TO_TICKS( BLUETOOTH_MS ); // Tarea periodica
	 uint8_t data = 0;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	while( 1 )
	{
    if( uartReadByte( UART_BLUETOOTH, &data ) ) {
       if( data == 'h' ) {
    	  gpioToggle(LEDB);
          //gpioWrite( LEDB, ON );
    	  bluetooth_Update( 0 );
       }
       if( data == 'l' ) {
    	  gpioToggle(LEDB);
          //gpioWrite( LEDB, OFF );
    	  bluetooth_Update( 1 );
       }
       if( data == 'i' ) {
    	  gpioToggle(LEDB);
          //gpioWrite( LEDB, ON );
    	  bluetooth_Update( 2 );
       }
       if( data == 'm' ) {
    	  gpioToggle(LEDB);
          //gpioWrite( LEDB, OFF );
    	  bluetooth_Update( 3);
       }
    }
	// Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
	vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

void bluetooth_Update( uint32_t index ){
	// obtengo el puntero al dato del LCD
	t_lcd_data *ptr_lcd_data;
	ptr_lcd_data=&lcd_data;

	taskENTER_CRITICAL();     // disable ISR
	// Actualizo cantidad de comandos recibidos
	ptr_lcd_data->cuenta++;
	if(ptr_lcd_data->cuenta>255)ptr_lcd_data->cuenta=0;

	switch (index){
	case 0:
		motores.mot1_veloc=motores.mot1_veloc+DELTA;
		if(motores.mot1_veloc>MAX_VELOCIDAD) motores.mot1_veloc=MAX_VELOCIDAD;
		// Visualizo el comando en el LCD
		ptr_lcd_data->comamdo[0]='I';
		ptr_lcd_data->comamdo[1]='c';
		ptr_lcd_data->comamdo[2]='A';
		break;
	case 1:
		motores.mot1_veloc=motores.mot1_veloc-DELTA;
		if(motores.mot1_veloc<-MAX_VELOCIDAD) motores.mot1_veloc=-MAX_VELOCIDAD;
		// Visualizo el comando en el LCD
		ptr_lcd_data->comamdo[0]='D';
		ptr_lcd_data->comamdo[1]='c';
		ptr_lcd_data->comamdo[2]='A';
		break;
	case 2:
		motores.mot2_veloc=motores.mot2_veloc+DELTA;
		if(motores.mot2_veloc>MAX_VELOCIDAD) motores.mot2_veloc=MAX_VELOCIDAD;
		// Visualizo el comando en el LCD
		ptr_lcd_data->comamdo[0]='I';
		ptr_lcd_data->comamdo[1]='c';
		ptr_lcd_data->comamdo[2]='B';
		break;
	case 3:
		motores.mot2_veloc=motores.mot2_veloc-DELTA;
		if(motores.mot2_veloc<-MAX_VELOCIDAD) motores.mot2_veloc=-MAX_VELOCIDAD;
		// Visualizo el comando en el LCD
		ptr_lcd_data->comamdo[0]='D';
		ptr_lcd_data->comamdo[1]='c';
		ptr_lcd_data->comamdo[2]='B';
		break;
	}
	motores.mot1_ena=Motor_ON;
	motores.mot2_ena=Motor_ON;

	//Modifico condiciones de los LEDs
	led_data[0].periodo	=PERIODE_LED2_MSEG;
	led_data[0].t_on 	=(motores.mot1_veloc*4);
	led_data[1].periodo	=PERIODE_LED2_MSEG;
	led_data[1].t_on 	=(motores.mot2_veloc*4);
	taskEXIT_CRITICAL();				// enable ISR

}

/*==================[fin del archivo]========================================*/
