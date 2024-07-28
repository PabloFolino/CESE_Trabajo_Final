/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2020/08/17
 * Archivo: fsm_robot.c
 * Version: 1
 *===========================================================================*/
/*Descripción: esta función lee el vector de los "temporizadores", y actualiza
 * el estado del robot. Los estados posibles son:
 *       ESPERA     --> el robot se encuentra en reposo
 *       ADELANTE  	--> mueve los dos motores con un velocidad positiva cte.
 *       ATRAS 		--> mueve los dos motores con un velocidad negativa cte.
 *       NN    		--> para posibles actualizaciones
 *===========================================================================*/

/*==================[inclusions]=============================================*/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "sapi.h"
#include "fsm_robot.h"
#include "keys.h"
#include "led.h"
#include "lcd.h"
#include "motor.h"
#include "bluetooth_hc05.h"
#include "bumpers.h"
#include "mpu9250.h"
#include "uart232_esp01.h"
#include "laser.h"
#include <string.h>


/*==================[macros and definitions]=================================*/
#define VEL_CTE 230
/*==================[internal data declaration]==============================*/
t_lcd_data lcd_data;
/*==================[internal functions declaration]=========================*/
void task_robot( void* );
uint8_t validar_dato_rx(uint8_t *p1);
void comando(uint8_t *p1, uint8_t p2);
uint8_t obtener_pwm(uint8_t *buffer);
/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/
extern estadoMotores_t motores;
extern uart232_rx_t data_uart232_rx;
/*==================[internal functions definition]==========================*/
void buffer_clean(uint8_t *buffer1);
/*==================[external functions definition]==========================*/
void fsmRobotInit(void* taskParam){
	BaseType_t res=0;
	estadoRobot_t * ptr_estado;
	ptr_estado=(estadoRobot_t*)taskParam;

	*ptr_estado=ESPERA;		// Condici�n inicial del Robot


	// Crear tareas en freeRTOS
	res = xTaskCreate (
			  task_robot,					// Funcion de la tarea a ejecutar
			  ( const char * )"task_robot",	// Nombre de la tarea como String amigable para el usuario
			  configMINIMAL_STACK_SIZE*4,	// Cantidad de stack de la tarea
			  (void*)  ptr_estado,		    // Parametros de tarea
			  tskIDLE_PRIORITY+1,			// Prioridad de la tarea
			  0								// Puntero a la tarea creada en el sistema
		  );

	// Gestion de errores
	configASSERT( (res == pdPASS));
}

