#include "axis.h"

/*	Initialize static variables	*/

step_vec_t		axis::m_cur_pos			= {};
step_vec_t		axis::m_end_pos			= {};

uint8_t			axis::m_status			= 0x00;
uint8_t			axis::m_axis			= 0x00;
uint8_t			axis::m_step_axis		= 0xFF;
uint32_t		axis::m_spmm			= 0;
uint32_t		axis::m_ticks			= 1;

uint32_t		axis::m_cur_speed		= 0;
uint32_t		axis::m_end_speed		= 2000;
uint32_t		axis::m_jog_speed		= 0;
uint32_t		axis::m_min_speed		= 2000;
uint32_t		axis::m_max_speed		= 0;

uint32_t		axis::m_cur_divider		= 0;
uint32_t		axis::m_max_divider		= 0;

uint32_t		axis::m_cur_step		= 0;
uint32_t		axis::m_acc_step		= 0;
uint32_t		axis::m_dec_step		= 0;
uint32_t		axis::m_end_step		= 0;

uint32_t		axis::m_accel			= 0;
uint32_t		axis::m_radius			= 0;
uint32_t		axis::m_cw				= 0;
double			axis::m_slope			= 0;

int				axis::m_step_position	= 0;
double			axis::m_cur_time		= 0;

esp_err_t		axis::m_esp_err			= ESP_OK;
xQueueHandle	axis::m_syncSem			= xSemaphoreCreateBinary();

void axis::check_error(const char* msg)
{
	if(m_esp_err != ESP_OK) {
		std::cout << msg << "\n\t" << esp_err_to_name(m_esp_err);
	}
}

void axis::setup_gpio()
{
	/*	Setup the Sync Pin	*/
	gpio_config_t io_conf;
	memset(&io_conf, 0, sizeof(io_conf));
	io_conf.intr_type		= GPIO_INTR_POSEDGE;
	io_conf.mode 			= GPIO_MODE_INPUT;
	io_conf.pull_down_en	= GPIO_PULLDOWN_ENABLE;
	io_conf.pin_bit_mask	= (1ULL << SYNC_PIN);
	
	gpio_config(&io_conf);	
	gpio_set_intr_type(SYNC_PIN, GPIO_INTR_POSEDGE);
	gpio_isr_handler_add(SYNC_PIN, syncSem_release_isr, NULL);

	/*	Set the pulse, direction, and enable pins as outputs	*/
	gpio_pad_select_gpio(PULSE_PIN);
	m_esp_err = gpio_set_direction(PULSE_PIN, GPIO_MODE_OUTPUT);
	check_error("PULSE_PIN Configure:");

	gpio_pad_select_gpio(DIR_PIN);
	m_esp_err = gpio_set_direction(DIR_PIN, GPIO_MODE_OUTPUT);
	check_error("DIR_PIN Configure:");

/*
	gpio_pad_select_gpio(EN_PIN);
	m_esp_err = gpio_set_direction(EN_PIN, GPIO_MODE_OUTPUT);
	check_error("EN_PIN Configure:");
	gpio_set_level(EN_PIN, 1);
*/
	
	gpio_pad_select_gpio(MOTION_PIN);
	m_esp_err = gpio_set_direction(MOTION_PIN, GPIO_MODE_OUTPUT);
	check_error("MOTION_PIN Configure:");
	gpio_set_level(MOTION_PIN, 0);

}

void axis::setup_timers()
{
	timer_config_t	timer_config;
	memset(&timer_config, 0, sizeof(timer_config));

	timer_config.divider		= DIVIDER_US;		//Set the divider so default timer counter period is 1 microsecond
	timer_config.counter_dir	= TIMER_COUNT_UP;	//Set counter to increase in value
	timer_config.counter_en		= TIMER_PAUSE;
	timer_config.intr_type		= TIMER_INTR_LEVEL;

	/*	SECONDS_TIMER config
	*		Seconds timer does not auto-reload (one-shot) and does not have an
	*		alarm enabled; counts until stopped. Used to get time passed in seconds during a move
	*/
	timer_config.auto_reload	= TIMER_AUTORELOAD_DIS;
	timer_config.alarm_en		= TIMER_ALARM_DIS;

	/*	Initiate the seconds timers	*/
	timer_init(STEP_GROUP, SECONDS_TIMER, &timer_config);

	/*	STEP_TIMER config
	*		Pulse timer should auto-reload and have its alarm enabled
	*/
	timer_config.auto_reload	= TIMER_AUTORELOAD_EN;
	timer_config.alarm_en		= TIMER_ALARM_EN;

	/*	Initiate the pulse timers
	*		Initiate step timer and register the interrupt handler 
	*/
	timer_init(STEP_GROUP, STEP_TIMER, &timer_config);
	timer_isr_register(STEP_GROUP, STEP_TIMER, step_timer_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);

	m_status |= MOVE_READY;
}

