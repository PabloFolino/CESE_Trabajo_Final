/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2020/09/24
 * Archivo: Keys.c
 * Version: 1
 *===========================================================================*/
/*Descripción: Este módulo se encarga de manejar las teclas, posee una rutiana
 * 				de anti-rebote mediante una máquina de estados. inicializa
 * 				la variable "keys_data[]" en el cual se lleva el estado de
 * 				cada tecla,etc.
 *===========================================================================*/

/*==================[ Inclusions ]============================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "keys.h"
#include "led.h"
#include "motor.h"

/*=====[ Definitions of private data types ]===================================*/

/*==================[external data definition]===============================*/
extern uint16_t contador;
extern t_led_data led_data[];
extern estadoMotores_t motores;

/*=====[Definition macros of private constants]==============================*/
//#define BUTTON_RATE     1
#define DEBOUNCE_TIME   40

/*=====[Prototypes (declarations) of private functions]======================*/
static void keys_ButtonError( uint32_t index );
static void buttonPressed( uint32_t index );
//static void buttonReleased( uint32_t index );

/*=====[Definitions of private global variables]=============================*/

/*=====[Definitions of public global variables]==============================*/
const t_key_config  keys_config[] = { TEC1,TEC2 } ;
#define key_count   sizeof(keys_config)/sizeof(keys_config[0])
t_key_data keys_data[key_count];

/*=====[prototype of private functions]=================================*/
void task_tecla1( void* taskParmPtr );
void task_tecla2( void* taskParmPtr );

/*=====[Implementations of public functions]=================================*/
TickType_t get_diff()
{
	TickType_t tiempo;

	taskENTER_CRITICAL();
	tiempo = keys_data[0].time_diff;
	taskEXIT_CRITICAL();

	return tiempo;
}

void clear_diff()
{
	taskENTER_CRITICAL();
	keys_data[0].time_diff = 0;
	taskEXIT_CRITICAL();
}

void keys_init( void )
{
	BaseType_t res1,res2;
	uint8_t i;

	for(i=0; i<key_count;i++){
		keys_data[i].state          = STATE_BUTTON_UP;  	// Set initial state
		keys_data[i].time_down      = KEYS_INVALID_TIME;
		keys_data[i].time_up        = KEYS_INVALID_TIME;
		keys_data[i].time_diff      = KEYS_INVALID_TIME;
	}


	// Crear tareas en freeRTOS
	res1 = xTaskCreate (
			  task_tecla1,					// Funcion de la tarea a ejecutar
			  ( const char * )"task_tecla1",// Nombre de la tarea como String amigable para el usuario
			  configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
			  0,							// Parametros de tarea
			  tskIDLE_PRIORITY+1,			// Prioridad de la tarea
			  0								// Puntero a la tarea creada en el sistema
		  );

	res2 = xTaskCreate (
			  task_tecla2,					// Funcion de la tarea a ejecutar
			  ( const char * )"task_tecla2",// Nombre de la tarea como String amigable para el usuario
			  configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
			  0,							// Parametros de tarea
			  tskIDLE_PRIORITY+1,			// Prioridad de la tarea
			  0								// Puntero a la tarea creada en el sistema
		  );


	// Gestion de errores
	configASSERT( (res1 == pdPASS)|| (res2 == pdPASS));
}

// keys_ Update State Function
void keys_Update( uint32_t index )
{
	switch( keys_data[index].state )
	{
		case STATE_BUTTON_UP:
			/* CHECK TRANSITION CONDITIONS */
			if( !gpioRead( keys_config[index].tecla ) )
			{
				keys_data[index].state = STATE_BUTTON_FALLING;
			}
			break;

		case STATE_BUTTON_FALLING:
			/* ENTRY */

			/* CHECK TRANSITION CONDITIONS */
			if( !gpioRead( keys_config[index].tecla ) )
			{
				keys_data[index].state = STATE_BUTTON_DOWN;
				/* ACCION DEL EVENTO !*/
				buttonPressed( index );
			}
			else
			{
				keys_data[index].state = STATE_BUTTON_UP;
			}

			/* LEAVE */
			break;

		case STATE_BUTTON_DOWN:
			/* CHECK TRANSITION CONDITIONS */
			if( gpioRead( keys_config[index].tecla ) )
			{
				keys_data[index].state = STATE_BUTTON_RISING;
			}
			break;

		case STATE_BUTTON_RISING:
			/* ENTRY */

			/* CHECK TRANSITION CONDITIONS */

			if( gpioRead( keys_config[index].tecla ) )
			{
				keys_data[index].state = STATE_BUTTON_UP;
				/* ACCION DEL EVENTO ! */
				buttonReleased( index );
			}
			else
			{
				keys_data[index].state = STATE_BUTTON_DOWN;
			}

			/* LEAVE */
			break;

		default:
			keys_ButtonError( index );
			break;
	}
}

/*=====[Implementations of private functions]================================*/

/* accion de el evento de tecla pulsada */
//static void buttonPressed( uint32_t index )
void buttonPressed( uint32_t index )
{
	TickType_t current_tick_count = xTaskGetTickCount();

	taskENTER_CRITICAL();
	keys_data[index].time_down = current_tick_count;
	taskEXIT_CRITICAL();
}

/* accion de el evento de tecla liberada */
//static void buttonReleased( uint32_t index )
void buttonReleased( uint32_t index )
{
	taskENTER_CRITICAL();     // disable ISR
	switch (index){
	case 0:
		motores.mot1_veloc=motores.mot1_veloc+DELTA;
		if(motores.mot1_veloc>MAX_VELOCIDAD) motores.mot1_veloc=MAX_VELOCIDAD;
		motores.mot2_veloc=motores.mot2_veloc+DELTA;
		if(motores.mot2_veloc>MAX_VELOCIDAD) motores.mot2_veloc=MAX_VELOCIDAD;
		break;
	case 1:
		motores.mot1_veloc=motores.mot1_veloc-DELTA;
		if(motores.mot1_veloc<-MAX_VELOCIDAD) motores.mot1_veloc=-MAX_VELOCIDAD;
		motores.mot2_veloc=motores.mot2_veloc-DELTA;
		if(motores.mot2_veloc<-MAX_VELOCIDAD) motores.mot2_veloc=-MAX_VELOCIDAD;
		break;
	}
	//Habilito los motores
	motores.mot1_ena=Motor_ON;
	motores.mot2_ena=Motor_ON;

	//Modifico condiciones de los LEDs
	led_data[0].periodo	=PERIODE_LED2_MSEG;
	led_data[0].t_on 	=(motores.mot1_veloc*4);
	led_data[1].periodo	=PERIODE_LED2_MSEG;
	led_data[1].t_on 	=(motores.mot2_veloc*4);
	taskEXIT_CRITICAL();				// enable ISR

}

static void keys_ButtonError( uint32_t index )
{
	taskENTER_CRITICAL();		// disable ISR
	keys_data[index].state = STATE_BUTTON_UP;
	taskEXIT_CRITICAL();		// enable ISR
}

/*=====[Implementations of private functions]=================================*/
void task_tecla1( void* taskParmPtr )
{
	while( 1 )
	{
		keys_Update( 0 );
		vTaskDelay( DEBOUNCE_TIME / portTICK_RATE_MS );
	}
}

void task_tecla2( void* taskParmPtr )
{
	while( 1 )
	{
		keys_Update( 1 );
		vTaskDelay( DEBOUNCE_TIME / portTICK_RATE_MS );
	}
}
