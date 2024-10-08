#ifndef HOST_STEPPER_H
#define HOST_STEPPER_H

#include <semaphore.h>

#include "stepper_common.h"
#include "c/spi/spi_client.h"
#include "c/io/pin_request.h"

#define FPGA_SPI_MSG_SIZE	2

#define X_SPI_HANDSHAKE		5
#define X_STATUS_INTERRUPT	21
#define X_FPGA_STATUS_ADDR	0xA0

#define Y_SPI_HANDSHAKE		6
#define Y_STATUS_INTERRUPT	13
#define Y_FPGA_STATUS_ADDR	0xB0

typedef struct
{
	uint8_t			cmd;
	uint32_t		size;
	uint8_t*		tx;
	uint8_t*		rx;

} pincPiStepperMsg;

typedef struct
{
	uint8_t				status;
	int					step_pos;

	int					pin_status;

	pin_request_t		spi_request;
	signal_t			status_sig;

	pincSPIclient_t		pico_spi_client;
	pincSPIclient_t		fpga_spi_client;
	pincStepperConfig_t	config;
	pincStepperMove_t	jog_move;

	uint8_t				fpga_status_addr;
	pincPiStepperMsg	msg;

	pthread_mutex_t		mutex;

	sem_t				sync_sem;

} pincPiStepper;

void	stepper_spi_send	(pincPiStepper* s);
void	stepper_config		(pincPiStepper* s, pincStepperConfig_t* config);
void	stepper_move		(pincPiStepper* s, pincStepperMove_t* move);
void	stepper_pause		(pincPiStepper* s, bool p);
void	stepper_jog			(pincPiStepper* s);
void	stepper_home		(pincPiStepper* s);
void	stepper_update		(pincPiStepper* s);
void	stepper_print		(pincPiStepper* s);
void	stepper_pin_isr		(int gpio, int level, uint32_t tick, void* dev);
void	stepper_write_msg	(pincPiStepper* stepper, uint8_t cmd, void* src, uint32_t bytes);
void	stepper_send_msg	(pincPiStepper* stepper);
void	stepper_read_msg	(pincPiStepper* stepper);

void*	stepper_read_fpga	(void* arg);

static inline void stepper_launch_thread(pincPiStepper* s, void* (*func)(void*))
{
	pthread_t thread;
	pthread_create(&thread, NULL, func, s);
	pthread_detach(thread);	
}

static inline void stepper_lock(pincPiStepper* s)
{
	pthread_mutex_lock(&s->mutex);
}

static inline void stepper_unlock(pincPiStepper* s)
{
	pthread_mutex_unlock(&s->mutex);
}

#endif