void axis::reset_timers()
{
	timer_pause					(STEP_GROUP, STEP_TIMER);
	timer_pause					(STEP_GROUP, SECONDS_TIMER);

	timer_disable_intr			(STEP_GROUP, STEP_TIMER);

	timer_set_counter_value		(STEP_GROUP, STEP_TIMER, 0);
	timer_set_counter_value		(STEP_GROUP, SECONDS_TIMER, 0);

	timer_set_alarm				(STEP_GROUP, STEP_TIMER, TIMER_ALARM_EN);

}

void axis::pause_timers(bool pause)
{
	if(!pause && (m_status & IN_MOTION))
	{
		gpio_set_level(MOTION_PIN, 1);
		timer_start(STEP_GROUP, STEP_TIMER);
		timer_start(STEP_GROUP, SECONDS_TIMER);
	}

	else if(pause)
	{
		gpio_set_level(MOTION_PIN, 0);
		timer_pause(STEP_GROUP, STEP_TIMER);
		timer_pause(STEP_GROUP, SECONDS_TIMER);
	}
}

void axis::reset()
{
	m_cur_step		= 0;
	m_cur_time		= 0;
	m_cur_speed		= m_min_speed;
	m_end_step		= TIMER_BASE_CLK;
	m_acc_step		= TIMER_BASE_CLK;
	m_dec_step		= m_acc_step;

	reset_timers();
}

void axis::move(	const position_t<double>&	start,
					const position_t<double>&	end,
					double						final_speed,
					double						r,
					bool						cw,
					bool						dir)
{
	if(m_status & IN_MOTION)
		return;

	m_status &= ~MOVE_READY;
	reset();

	if(dir)
		m_status |= DIRECTION;
	else
		m_status &= ~DIRECTION;

	std::cout << "Steps/mm:\t" << m_spmm << '\n';

	m_cur_pos.x	= std::round(start.x * m_spmm);
	m_cur_pos.y	= std::round(start.y * m_spmm);
	m_cur_pos.z	= std::round(start.z * m_spmm);
	m_end_pos.x	= std::round(end.x * m_spmm);
	m_end_pos.y	= std::round(end.y * m_spmm);
	m_end_pos.z	= std::round(end.z * m_spmm);
	m_slope		= step_vec_t::slope(m_end_pos, m_cur_pos);
	m_end_speed	= final_speed * m_spmm;
	m_radius	= r * m_spmm;;
	m_cw		= cw;
	m_ticks		= 1000000 / m_end_speed;
	if(m_ticks % 2)
		m_ticks--;

	m_max_divider		= DIVIDER_US * m_end_speed / m_min_speed;
	m_cur_divider		= m_max_divider;

	std::cout << "Start:\t\t" << m_cur_pos << '\n';
	std::cout << "End:\t\t" << m_end_pos << '\n';
	std::cout << "Radius:\t\t" << m_radius << '\n';
	std::cout << "Clockwise:\t" << m_cw << '\n';
	std::cout << "Divider us:\t" << DIVIDER_US << '\n';
	std::cout << "Divider cur:\t" << m_cur_divider << '\n';
	std::cout << "End Speed:\t" << m_end_speed << '\n';
	std::cout << "Min Speed:\t" << m_min_speed << '\n';
	std::cout << "Alarm Value:\t" << m_ticks << '\n';
	std::cout << "Slope:\t\t" << m_slope << '\n';

	if(m_status & JOG_MODE)
	{
		m_end_step = m_end_pos.sum();
		m_step_axis = m_axis;
		gpio_set_level(DIR_PIN, m_status & DIRECTION);
	}
	else if(m_status & LINE_MODE)
	{
		m_end_step = std::abs((m_end_pos - m_cur_pos).axis((AXIS)m_axis));
		linear_interpolation_2D();
	}
	else if(m_status & CURV_MODE)
		circular_interpolation_2D();

	std::cout << "End Step:\t" << m_end_step << '\n';
	std::cout << "Starting Dir:\t" << (int)(m_status & DIRECTION) << '\n';

	timer_enable_intr		(STEP_GROUP, STEP_TIMER);
	timer_set_divider		(STEP_GROUP, STEP_TIMER, m_max_divider);
	timer_set_alarm_value	(STEP_GROUP, STEP_TIMER, m_ticks);

	if(m_status & SYNC_MODE)
	{
		m_status |= SYNC_READY;
		m_esp_err = xSemaphoreTake(m_syncSem, portMAX_DELAY);
	}

	if(m_end_step)
	{
		m_status |= IN_MOTION;

		gpio_set_level(MOTION_PIN, 1);
		if(timer_start(STEP_GROUP, STEP_TIMER) != ESP_OK)
			std::cout << "Step Timer Start Error\n";
		if(timer_start(STEP_GROUP, SECONDS_TIMER) != ESP_OK)
			std::cout << "Seconds Timer Start Error\n";
	}
	else
		m_status |= MOVE_READY;

	m_status &= ~SYNC_READY;
}

