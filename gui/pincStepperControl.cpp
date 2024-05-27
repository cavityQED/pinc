#include "pincStepperControl.h"

pincStepperControl::pincStepperControl(QWidget* parent) : QGroupBox(parent)
{
	spi_mode	= SPI_MODE_0;
	fd_CS0		= open("/dev/spidev0.0", O_RDWR);
	fd_CS1		= open("/dev/spidev0.1", O_RDWR);
	
	ioctl(fd_CS0, SPI_IOC_WR_MODE, &spi_mode);
	ioctl(fd_CS1, SPI_IOC_WR_MODE, &spi_mode);
}

void pincStepperControl::addStepper(pincStepperConfig_t* config)
{
	std::shared_ptr<pincPiStepper> new_stepper = std::make_shared<pincPiStepper>();

	std::memcpy(&new_stepper->config, config, sizeof(pincStepperConfig_t));
	pin_request_init(&new_stepper->spi_request, config->pin_spi_hs, 1, config->pin_req_mutex);

	sem_init(&new_stepper->sync_sem, 0, 0);

	new_stepper->status = 0xFF;

	std::memset(&new_stepper->fpga_spi_client.tr, 0, sizeof(spiTr));
	new_stepper->fpga_spi_client.cs					= -1;
	new_stepper->fpga_spi_client.fd					= fd_CS0;
	new_stepper->fpga_spi_client.mutex				= config->spi_mutex;
	new_stepper->fpga_spi_client.tr.speed_hz		= config->spi_fpga_speed;
	new_stepper->fpga_spi_client.tr.delay_usecs		= config->spi_delay;
	new_stepper->fpga_spi_client.tr.bits_per_word	= config->spi_bpw;
	new_stepper->fpga_spi_client.tr.cs_change		= config->spi_cs_change;

	std::memset(&new_stepper->pico_spi_client.tr, 0, sizeof(spiTr));
	new_stepper->pico_spi_client.cs					= config->spi_client_cs;
	new_stepper->pico_spi_client.fd					= fd_CS1;
	new_stepper->pico_spi_client.mutex				= config->spi_mutex;
	new_stepper->pico_spi_client.tr.speed_hz		= config->spi_pico_speed;
	new_stepper->pico_spi_client.tr.delay_usecs		= config->spi_delay;
	new_stepper->pico_spi_client.tr.bits_per_word	= config->spi_bpw;
	new_stepper->pico_spi_client.tr.cs_change		= config->spi_cs_change;

	memset(&new_stepper->jog_move, 0, sizeof(pincStepperMove_t));
	new_stepper->jog_move.steps		= config->spmm;
	new_stepper->jog_move.v0_sps	= config->min_speed;
	new_stepper->jog_move.vf_sps	= config->jog_speed;
	new_stepper->jog_move.accel		= config->accel;

	stepper_print_move(&new_stepper->jog_move);

	new_stepper->pin_status = config->pin_status;
	gpioSetMode(new_stepper->pin_status, PI_INPUT);
	gpioSetPullUpDown(new_stepper->pin_status, PI_PUD_UP);
	gpioSetISRFuncEx(new_stepper->pin_status, FALLING_EDGE, 0, stepper_pin_isr, new_stepper.get());

	stepper_config(new_stepper.get(), config);

	m_steppers.insert(std::make_pair((PINC_AXIS)config->axis, new_stepper));
}

void pincStepperControl::jog(PINC_AXIS axis, bool dir)
{
	if(m_steppers.contains(axis))
	{
		auto stepper = m_steppers.at(axis);

		stepper->jog_move.mode		= JOG_MOVE;
		stepper->jog_move.step_dir	= dir;
		stepper_jog(stepper.get());
	}
}

void pincStepperControl::sync_move(pincStepperMove_t* move, bool convert)
{
	move->mode		|= SYNC_MOVE;
	move->a_phase	= INITIAL;
	move->a_steps	= 0;
	move->steps 	= total(move->cur, move->end);

	std::vector<sem_t*> sems;
	pincStepperMove_t	steps_move;

	float linearize = (float)total(move->cur, move->end) / length(move->cur, move->end);
	move->v0_sps *= linearize;
	move->vf_sps *= linearize;

	for(auto it = m_steppers.begin(); it != m_steppers.end(); it++)
	{
		if(it->second->status & PICO_STATUS_MOVE_READY)
		{
			if(convert)
			{
				stepper_move_mm_to_steps(move, &steps_move, it->second->config.spmm);
				stepper_move(it->second.get(), &steps_move);
			}
			else
				stepper_move(it->second.get(), move);

			sems.push_back(&it->second->sync_sem);
		}
		else
			return;
	}
	
	gpioWrite(SYNC_PIN, 0);

	for(auto& sem : sems)
		sem_wait(sem);

	gpioWrite(SYNC_PIN, 1);
}