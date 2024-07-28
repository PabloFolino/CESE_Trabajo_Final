/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2020/09/24
 * Archivo: lcd.c
 * Version: 1
 *===========================================================================*/
/*Descripciónn:
 *===========================================================================*/
/*==================[inlcusiones]============================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "lcd.h"
#include "motor.h"
/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/


/*==================[definiciones de datos externos]=========================*/
//t_lcd_data lcd_data;
extern uint16_t contador;
extern estadoMotores_t motores;
/*==================[declaraciones de funciones internas]====================*/
void task_lcd( void* taskParmPtr );
void uInToChar_sig(int16_t , char *,uint8_t);
void uInToChar(int16_t , char *,uint8_t);
/*==================[declaraciones de funciones externas]====================*/

void lcd_init(t_lcd_data * display)
{

	BaseType_t res=0;

	taskENTER_CRITICAL();		    // disable ISR

   //i2cInit( I2C0, CLOCK_RATE_I2C ); // El I2C ho habilita el mòdulo acelerómetro

   //delay( LCD_STARTUP_WAIT_MS );   // Wait for stable power (some LCD need that)

   // Inicializar LCD de 16x2 (caracteres x lineas) con cada caracter de 5x8 pixeles
   lcdInit( 16, 2, 5, 8 );
   lcdCursorSet( LCD_CURSOR_OFF ); // Apaga el cursor
   lcdClear();                     // Borrar la pantalla
   taskEXIT_CRITICAL();		   // enable ISR


	display->tOkString[0]='T';
	display->tOkString[1]='_';
	display->tOkString[2]='O';
	display->tOkString[3]='K';
	display->tOkString[4]=':';
	display->tOkString[5]='\0';
	display->countsOk[0]=' ';
	display->countsOk[1]=' ';
	display->countsOk[2]='0';
	display->countsOk[3]=' ';
	display->countsOk[4]='\0';
	display->cmdString[0]='C';
	display->cmdString[1]='M';
	display->cmdString[2]='D';
	display->cmdString[3]=' ';
	display->cmdString[4]=':';
	display->cmdString[5]='\0';
	display->comamdo[0]='N';
	display->comamdo[1]='N';
	display->comamdo[2]='N';
	display->comamdo[3]=' ';
	display->comamdo[4]='\0';
	display->motor0String[0]='M';
	display->motor0String[1]='0';
	display->motor0String[2]=':';
	display->motor0String[3]='\0';
	display->speedM0[0]=' ';
	display->speedM0[1]=' ';
	display->speedM0[2]='0';
	display->speedM0[3]='+';
	display->speedM0[4]='\0';
	display->motor1String[0]='M';
	display->motor1String[1]='1';
	display->motor1String[2]=':';
	display->motor1String[3]='\0';
	display->speedM1[0]=' ';
	display->speedM1[1]=' ';
	display->speedM1[2]='0';
	display->speedM1[3]='+';
	display->speedM1[4]='\0';


	//uInToChar(motores.mot1_veloc, display->countsOk,3);

	// Crear tareas en freeRTOS
	res = xTaskCreate (
			  task_lcd,						// Funcion de la tarea a ejecutar
			  ( const char * )"task_lcd",	// Nombre de la tarea como String amigable para el usuario
			  configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
			  (void*)  display,		        // Parametros de tarea
			  tskIDLE_PRIORITY+1,			// Prioridad de la tarea
			  0								// Puntero a la tarea creada en el sistema
		  );

	// Gestion de errores
	configASSERT( (res == pdPASS));

	printf("LCD                --> inicializado correctamente.\r\n" );
}

void task_lcd( void* taskParmPtr )
{
	t_lcd_data * ptr_lcd_data = (t_lcd_data *) taskParmPtr;

	TickType_t xPeriodicity = pdMS_TO_TICKS(DISPLAY_MS); // Tarea periodica
	TickType_t xLastWakeTime = xTaskGetTickCount();

	while( 1 )
	{
		//ptr_lcd_data=&lcd_data;
		uInToChar_sig(motores.mot1_veloc, ptr_lcd_data->speedM0,3);
		uInToChar_sig(motores.mot2_veloc, ptr_lcd_data->speedM1,3);

		uInToChar(ptr_lcd_data->cuenta, ptr_lcd_data->countsOk,3);

		lcd_display(ptr_lcd_data);

		// Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

void lcd_display( t_lcd_data * display){
	lcdGoToXY( 0, 0 ); // Poner cursor en 0, 0
	lcdSendStringRaw(display->tOkString);
	lcdGoToXY( 5, 0 );
	lcdSendStringRaw(display->countsOk);
	lcdGoToXY( 0, 1 );
	lcdSendStringRaw( display->cmdString);
	lcdGoToXY( 5, 1 );
	lcdSendStringRaw(display->comamdo);
	lcdGoToXY( 9 , 0 );
	lcdSendStringRaw( display->motor0String );
	lcdGoToXY(12 , 0 );
	lcdSendStringRaw(display->speedM0);
	lcdGoToXY( 9 , 1 );
	lcdSendStringRaw( display->motor1String );
	lcdGoToXY(12 , 1 );
	lcdSendStringRaw(display->speedM1);
}

void uInToChar_sig(int16_t value, char *str,uint8_t cant)
{
	uint8_t i,j=cant-1;

	//evaluo el signo
	str[cant]='+';
    if(value<0){
    str[cant]='-';
    value=-value;
    }

	for(i=0;i<cant;i++){
		str[j]=(value%10)+'0';
		value=value/10;
		j--;
	}

}

void uInToChar(int16_t value, char *str,uint8_t cant)
{
	uint8_t i,j=cant-1;

	for(i=0;i<cant;i++){
		str[j]=(value%10)+'0';
		value=value/10;
		j--;
	}

}
/*==================[fin del archivo]========================================*/