void axis::stop()
{
	gpio_set_level(MOTION_PIN, 0);
	reset_timers();
	m_status &= ~IN_MOTION;
}

void axis::linear_interpolation_2D()
{
	if(m_cur_pos == m_end_pos)
	{
		timer_pause(STEP_GROUP, STEP_TIMER);
		timer_pause(STEP_GROUP, SECONDS_TIMER);
		gpio_set_level(MOTION_PIN, 0);
		m_status &= ~IN_MOTION;
		m_status |= MOVE_READY;
		return;
	}

	int mask = 0;
	int y = (m_cur_pos.x != m_end_pos.x)? m_slope*m_cur_pos.x : m_end_pos.y;

	mask += (m_cur_pos.y > y)?	4 : 0;
	mask += (m_end_pos.x > 0)?	2 : 0;
	mask += (m_end_pos.y > 0)?	1 : 0;

	switch(mask)
	{
		case 0:		m_cur_pos.x--;	m_step_axis = X_AXIS;	m_status &= ~DIRECTION;		break;
		case 1:		m_cur_pos.y++;	m_step_axis = Y_AXIS;	m_status |= DIRECTION;		break;
		case 2:		m_cur_pos.x++;	m_step_axis = X_AXIS;	m_status |= DIRECTION;		break;
		case 3:		m_cur_pos.y++;	m_step_axis = Y_AXIS;	m_status |= DIRECTION;		break;
		case 4:		m_cur_pos.y--;	m_step_axis = Y_AXIS;	m_status &= ~DIRECTION;		break;
		case 5:		m_cur_pos.x--;	m_step_axis = X_AXIS;	m_status &= ~DIRECTION;		break;
		case 6:		m_cur_pos.y--;	m_step_axis = Y_AXIS;	m_status &= ~DIRECTION;		break;
		case 7:		m_cur_pos.x++;	m_step_axis = X_AXIS;	m_status |= DIRECTION;		break;
	}

	gpio_set_level(DIR_PIN, m_status & DIRECTION);
	ets_delay_us(8);
}

