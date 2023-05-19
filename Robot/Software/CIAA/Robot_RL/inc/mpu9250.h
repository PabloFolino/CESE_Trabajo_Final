/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2021/08/24
 * Archivo: mpu9250.h
 * Version: 1
 *===========================================================================*/

#ifndef _MPU9250_H_
#define _MPU9250_H_

/*=====[Inclusions of public function dependencies]==========================*/
#include "sapi.h"
#include "sapi_imu_mpu9250_freeRTOS.h"

/*=====[Definition macros of public constants]===============================*/
#define MPU9250_MS	200			// Tiempo que actualiza el acelerómetro
/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/*=====[Prototypes (declarations) of public functions]=======================*/
void mpu9250_init(void);

#endif /* End of File H */
