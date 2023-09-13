#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "esp32/rom/ets_sys.h"

#include "driver/gpio.h"
#include "driver/timer.h"
#include "esp_timer.h"
#include "esp_types.h"

#include "esp_common.h"
#include "spi_common.h"
#include "motor_common.h"
#include "spi.h"
#include "axis.h"

#include <memory>

#define LIMIT_PIN	(gpio_num_t) 27

static QueueHandle_t					msg_queue;
static QueueHandle_t					event_queue;
static std::shared_ptr<axis>			motor;
static std::shared_ptr<SPI::spiClient>	spi_client;

static move_msg_t						moveMsg;
static SPI::spiMsg						SPImsg;
static stepper_msg_out_t				stepperMsgOUT;
static stepper_msg_in_t					stepperMsgIN;
static EVENT_TYPE						event;

static void limit_switch_isr(void* arg)
{
	static constexpr EVENT_TYPE	limit_event	= LIMIT_SWITCH;
	BaseType_t wait = pdFALSE; 

	xQueueSendFromISR(event_queue, &limit_event, &wait);
}

static void recv_spi_msg()
{
	stepperMsgOUT = {};

	std::memset(SPImsg.m_recvbuf.data(), 0, SPImsg.m_recvbuf.size());
	
	stepperMsgOUT.status			= motor->status();
	stepperMsgOUT.step_position		= motor->step_position();
	stepperMsgOUT.cur_pulse			= motor->cur_step();
	stepperMsgOUT.decel_pulse		= motor->dec_step();
	stepperMsgOUT.ticks				= motor->ticks();
	stepperMsgOUT.divider			= motor->divider();
	stepperMsgOUT.cur_time			= motor->cur_time();

	stepperMsgOUT >> SPImsg.m_sendbuf;

	SPImsg.m_tr.length				= 8*SPImsg.m_recvbuf.size();
	SPImsg.m_tr.tx_buffer			= SPImsg.m_sendbuf.data();
	SPImsg.m_tr.rx_buffer			= SPImsg.m_recvbuf.data();

	spi_client->recv(SPImsg);

	xQueueSend(msg_queue, &SPImsg, 1);
}

static void event_task(void* arg)
{
	while(1)
	{
		if(xQueueReceive(event_queue, &event, portMAX_DELAY))
		{
			switch(event)
			{
				case SPI_RECV_MSG:
					recv_spi_msg();
					break;

				case LIMIT_SWITCH:
					motor->limit_switch_hit();
					break;

				default:
					break;
			}
		}
	}
}

static void msg_task(void* arg)
{
	SPI::spiMsg		msg;

	while(1)
	{
		if(xQueueReceive(msg_queue, &msg, portMAX_DELAY))
		{
			stepperMsgIN << msg.m_recvbuf;

			switch(stepperMsgIN.func)
			{
				case MOVE:
				{
					moveMsg << msg.m_recvbuf;
					motor->set_mode(moveMsg.mode);
					motor->move(	moveMsg.start,
									moveMsg.end,
									moveMsg.vf_mmps,
									moveMsg.radius,
									moveMsg.cw,
									moveMsg.dir);
					break;
				}

				case PRINT_INFO:			motor->print_info();								break;

				case SET_AXIS:				motor->set_axis(stepperMsgIN.set_data);				break;
				case SET_MODE:				motor->set_mode(stepperMsgIN.set_data);				break;
				case SET_SPMM:				motor->set_spmm(stepperMsgIN.set_data);				break;

				case SET_MIN_SPEED:			motor->set_min_speed(stepperMsgIN.set_data);		break;
				case SET_JOG_SPEED:			motor->set_jog_speed(stepperMsgIN.set_data);		break;
				case SET_MAX_SPEED:			motor->set_max_speed(stepperMsgIN.set_data);		break;

				case SET_ACCELERATION:		motor->set_accel(stepperMsgIN.set_data);			break;

				case STOP:					motor->stop();										break;
				case PAUSE:					motor->pause_timers((bool)stepperMsgIN.set_data);	break;

				default:
					break;
			}

		}
	}
}

extern "C" void app_main(void)
{
	gpio_install_isr_service(0);

	gpio_config_t io_conf;
	memset(&io_conf, 0, sizeof(io_conf));
	io_conf.intr_type		= GPIO_INTR_POSEDGE;
	io_conf.mode 			= GPIO_MODE_INPUT;
	io_conf.pull_down_en	= GPIO_PULLDOWN_ENABLE;
	io_conf.pin_bit_mask	= (1ULL << LIMIT_PIN);

	gpio_config(&io_conf);
	gpio_set_intr_type(LIMIT_PIN, GPIO_INTR_POSEDGE);
	gpio_isr_handler_add(LIMIT_PIN, limit_switch_isr, NULL);

	event_queue	= xQueueCreate(1, sizeof(EVENT_TYPE));
	msg_queue	= xQueueCreate(1, sizeof(SPI::spiMsg));
	spi_client	= std::make_shared<SPI::spiClient>(&event_queue);
	motor		= std::make_shared<axis>();

	motor->setup_gpio();
	motor->setup_timers();
	motor->set_msg_queue(&msg_queue);

	xTaskCreatePinnedToCore(event_task, "event_task", 4096, NULL, 1, NULL, 0);
	xTaskCreatePinnedToCore(msg_task, "msg_task", 4096, NULL, 1, NULL, 1);
}