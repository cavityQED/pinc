#ifndef AXIS_H
#define AXIS_H

#include <cstring>
#include <cstdint>
#include <iostream>
#include <vector>

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

#define PULSE_PIN	(gpio_num_t) 17
#define DIR_PIN		(gpio_num_t) 16
#define EN_PIN		(gpio_num_t) 4
#define SYNC_PIN	(gpio_num_t) 14
#define MOTION_PIN	(gpio_num_t) 22

#define STEP_GROUP		TIMER_GROUP_0
#define STEP_TIMER		TIMER_0
#define SECONDS_TIMER	TIMER_1

#define DIVIDER_US		(uint32_t)TIMER_BASE_CLK / 1000000

class axis {

public:

	/*	Constructor	*/
	axis() {}

	/*	Destructor	*/
	~axis() {}
	
	void setup_gpio();
	void setup_timers();
	void reset_timers();
	void pause_timers(bool pause);
	void reset();

	void limit_switch_hit() {std::cout << "Limit Switch Hit\n";}

	/*	Check error function; prints the input and then the error	*/
	void check_error(const char* msg);
		
	/*	Printers	*/
	void print_position() const
	{
		std::cout << "\tPosition (steps):\t\t" << m_step_position << '\n';
	}

	void print_info() const
	{
		std::cout << "\nSTEPPER MOTOR STATE:\n";
		std::cout << "--------------------\n";
		std::cout << "\tAxis:\t\t\t"				<< (int)m_axis				<< '\n';
		std::cout << "\tPosition (steps):\t"		<< m_step_position			<< '\n';
		std::cout << "\tMotion:\t\t\t"				<< (m_status & IN_MOTION)	<< '\n';
		std::cout << "\tDirection:\t\t"				<< (m_status & DIRECTION)	<< '\n';
		std::cout << "\tMove Position:\t\t"			<< m_cur_pos				<< '\n';
		std::cout << "\tCurrent Step:\t\t"			<< m_cur_step				<< '\n';
		std::cout << "\tCurrent Divider:\t"			<< m_cur_divider			<< '\n';
		std::cout << "\tStart Speed (steps/s):\t"	<< m_min_speed				<< '\n';
		std::cout << "\tRapid Speed (steps/s):\t"	<< m_max_speed				<< '\n';
		std::cout << "\tJog Speed (steps/s):\t"		<< m_jog_speed				<< '\n';
		std::cout << "\tAcceleration:\t\t"			<< m_accel					<< '\n';
		std::cout << "\n";
	}

public:

	/*	Setters	*/
	void set_axis		(uint32_t a)		{m_axis = a;}
	void set_mode		(uint8_t mode)		{m_status &= MODE_RESET; m_status |= mode;}
	void set_spmm		(uint32_t spmm)		{m_spmm = spmm; std::cout << "spmm set:\t" << m_spmm << '\n';}
	
	void set_jog_speed	(uint32_t sps)		{m_jog_speed = sps;}
	void set_min_speed	(uint32_t sps)		{m_min_speed = sps;}
	void set_max_speed	(uint32_t sps)		{m_max_speed = sps;}

	void set_jog_speed	(double mmps)		{m_jog_speed = mmps * m_spmm;}
	void set_min_speed	(double mmps)		{m_min_speed = mmps * m_spmm;}
	void set_max_speed	(double mmps)		{m_max_speed = mmps * m_spmm;}

	void set_accel		(uint32_t accel)	{m_accel = accel;}

	void set_msg_queue	(QueueHandle_t* h)	{m_msgQueue = h;}

public:

	/*	Getters	*/
	uint8_t		status()		const {return m_status;}
	int			step_position() const {return m_step_position;}
	uint32_t	cur_step()		const {return m_cur_step;}
	uint32_t	dec_step()		const {return m_dec_step;}
	uint32_t	ticks()			const {return m_ticks;}
	uint32_t	divider()		const {return m_cur_divider;}
	double		cur_time()		const {return m_cur_time;}

	bool 		in_motion() 	const {return m_status & IN_MOTION;}
	bool		direction()		const {return m_status & DIRECTION;}
	bool		sync_ready()	const {return m_status & SYNC_READY;}

public:

	/*	Movers	*/

	void move(	const position_t<double>&	start,
				const position_t<double>&	end,
				double						final_speed,
				double						r,
				bool						cw,
				bool						dir);

	void stop();

public:

	static void IRAM_ATTR linear_interpolation_2D	();
	static void IRAM_ATTR circular_interpolation_2D	();
	static void IRAM_ATTR update_divider			(timer_group_t TIMER_GROUP);
	static void IRAM_ATTR step_timer_isr			(void* arg);
	static void IRAM_ATTR syncSem_release_isr		(void* arg);

protected:

	static step_vec_t		m_cur_pos;
	static step_vec_t		m_end_pos;
	
	static bool				m_homed;
	static uint8_t			m_status;
	static uint8_t			m_axis;
	static uint8_t			m_step_axis;
	static uint32_t			m_spmm;
	static uint32_t			m_ticks;

	static uint32_t			m_cur_speed;
	static uint32_t			m_end_speed;
	static uint32_t			m_jog_speed;
	static uint32_t			m_min_speed;
	static uint32_t			m_max_speed;

	static uint32_t			m_cur_divider;
	static uint32_t			m_max_divider;

	static uint32_t			m_cur_step;
	static uint32_t			m_acc_step;
	static uint32_t			m_dec_step;
	static uint32_t			m_end_step;

	static uint32_t			m_accel;
	static uint32_t			m_radius;
	static uint32_t			m_cw;
	static double			m_slope;

	static int				m_step_position;
	static double			m_cur_time;

	static esp_err_t		m_esp_err;
	static xQueueHandle 	m_syncSem;			//Sync semaphore; used to signal start of sync move
	static QueueHandle_t*	m_msgQueue;
};

#endif	
