/*=============================================================================
 * Author: Pablo Daniel Folino  <pfolino@gmail.com>
 * Date: 2020/08/17
 * Archivo: fsm_robot.h
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of public function dependencies]==========================*/

/*=====[Definition macros of public constants]===============================*/
#define ROBOT_MS 100
/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/
typedef enum{
	ESPERA,
	ADELANTE,
	ATRAS,
	GIRO,
	BUMPERS_BACK,
	BUMBERS_FRONT,
	NN
} estadoRobot_t;
/*=====[Prototypes (declarations) of public functions]=======================*/
void fsmRobotInit(void* taskParam);
void fsmRobotUpdate(void* taskParam);

/*=====[Prototypes (declarations) of public interrupt functions]=============*/