void task_robot( void* taskParmPtr )
{
	estadoRobot_t * ptr_estadoTemp=(estadoRobot_t*)taskParmPtr;

	// obtengo el puntero al dato del LCD
	t_lcd_data *ptr_lcd_data;
	ptr_lcd_data=&lcd_data;

	ptr_lcd_data->cuenta=0;	// Lleva la cuenta de datos recibidos


	// Se inicia las distintas tareas de los distintos módulos
	// IMPORTNATE: en el módulo MPU9250 se configura el I2C
	//             debe estar antes que el módulo LCD
	bumpers_init();			// Configuro los bumper
	laser_init( );			// Carga el módulo de laser
	//init_bluetoothHC05();	// El módulo Bluetooth
	//keys_init();			// Configuro TEC1-TEC2 y crea 2 tareas
	led_init();				// Configuro los 2 Leds y crea 2 tareas
	motor_init();			// Configuro los 2 PWM de los motores
	mpu9250_init();			// Configuro el acelerómetro
	lcd_init(ptr_lcd_data);	// Configuro el display
	uart_esp01_init();		// La UART que se comunica con el Módulo ESP01



	TickType_t xPeriodicity = pdMS_TO_TICKS(ROBOT_MS); // Tarea periodica
	TickType_t xLastWakeTime = xTaskGetTickCount();

	while( 1 )
	{

		fsmRobotUpdate(ptr_estadoTemp);

		// Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}


// Tiene prioridad el movimiento hacia adelante
void fsmRobotUpdate(void* taskParam){
	estadoRobot_t * ptr_estadoTemp;
	ptr_estadoTemp=(estadoRobot_t*)taskParam;

	// *** Declaro un buffer temporal ***
	// La cte. LONG_BUFFER_RX se encuentra
	// definida en el uart232_esp01.h
	uint8_t buffer_rx_tmp[LONG_BUFFER_RX];
	uint8_t index, status_rx;

	// Recupero el puntero al dato del LCD
	t_lcd_data *ptr_lcd_data;
	ptr_lcd_data=&lcd_data;

	// Actualizo velocidades de los motores

//	motorEnable(MOT_DER,Motor_ON);
//	motorSpeed(120,MOT_DER);

	if(motores.mot1_ena==Motor_ON)
	motorSpeed(motores.mot1_veloc,motores.mot1_nombre);
	if(motores.mot2_ena==Motor_ON)
	motorSpeed(motores.mot2_veloc,motores.mot2_nombre);

	//Actualizo estado del Robot
	gpioToggle(LEDB);

	//***************  Recepción de datos de la UART 232 ***********************
	if(data_uart232_rx.status==DATO_RCV){
    	gpioToggle(LED3);
    	buffer_rx_clean(buffer_rx_tmp);
		// Copio el dato recibido
		for (index=0;index<data_uart232_rx.cuenta;index++){
			buffer_rx_tmp[index]=data_uart232_rx.buffer_rx[index];
		}
		// Limpio el buffer de rx
		buffer_rx_clean(data_uart232_rx.buffer_rx);
		// Seteo el estado
		data_uart232_rx.status=SIN_DATOS;
		// Verifica que el dato sea válido
		status_rx=validar_dato_rx(buffer_rx_tmp);
        if (status_rx==1){
        	ptr_lcd_data->cuenta++;
        	if(ptr_lcd_data->cuenta>999)ptr_lcd_data->cuenta=0;
        	// Proceso el dato recibido
        	comando(buffer_rx_tmp, strlen(buffer_rx_tmp));
        }

	}

}

// validar_dato_rx retorna un cero si la trama no es
// válida
uint8_t validar_dato_rx(uint8_t *buffer){
	uint8_t flag=0;
	uint8_t flag_stop;
	// Verifico el fin de la trama que debe ser "_55"
	//Busco el último '5'
	for (flag_stop=LONG_BUFFER_RX;flag_stop>3;flag_stop--){
		if(buffer[flag_stop]=='5') break;
		}
	if(flag_stop>=3){
		flag_stop--;
		if(buffer[flag_stop]=='5'){
			flag_stop--;
			if(buffer[flag_stop]=='_') flag=1;
			}
		}
	return flag;
}

// obtener_pwm devuelve un entero sin signo con el
// valor del pwm
uint8_t obtener_pwm(uint8_t *buffer ){
// Puede se una trama que tenga dos datos(long. 11) o
// puede ser una trama con 3 datos(Long. 12 caracteres)
	uint8_t i;
	uint8_t dato_str_pwm[3];
	uint8_t dato_pwm;
	uint8_t flag_stop;
	// Verifico el fin de la trama que debe ser "_55"
	//En flag_stop me queda el '_'
	for (flag_stop=LONG_BUFFER_RX;flag_stop>3;flag_stop--){
		if(buffer[flag_stop]=='5') break;
		}
	if(flag_stop>=3){
		flag_stop--;
		if(buffer[flag_stop]=='5'){
			flag_stop--;
			}
		}
	flag_stop--;
	if(flag_stop==8){		// Son tres caracteres
		for (i=flag_stop;i>(flag_stop-3);i--){
			dato_str_pwm[i-flag_stop+3]=buffer[i];
			}
		// Conversión a entero
		dato_pwm=atoi(dato_str_pwm);
	}
	if(flag_stop==7){		// Son dos caracteres
		for (i=flag_stop;i>(flag_stop-2);i--){
			dato_str_pwm[i-flag_stop+3]=buffer[i];
			}
		// Conversión a entero
		dato_pwm=atoi(dato_str_pwm);
	}
	return dato_pwm;
}

char* itoa(int value, char* result, int base) {
   // check that the base if valid
   if (base < 2 || base > 36) { *result = '\0'; return result; }

   char* ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}

void comando(uint8_t *command, uint8_t longitud){
	uint8_t flag,dato;
	uint8_t data_pwm;
	char mensaje[LONG_BUFFER_RX];
	char str_pwm[3],str_dato[3];
	flag=0;

	buffer_clean(mensaje);

	if(!strncmp(command,"LASER_DER=ON",12)){
		laser_set("LASER_DER", ON);
		uartWriteString( UART_232, "LASER_DER=ON\n");
		printf("LASER_DER=ON\n");
		flag=1;
		}
	if(!strncmp(command,"LASER_DER=OFF",13)){
		laser_set("LASER_DER", OFF);
		uartWriteString( UART_232, "LASER_DER=OFF\n");
		printf("LASER_DER=OFF\n");
		flag=1;
		}
	if(!strncmp(command,"LASER_IZQ=ON",12)){
		laser_set("LASER_IZQ", ON);
		uartWriteString( UART_232, "LASER_IZQ=ON\n");
		printf("LASER_IZQ=ON\n");
		flag=1;
		}
	if(!strncmp(command,"LASER_IZQ=OFF",13)){
		laser_set("LASER_IZQ", OFF);
		uartWriteString( UART_232, "LASER_IZQ=OFF\n");
		printf("LASER_IZQ=OFF\n");
		flag=1;
		}
	if(!strncmp(command,"PWM_0=",6)){					// ojo el comando recibido tiene el valor del PWM
		data_pwm=obtener_pwm(command);
		motorSpeed((int16_t) (data_pwm-128), MOT_DER);	// se reservan 3 caracteres para el dato del PWM
		motorEnable(Motor_ON,MOT_DER);
		itoa( data_pwm, str_pwm, 10 ); 					// base 10 significa decimal
		strcpy(mensaje,"PWM_0=");
		strcat(mensaje,str_pwm);
		strcat(mensaje,"\n");
		uartWriteString( UART_232, mensaje);
		printf("PWM_0=%c\n",str_pwm);
		flag=1;
		}
	if(!strncmp(command,"PWM_1=",6)){					// ojo el comando recibido tiene el valor del PWM
		data_pwm=obtener_pwm(command);
		motorSpeed((int16_t) (data_pwm-128), MOT_IZQ);	    // se reservan 3 caracteres para el dato del PWM
		motorEnable(Motor_ON,MOT_IZQ);
		itoa( data_pwm, str_pwm, 10 ); 					// base 10 significa decimal
		strcpy(mensaje,"PWM_1=");
		strcat(mensaje,str_pwm);
		strcat(mensaje,"\n");
		uartWriteString( UART_232, mensaje);
		printf("PWM_1=%c\n",str_pwm);
		flag=1;
		}
	if(!strncmp(command,"STOP_MOT",8)){
		motorSpeed(0, MOT_DER);
		motorSpeed(0, MOT_IZQ);
		motorEnable(Motor_ON,MOT_DER);
		motorEnable(Motor_ON,MOT_IZQ);
		uartWriteString( UART_232, "MOTOR_STOP\n");
		printf("MOTOR_STOP\n");
		flag=1;
		}
	if(!strncmp(command,"TEST_BUMPERS",12)){
		dato=0;
		if(!gpioRead(STOP_FRONT) && gpioRead(STOP_BACK)) dato=1;
		if(gpioRead(STOP_FRONT) && !gpioRead(STOP_BACK)) dato=2;
		if(!gpioRead(STOP_FRONT) && !gpioRead(STOP_BACK)) dato=3;
		itoa( dato, str_dato, 10 );
		strcpy(mensaje,"TEST_BUMPERS=");
		strcat(mensaje,str_dato);
		strcat(mensaje,"\n");
		uartWriteString( UART_232, mensaje);
		printf("%s\n",mensaje);
		flag=1;
		}
	if(flag==0)	printf("Comando no reconocido\n");
}

void buffer_clean(uint8_t *buffer1)
{
	uint8_t index;
	if(&buffer1==&data_uart232_rx.buffer_rx) data_uart232_rx.cuenta=0;
	for (index=0;index<LONG_BUFFER_RX;index++){
		buffer1[index]=' ';
	}
}




