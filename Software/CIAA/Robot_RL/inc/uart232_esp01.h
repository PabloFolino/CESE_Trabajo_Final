/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2020/08/24
 * Archivo: uart232_esp01.h
 * Version: 1
 *===========================================================================*/

#ifndef _UART232_ESP01_H_
#define _UART232_ESP01_H_

/*=====[Inclusions of public function dependencies]==========================*/
#include "sapi.h"

/*=====[Definition macros of public constants]===============================*/
#define LONG_BUFFER_RX 50
#define UART232_MS 100

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/
typedef enum {
	SIN_DATOS,
	DATO_RCV
} status_uart232_rx_t;

typedef struct {
	uint8_t buffer_rx[LONG_BUFFER_RX];		// Donde se guardan los datos
	status_uart232_rx_t status;				// Si hay o no datos
	uint8_t cuenta;							// Se usa paar barrer el vector
} uart232_rx_t;
/*=====[Prototypes (declarations) of public functions]=======================*/
void uart_esp01_init(void);
void buffer_rx_clean(uint8_t * p);

#endif /* End of File H */
