/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2020/08/17
 * Archivo: motor.c
 * Version: 1
 *===========================================================================*/
/*Descripción: este módulo maneja los dos motores(MOT_DER o MOT_IZQ) mediante
 * dos señanes PWM. Para el sentido de giro se usan 4 pines gpio, dos para el
 * motor derecho(T_COL0 y T_FIL2), dos para el motor izquierdo (T_COL1 y
 * T_FIL2)
 *===========================================================================*/

/*==================[inclusions]=============================================*/

#include "motor.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
estadoMotores_t motores;
/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
/* Se establecen los límites de velocidades */
static uint8_t maxVel=MAX_VELOCIDAD;
static uint8_t minVel=MIN_VELOCIDAD;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
void motorSpin(spinMotor_t  spinMotor, numeroMotor_t numeroMotor){
	switch (numeroMotor){
		case MOT_DER:
			if(spinMotor == MOT_ADELANTE){
				gpioWrite(M1_ENA2,OFF);
				gpioWrite(M1_ENA1,ON);
				}
			if(spinMotor == MOT_ATRAS){
				gpioWrite(M1_ENA1,OFF);
				gpioWrite(M1_ENA2,ON);
				}
			break;
		case MOT_IZQ:
			if(spinMotor == MOT_ADELANTE){
				gpioWrite(M2_ENA2,OFF);
				gpioWrite(M2_ENA1,ON);
				}
			if(spinMotor == MOT_ATRAS){
				gpioWrite(M2_ENA1,OFF);
				gpioWrite(M2_ENA2,ON);
				}
			break;
		}
}


/*==================[external functions definition]==========================*/
// Función que inicializa la tarea
void motor_init( void ){
	/* Se configura los pines del sentido de giro
	 * IMPORTANTE: nunca deben estar los dos pines en nivel alto
	 */
	gpioInit(M1_ENA1, GPIO_OUTPUT);				//Motor derecho
	gpioInit(M1_ENA2, GPIO_OUTPUT);
	gpioInit(M2_ENA1, GPIO_OUTPUT);				//Motor izquierdo
	gpioInit(M2_ENA2, GPIO_OUTPUT);

	gpioWrite(M1_ENA1,OFF);
	gpioWrite(M1_ENA2,OFF);
	gpioWrite(M2_ENA1,OFF);
	gpioWrite(M2_ENA2,OFF);


   /* Se configura los PWMs */
   pwmConfig( 0, PWM_ENABLE );

   pwmInit(MOT_DER,PWM_ENABLE_OUTPUT );		//Motor derecho
   pwmInit(MOT_IZQ,PWM_ENABLE_OUTPUT ); 	//Motor izquierdo


   /* Se establece el valor de velocidad inicial */
   pwmWrite( MOT_DER,0 );				//Motor derecho
   pwmWrite( MOT_IZQ,0);				//Motor izquierdo

   motores.mot1_nombre=MOT_DER;
   motores.mot1_veloc=MAX_VELOCIDAD / 2;
   motores.mot1_ena=Motor_OFF;
   motores.mot1_spin=MOT_ADELANTE;
   motores.mot2_nombre=MOT_IZQ;
   motores.mot2_veloc=MAX_VELOCIDAD / 2;
   motores.mot2_ena=Motor_OFF;
   motores.mot2_spin=MOT_ADELANTE;

   printf("Motores          --> inicializados correctamente.\r\n" );

}

/*Esta función contempla la zona muerta del motor con los valores
* de las variables "maxVel" y "minVel"
*/
bool_t motorSpeed(int16_t velocidad, bool_t numeroMotor){
	switch (numeroMotor){
		case MOT_DER:
			if(velocidad>0){
				motorSpin(MOT_ADELANTE,numeroMotor);
				if (velocidad<=maxVel && velocidad>=minVel){
						pwmWrite( MOT_DER,(uint8_t)velocidad );
					}
					else{
						if(velocidad>maxVel) pwmWrite( MOT_DER,(uint8_t)maxVel);
						//if(velocidad<minVel) pwmWrite( MOT_DER,(uint8_t)minVel);
						if(velocidad<minVel) pwmWrite( MOT_DER,0);
					}
				}
			if(velocidad<0){
				motorSpin(MOT_ATRAS,numeroMotor);
				velocidad=-velocidad;
				if (velocidad<=maxVel && velocidad>=minVel){
						pwmWrite( MOT_DER,(uint8_t)velocidad );
					}
					else{
						if(velocidad>maxVel) pwmWrite( MOT_DER,(uint8_t)maxVel);
						//if(velocidad<minVel) pwmWrite( MOT_DER,(uint8_t)minVel);
						if(velocidad<minVel) pwmWrite( MOT_DER,0);
					}
				}
			if(velocidad==0) pwmWrite(MOT_DER,0);
			break;
		case MOT_IZQ:
			if(velocidad>0){
				motorSpin(MOT_ADELANTE,numeroMotor);
				if (velocidad<=maxVel && velocidad>=minVel){
						pwmWrite( MOT_IZQ,(uint8_t)velocidad );
					}
					else{
						if (velocidad>maxVel) pwmWrite( MOT_IZQ,(uint8_t)maxVel);
						//if (velocidad<minVel) pwmWrite( MOT_IZQ,(uint8_t)minVel);
						if(velocidad<minVel) pwmWrite( MOT_IZQ,0);
					}
				}
			if(velocidad<0){
				motorSpin(MOT_ATRAS,numeroMotor);
				velocidad=-velocidad;
				if (velocidad<=maxVel && velocidad>=minVel){
						pwmWrite( MOT_IZQ,(uint8_t)velocidad );
					}
					else{
						if(velocidad>maxVel) pwmWrite( MOT_IZQ,(uint8_t)maxVel);
						//if(velocidad<minVel) pwmWrite( MOT_IZQ,(uint8_t)minVel);
						if(velocidad<minVel) pwmWrite( MOT_IZQ,0);
					}
				}
			if(velocidad==0) pwmWrite(MOT_IZQ,0);
			break;
	}
}


void motorEnable(enableDisable_t enableDisable, numeroMotor_t numeroMotor){
	switch (numeroMotor){
		case MOT_DER:
			if(enableDisable == Motor_ON) pwmInit( MOT_DER, PWM_ENABLE_OUTPUT);
			if(enableDisable == Motor_OFF)pwmInit( MOT_DER, PWM_DISABLE_OUTPUT);
			break;
		case MOT_IZQ:
			if(enableDisable == Motor_ON) pwmInit( MOT_IZQ, PWM_ENABLE_OUTPUT);
			if(enableDisable == Motor_OFF)pwmInit( MOT_IZQ, PWM_DISABLE_OUTPUT);
			break;
		}
}


// Límites de  velocidades (se prevee la zona muerta del motor)
void motorSpeedMax(uint8_t valor){
maxVel=valor;
}

void motorSpeedMin(uint8_t valor){
minVel=valor;
}


