#include "pincStepperControl.h"

pincStepperControl::pincStepperControl(QWidget* parent) : pincPanel("Position", parent)
{
	feedRate	= 10;
	
	spi_mode	= SPI_MODE_0;
	fd_CS0		= open("/dev/spidev0.0", O_RDWR);
	fd_CS1		= open("/dev/spidev0.1", O_RDWR);
	
	ioctl(fd_CS0, SPI_IOC_WR_MODE, &spi_mode);
	ioctl(fd_CS1, SPI_IOC_WR_MODE, &spi_mode);

	sem_init(&pin_req_sem, 0, 1);

	gpioSetMode(SYNC_PIN, PI_OUTPUT);
	gpioWrite(SYNC_PIN, 0);

	connect(	&m_timer,
				&QTimer::timeout,
				this,
				&pincStepperControl::update);

	layout = new QVBoxLayout();
	layout->setContentsMargins(10,10,10,10);
	setLayout(layout);
}

void pincStepperControl::addStepper(pincStepperConfig_t* config)
{
	std::shared_ptr<pincPiStepper> new_stepper = std::make_shared<pincPiStepper>();

	std::memcpy(&new_stepper->config, config, sizeof(pincStepperConfig_t));
	pin_request_init(&new_stepper->spi_request, config->pin_spi_hs, 1, &pin_req_sem);

	sem_init(&new_stepper->sync_sem, 0, 0);

	new_stepper->status = 0xFF;
	new_stepper->status_sig.cur = 0xFF;

	new_stepper->pin_status = config->pin_status;
	gpioSetMode(new_stepper->pin_status, PI_INPUT);
	gpioSetPullUpDown(new_stepper->pin_status, PI_PUD_UP);
	gpioSetISRFuncEx(new_stepper->pin_status, FALLING_EDGE, 0, stepper_pin_isr, new_stepper.get());

	std::memset(&new_stepper->fpga_spi_client.tr, 0, sizeof(spiTr));
	new_stepper->fpga_spi_client.cs					= -1;
	new_stepper->fpga_spi_client.fd					= fd_CS0;
	new_stepper->fpga_spi_client.mutex				= config->spi_mutex;
	new_stepper->fpga_spi_client.tr.speed_hz		= config->spi_fpga_speed;
	new_stepper->fpga_spi_client.tr.delay_usecs		= config->spi_delay;
	new_stepper->fpga_spi_client.tr.bits_per_word	= config->spi_bpw;
	new_stepper->fpga_spi_client.tr.cs_change		= config->spi_cs_change;
	new_stepper->fpga_status_addr					= config->fpga_addr;

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

	stepper_config(new_stepper.get(), config);

	m_steppers.insert(std::make_pair((PINC_AXIS)config->axis, new_stepper));

	pincPosition* pos = new pincPosition((PINC_AXIS)config->axis);
	layout->addWidget(pos);
	m_positions.insert(std::make_pair((PINC_AXIS)config->axis, pos));
}

double pincStepperControl::pos(PINC_AXIS axis)
{
	double pos = 0;

	if(m_steppers.contains(axis))
	{
		auto stepper = m_steppers.at(axis);
		pos = stepper->step_pos;
		pos /= stepper->config.spmm;
	}

	return pos;
}

void pincStepperControl::jog(PINC_AXIS axis, bool dir)
{
	if(m_steppers.contains(axis))
	{
		auto stepper = m_steppers.at(axis);

		stepper->jog_move.mode		= JOG_MOVE;
		stepper->jog_move.step_dir	= dir;
		if(stepper->status_sig.cur & PICO_STATUS_MOVE_READY)
		{
			stepper_jog(stepper.get());
			monitor_start(10);
		}
	}
}

void pincStepperControl::home(PINC_AXIS axis)
{

	if(m_steppers.contains(axis))
	{
		auto stepper = m_steppers.at(axis);

		stepper_home(stepper.get());
		monitor_start(10);
	}
}

void pincStepperControl::sync_move(pincMove* move)
{
	move->mode		|= SYNC_MOVE;

	std::vector<sem_t*> sems;
	pincStepperMove_t	step_move;

	for(auto it = m_steppers.begin(); it != m_steppers.end(); it++)
	{
		if(it->second->status_sig.cur & PICO_STATUS_MOVE_READY)
		{
			std::memset(&step_move, 0, sizeof(pincStepperMove_t));
			int spmm = it->second->config.spmm;

			step_move.mode		= move->mode;

			step_move.cur		= (move->cur * spmm).step();
			step_move.end		= (move->end * spmm).step();
			step_move.center	= (move->ctr * spmm).step();

			step_move.cw		= move->cw;
			step_move.vf_sps	= move->feed * spmm;
			step_move.radius	= move->radius * spmm;

			stepper_move(it->second.get(), &step_move);

			sems.push_back(&it->second->sync_sem);
		}

		else
			return;
	}

	for(auto& sem : sems)
		sem_wait(sem);

	gpioWrite(SYNC_PIN, 1);
	monitor_start(50);
}

void pincStepperControl::run(const gBlock& block)
{
	pincMove move;
	std::memset(&move, 0, sizeof(pincMove));

	for(auto it = block.args().begin(); it != block.args().end(); it++)
	{
		switch(it.key().unicode())
		{
			case 'F':
				feedRate	= it.value();
				move.feed	= feedRate;
				break;

			case 'X':
			case 'U':
				move.end.x = it.value();
				move.cur.x = (it.key() == 'X')? pos(X_AXIS) : 0;
				break;

			case 'Y':
			case 'V':
				move.end.y = it.value();
				move.cur.y = (it.key() == 'Y')? pos(Y_AXIS) : 0;
				break;

			case 'Z':
			case 'W':
				move.end.z = it.value();
				move.cur.z = (it.key() == 'Z')? pos(Z_AXIS) : 0;
				break;

			case 'I':
				move.ctr.x = move.cur.x + it.value();
				break;
			case 'J':
				move.ctr.y = move.cur.y + it.value();
				break;
			case 'K':
				move.ctr.z = move.cur.z + it.value();
				break;

			case 'R':
				move.radius = it.value();
				break;

			default:
				break;
		}

		if(move.feed == 0)
			move.feed = feedRate;
	}

	int block_int = (int)block.num();

	switch(block_int)
	{
		case 1:
			move.mode	= LINE_MOVE;
			move.end	= move.end - move.cur;
			move.cur	= {0,0,0};
			break;

		case 2:
		case 3:
			move.mode	= CURVE_MOVE;
			move.cw		= (block.num() == 2)? 1 : 0;
			if(!move.radius)
				move.radius	= move.ctr.distanceTo(move.cur);
			break;
	}

	sync_move(&move);
}

void pincStepperControl::update()
{
	bool no_motion	= true;
	bool move_ready	= true;

	for(auto it = m_steppers.begin(); it != m_steppers.end(); it++)
	{
		stepper_update(it->second.get());
		no_motion	= no_motion && (it->second->status_sig.cur & PICO_STATUS_IN_MOTION);
		move_ready	= move_ready && (it->second->status_sig.cur & PICO_STATUS_MOVE_READY);
		auto pos	= m_positions.at((PINC_AXIS)it->second->config.axis);
		if(pos)
			pos->set((double)it->second->step_pos / it->second->config.spmm);
	}

	if(no_motion)
	{
		gpioWrite(SYNC_PIN, 0);
		monitor_stop();
	}

	if(move_ready)
		emit moveReady();
}