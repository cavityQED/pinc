#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "esp32/rom/ets_sys.h"

#include "driver/gpio.h"
#include "driver/timer.h"
#include "esp_timer.h"
#include "esp_types.h"

#include "spi_common.h"
#include "motor_common.h"
#include "spi.h"
#include "axis.h"

#include <memory>

static QueueHandle_t					msg_queue;

static std::shared_ptr<axis>			motor;
static std::shared_ptr<SPI::spiMsg>		msg_ptr;

void printBytes(const std::vector<uint8_t>& buf)
{
	size_t count = 0;
	while(count < buf.size())
	{
		if(!(count % 8))
			printf("\n%u:\t", count / 8);
		printf("\t%#02x", buf[count++]);
	}
	printf("\n\n");
}

static void spi_task(void* arg)
{
	QueueHandle_t					spi_queue	= xQueueCreate(1, sizeof(int));
	std::shared_ptr<SPI::spiClient> spi_client	= std::make_shared<SPI::spiClient>(&spi_queue);

	stepper_msg_out_t	msgOUT;
	stepper_msg_in_t	msgIN;
	move_msg_t			movemsg;
	std::shared_ptr<SPI::spiMsg> 	msg;

	msg_ptr = std::make_shared<SPI::spiMsg>();

	while(1)
	{
		msg		= std::make_shared<SPI::spiMsg>();
		msgOUT	= {};

		int recv_evt;

		if(xQueueReceive(spi_queue, &recv_evt, portMAX_DELAY))
		{
			std::memset(msg_ptr->m_recvbuf.data(), 0, msg_ptr->m_recvbuf.size());

			msgOUT.status			= motor->status();
			msgOUT.step_position	= motor->step_position();
			msgOUT.cur_pulse		= motor->cur_step();
			msgOUT.decel_pulse		= motor->dec_step();
			msgOUT.ticks			= motor->ticks();
			msgOUT.divider			= motor->divider();
			msgOUT.cur_time			= motor->cur_time();

			msgOUT >> msg_ptr->m_sendbuf;
			msg_ptr->m_tr.length		= 8*msg_ptr->m_recvbuf.size();
			msg_ptr->m_tr.tx_buffer		= msg_ptr->m_sendbuf.data();
			msg_ptr->m_tr.rx_buffer		= msg_ptr->m_recvbuf.data();

			spi_client->recv(*msg_ptr);

			if(msg_ptr->m_err == ESP_OK)
				xQueueSend(msg_queue, &msg_ptr, 1);
		}
	}
}

static void msg_task(void* arg)
{
	std::shared_ptr<SPI::spiMsg>	msg;
	stepper_msg_in_t 				msgIN;
	move_msg_t						movemsg;

	while(1)
	{
		if(xQueueReceive(msg_queue, &msg, portMAX_DELAY))
		{
			msgIN << msg->m_recvbuf;

			switch(msgIN.func)
			{
				case MOVE:
				{
					movemsg << msg->m_recvbuf;
					motor->set_mode(movemsg.mode);
					motor->move(	movemsg.start,
									movemsg.end,
									movemsg.vf_mmps,
									movemsg.radius,
									movemsg.cw,
									movemsg.dir);
					break;
				}

				case PRINT_INFO:			motor->print_info();						break;

				case SET_AXIS:				motor->set_axis(msgIN.set_data);			break;
				case SET_MODE:				motor->set_mode(msgIN.set_data);			break;
				case SET_SPMM:				motor->set_spmm(msgIN.set_data);			break;

				case SET_MIN_SPEED:			motor->set_min_speed(msgIN.set_data);		break;
				case SET_JOG_SPEED:			motor->set_jog_speed(msgIN.set_data);		break;
				case SET_MAX_SPEED:			motor->set_max_speed(msgIN.set_data);		break;

				case SET_ACCELERATION:		motor->set_accel(msgIN.set_data);			break;

				case STOP:					motor->stop();								break;
				case PAUSE:					motor->pause_timers((bool)msgIN.set_data);	break;

				default:
					break;
			}

		}
	}
}

extern "C" void app_main(void)
{
	gpio_install_isr_service(0);

	motor = std::make_shared<axis>();

	motor->setup_gpio();
	motor->setup_timers();

	std::shared_ptr<SPI::spiMsg> tmp;

	msg_queue = xQueueCreate(1, sizeof(tmp));

	xTaskCreatePinnedToCore(spi_task, "spi_task", 4096, NULL, 1, NULL, 0);
	xTaskCreatePinnedToCore(msg_task, "msg_task", 4096, NULL, 1, NULL, 1);
}