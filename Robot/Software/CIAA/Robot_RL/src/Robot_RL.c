/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2020/09/24
 * Archivo: Robot_RL.c
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "sapi.h"
#include "fsm_robot.h"
#include "motor.h"
#include "keys.h"
#include "mpu9250.h"

/*=====[Definition & macros of public constants]==============================*/
#define MSG_1   "\n CESE-FIUBA-T.P.Folino \n"

/*==================[internal functions declaration]=========================*/

/*=====[Definitions of extern global functions]==============================*/
//extern	t_lcd_data lcd_data;
/*=====[Definitions of public global variables]==============================*/
uint16_t contador=VALOR_INICIAL;
estadoRobot_t estado=NN;


/*=====[Main function, program entry point after power on or reset]==========*/

int main( void )
{
	// ---------- CONFIGURACIONES ------------------------------
	boardConfig();  // Inicializar y configurar la plataforma

	printf( MSG_1 );

	// En la variable "estado" se evalúa el estado gral del robot

	estadoRobot_t *ptr_estado;
	ptr_estado=&estado;

	fsmRobotInit(ptr_estado);

	// Iniciar scheduler
	vTaskStartScheduler();		// Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

	/* realizar un assert con "false" es equivalente al while(1) */
	configASSERT( 0 );

	// ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
       // Si cae en este while 1 significa que no pudo iniciar el scheduler
    }

    // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
    // directamenteno sobre un microcontroladore y no es llamado por ningun
    // Sistema Operativo, como en el caso de un programa para PC.
    return 0;
}

/* hook que se ejecuta si al necesitar un objeto dinamico, no hay memoria disponible */
void vApplicationMallocFailedHook()
{
	printf( "Malloc Failed Hook!\n" );
	configASSERT( 0 );
}