void axis::circular_interpolation_2D()
{
	if(m_cur_pos == m_end_pos && m_cur_step > 1)
	{
		timer_pause(STEP_GROUP, STEP_TIMER);
		timer_pause(STEP_GROUP, SECONDS_TIMER);
		gpio_set_level(MOTION_PIN, 0);
		m_status &= ~IN_MOTION;
		m_status |= MOVE_READY;
		return;
	}

	int fxy = (m_cur_pos.x*m_cur_pos.x) + (m_cur_pos.y*m_cur_pos.y) - (m_radius*m_radius);
	int dx = 2*m_cur_pos.x;
	int dy = 2*m_cur_pos.y;
	
	bool f = (fxy < 0)? 0 : 1;
	bool a = (dx < 0)? 0 : 1;
	bool b = (dy < 0)? 0 : 1;
	
	int mask = 0;
	if(f) mask += 8;
	if(a) mask += 4;
	if(b) mask += 2;
	if(m_cw) mask += 1;
	
	switch(mask) {
		case 0:		m_cur_pos.y--;	m_step_axis = Y_AXIS;	m_status &= ~DIRECTION;		break;
		case 1:		m_cur_pos.x--;	m_step_axis = X_AXIS;	m_status &= ~DIRECTION;		break;
		case 2:		m_cur_pos.x--;	m_step_axis = X_AXIS;	m_status &= ~DIRECTION;		break;
		case 3:		m_cur_pos.y++;	m_step_axis = Y_AXIS;	m_status |= DIRECTION;		break;
		case 4:		m_cur_pos.x++;	m_step_axis = X_AXIS;	m_status |= DIRECTION;		break;
		case 5:		m_cur_pos.y--;	m_step_axis = Y_AXIS;	m_status &= ~DIRECTION;		break;
		case 6:		m_cur_pos.y++;	m_step_axis = Y_AXIS;	m_status |= DIRECTION;		break;
		case 7:		m_cur_pos.x++;	m_step_axis = X_AXIS;	m_status |= DIRECTION;		break;
		case 8:		m_cur_pos.x++;	m_step_axis = X_AXIS;	m_status |= DIRECTION;		break;
		case 9:		m_cur_pos.y++;	m_step_axis = Y_AXIS;	m_status |= DIRECTION;		break;
		case 10:	m_cur_pos.y--;	m_step_axis = Y_AXIS;	m_status &= ~DIRECTION;		break;
		case 11:	m_cur_pos.x++;	m_step_axis = X_AXIS;	m_status |= DIRECTION;		break;
		case 12:	m_cur_pos.y++;	m_step_axis = Y_AXIS;	m_status |= DIRECTION;		break;
		case 13:	m_cur_pos.x--;	m_step_axis = X_AXIS;	m_status &= ~DIRECTION;		break;
		case 14:	m_cur_pos.x--;	m_step_axis = X_AXIS;	m_status &= ~DIRECTION;		break;
		case 15:	m_cur_pos.y--;	m_step_axis = Y_AXIS;	m_status &= ~DIRECTION;		break;
	}

	if(m_step_axis == m_axis)
		gpio_set_level(DIR_PIN, m_status & DIRECTION);
	ets_delay_us(8);
}

void axis::update_divider(timer_group_t TIMER_GROUP)
{

	timer_get_counter_time_sec(TIMER_GROUP, SECONDS_TIMER, &m_cur_time);

	if(m_cur_divider > DIVIDER_US && m_cur_step < m_acc_step)
	{
		m_cur_speed = m_min_speed + (m_accel * m_cur_time);
		m_cur_divider = (DIVIDER_US * m_end_speed) / m_cur_speed;
		m_cur_divider = std::max(m_cur_divider, DIVIDER_US);
		timer_set_divider(STEP_GROUP, STEP_TIMER, m_cur_divider);
		return;
	}

	if(m_cur_divider == DIVIDER_US && m_cur_step < m_acc_step)
	{
		// set decel condition now that we now how many steps it takes
		m_dec_step = (m_cur_step < (m_end_step >> 1))? m_end_step - m_cur_step : m_cur_step;

		m_acc_step = m_cur_step;
	}

	if(m_cur_step == m_dec_step)
	{
		timer_set_counter_value(TIMER_GROUP, SECONDS_TIMER, 0);
		return;
	}

	if(m_cur_step > m_dec_step)
	{
		m_cur_speed = m_end_speed - (m_accel * m_cur_time);
		m_cur_speed = std::max(m_cur_speed, m_min_speed);
		m_cur_speed = std::min(m_cur_speed, m_end_speed);
		m_cur_divider = (DIVIDER_US * m_end_speed) / m_cur_speed;
		timer_set_divider(STEP_GROUP, STEP_TIMER, m_cur_divider);
		return;
	}
}

void axis::step_timer_isr(void* arg)
{
//	timer_spinlock_take(STEP_GROUP);

	if(m_axis == m_step_axis)
	{
		gpio_set_level(PULSE_PIN, 1);
		ets_delay_us(8);	// stepper driver timing requirement
		gpio_set_level(PULSE_PIN, 0);

		m_step_position += (m_status & DIRECTION)? 1 : -1;

		if(++m_cur_step == m_end_step)
		{
			timer_pause(STEP_GROUP, STEP_TIMER);
			timer_pause(STEP_GROUP, SECONDS_TIMER);
			gpio_set_level(MOTION_PIN, 0);
			m_status &= ~IN_MOTION;
			m_status |= MOVE_READY;
		}
	}

	if(m_status & LINE_MODE)
		linear_interpolation_2D();
	else if(m_status & CURV_MODE)
		circular_interpolation_2D();

	update_divider(STEP_GROUP);
	timer_group_clr_intr_status_in_isr	(STEP_GROUP, STEP_TIMER);
	timer_group_enable_alarm_in_isr		(STEP_GROUP, STEP_TIMER);
//	timer_spinlock_give					(STEP_GROUP);
}

void axis::syncSem_release_isr(void* arg)
{
	xSemaphoreGiveFromISR(m_syncSem, NULL);
}