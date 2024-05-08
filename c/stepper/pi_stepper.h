#ifndef HOST_STEPPER_H
#define HOST_STEPPER_H

#include "c/spi/spi_client.h"
#include "c/io/pin_request.h"
#include "stepper_common.h"

#define X_SPMM					800

#define X_SPI_HANDSHAKE		6
#define X_STATUS_INTERRUPT	21
#define X_FPGA_STATUS_ADDR	0xA0

#define Y_SPI_HANDSHAKE		5
#define Y_STATUS_INTERRUPT	13
#define Y_FPGA_STATUS_ADDR	0xB0

#define PICO_STATUS_SPI_READY	0x01
#define PICO_STATUS_MOTION		0x08
#define PICO_STATUS_SYNC_READY	0x10

typedef struct
{
	uint8_t				status;
	int					step_pos;

	int					pin_status;

	pin_request_t		spi_request;
	pincSPIclient_t		pico_spi_client;
	pincSPIclient_t		fpga_spi_client;
	pincStepperConfig_t	config;
	pincStepperMove_t	jog_move;

	uint8_t*			tx;
	uint8_t*			rx;
	
	pthread_mutex_t		mutex;

} pincPiStepper;

void stepper_spi_send	(pincPiStepper* s);
void stepper_config		(pincPiStepper* s, pincStepperConfig_t* config);
void stepper_move		(pincPiStepper* s, pincStepperMove_t* move);
void stepper_jog		(pincPiStepper* s);
void stepper_cmd		(pincPiStepper* s, uint8_t cmd, void* data, uint32_t bytes);
void stepper_update		(pincPiStepper* s);
void stepper_print		(pincPiStepper* s);

void* stepper_thread_routine(void* arg);
void stepper_pin_isr(int gpio, int level, uint32_t tick, void* dev);

static inline void stepper_lock(pincPiStepper* s)
{
	pthread_mutex_lock(&s->mutex);
}

static inline void stepper_unlock(pincPiStepper* s)
{
	pthread_mutex_unlock(&s->mutex);
}

#endif