#ifndef SPI_H
#define SPI_H

#include "driver/gpio.h"
#include "driver/spi_slave.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp32/rom/ets_sys.h"

#include <cstring>
#include <iostream>
#include <vector>
#include <cmath>

#include "spi_common.h"
#include "motor_common.h"

#define MOSI		(gpio_num_t) 23
#define MISO		(gpio_num_t) 19
#define SCLK		(gpio_num_t) 18
#define CS			(gpio_num_t) 5
#define HANDSHAKE	(gpio_num_t) 33
#define READY		(gpio_num_t) 13

#define SPI_MODE		3
#define DMA_CHANNEL		1
#define QUEUE_SIZE		1
#define MAX_TR_LENGTH	128

namespace SPI
{

struct spiMsg
{
	std::vector<uint8_t>		m_sendbuf = std::vector<uint8_t>(128);
	std::vector<uint8_t>		m_recvbuf = std::vector<uint8_t>(128);
	TickType_t					m_delay = portMAX_DELAY;
	esp_err_t					m_err = ESP_OK;
	STEPPER_MOTOR_CMD			m_cmd;
	spi_slave_transaction_t		m_tr;
};

class spiClient
{

public:

	spiClient(QueueHandle_t* handle);
	~spiClient() {}

	void recv(spiMsg& msg);

	/*	SPI Post Setup Callback
	* 		Executes after spi transaction is set up to signal
	* 		to the host device that a message is ready to be received
	* 		by setting the READY pin high
	*/
	static void IRAM_ATTR spi_post_setup_callback(spi_slave_transaction_t *t);

	/*	SPI Post Transaction Callback
	* 		Executes after spi transaction is completed
	* 		Sets READY pin low again
	*/
	static void IRAM_ATTR spi_post_trans_callback(spi_slave_transaction_t *t);
	
	/*	SPI Handshake pin interrupt handler
	*
	*/
	static void IRAM_ATTR handshake_isr(void* arg);

public:

	bool receiving()	{return m_status == RECEIVING;}

protected:

	spi_bus_config_t				bus_config;
	spi_slave_interface_config_t	slave_config;
	gpio_config_t					io_config;

	static QueueHandle_t*			m_evtQueue;					
	static spi_status_t				m_status;
};

}

#endif