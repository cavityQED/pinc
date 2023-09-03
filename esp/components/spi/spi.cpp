#include "spi.h"

namespace SPI
{

QueueHandle_t*			spiClient::m_evtQueue	= nullptr;
spi_status_t			spiClient::m_status		= IDLE;

spiClient::spiClient(QueueHandle_t* handle)
{
	m_evtQueue = handle;

	//Clear the config structures to avoid warnings about
	//not setting all parameters
	memset(&bus_config, 0, sizeof(bus_config));
	memset(&slave_config, 0, sizeof(slave_config));
	
	//Bus Configuration
	bus_config.mosi_io_num	= MOSI;
	bus_config.miso_io_num	= MISO;
	bus_config.sclk_io_num	= SCLK;
	bus_config.quadwp_io_num = -1;	//Only needed for 4-bit transactions
	bus_config.quadhd_io_num = -1;	//Only needed for 4-bit transactions
	
	//Slave Interface Configuration
	slave_config.mode			= SPI_MODE;
	slave_config.spics_io_num	= CS;
	slave_config.queue_size		= QUEUE_SIZE;
	slave_config.post_setup_cb	= spi_post_setup_callback;
	slave_config.post_trans_cb	= spi_post_trans_callback;

	//Enable Pullups on SPI Pins
	gpio_set_pull_mode(MOSI, GPIO_PULLUP_ONLY);
	gpio_set_pull_mode(MISO, GPIO_PULLUP_ONLY);
	gpio_set_pull_mode(SCLK, GPIO_PULLUP_ONLY);
	gpio_set_pull_mode(CS, GPIO_PULLUP_ONLY);
	
	//Initialize SPI
	esp_err_t err = spi_slave_initialize(SPI2_HOST, &bus_config, &slave_config, DMA_CHANNEL);
	if(err != ESP_OK)
		std::cout << "Failed to initialize SPI\n";
		
	//Configure the  Handshake Signal
	io_config.intr_type			= GPIO_INTR_POSEDGE;
	io_config.mode	 			= GPIO_MODE_INPUT;
	io_config.pull_down_en		= GPIO_PULLDOWN_ENABLE;
	io_config.pull_up_en		= GPIO_PULLUP_DISABLE;
	io_config.pin_bit_mask		= (1ULL << HANDSHAKE);
	gpio_config(&io_config);
	gpio_isr_handler_add(HANDSHAKE, handshake_isr, NULL);

	//Configure the Ready Signal
	io_config.intr_type			= GPIO_INTR_DISABLE;
	io_config.mode				= GPIO_MODE_OUTPUT;
	io_config.pin_bit_mask		= (1ULL << READY);
	gpio_config(&io_config);
	WRITE_PERI_REG(GPIO_OUT_W1TC_REG, 1ULL<<READY);

	m_status = IDLE;
}

void spiClient::recv(spiMsg& msg)
{
//	std::cout << "\tWaiting for CS low\n";
	//Wait for the message
	msg.m_err = spi_slave_transmit(	SPI2_HOST,
									&msg.m_tr,
									msg.m_delay);

	m_status = IDLE;
}

void spiClient::spi_post_setup_callback(spi_slave_transaction_t* t)
{
	WRITE_PERI_REG(GPIO_OUT_W1TS_REG, 1ULL<<READY);
}

void spiClient::spi_post_trans_callback(spi_slave_transaction_t* t)
{
	WRITE_PERI_REG(GPIO_OUT_W1TC_REG, 1ULL<<READY);
}

void spiClient::handshake_isr(void* arg)
{
	if(m_status == IDLE)
	{		
		m_status = RECEIVING;
		int evt = 1;
		xQueueSendFromISR(*m_evtQueue, &evt, NULL);
	}
}

}//SPI namespace