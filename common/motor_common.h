#ifndef MOTOR_COMMON_H
#define MOTOR_COMMON_H

#include <mutex>
#include <vector>
#include <cstring>
#include <cmath>
#include <iostream>
#include <numeric>

#define IN_MOTION	0x01
#define DIRECTION	0x02
#define SYNC_READY	0x04
#define MOVE_READY	0x08	

#define JOG_MODE	0x10
#define LINE_MODE	0x20
#define CURV_MODE	0x40
#define SYNC_MODE	0x80
#define MODE_RESET	~(JOG_MODE | LINE_MODE | CURV_MODE | SYNC_MODE)

enum STEPPER_MOTOR_CMD : uint16_t
{	
	PRINT_INFO,

	SET_AXIS,				
	SET_MODE,
	SET_SPMM,

	SET_JOG_SPEED,			
	SET_MIN_SPEED,		
	SET_MAX_SPEED,		
	
	SET_ACCELERATION,		

	MOVE,			
	STOP,					
	PAUSE,					
	RECEIVE,				
};

enum AXIS : uint8_t
{
	X_AXIS = 1,
	Y_AXIS = 2,
	Z_AXIS = 3,
	A_AXIS = 4,
	C_AXIS = 5
};

enum CONTROL_MODE : uint8_t
{
	AUTO,
	JOG,
	HOME,
	EDIT,
	MDI,
	MANUAL
};

template <typename T>
struct pos_vec_t
{
	std::vector<T>	points;

	pos_vec_t(size_t count = 3)
	{
		points.resize(count);
		std::memset(points.data(), 0, points.size() * sizeof(T));
	}

	size_t size() const {return points.size();}

	T length()
	{	
		T len = 0;

		for(auto it = points.begin(); it != points.end(); it++)
			len += *it * (*it);

		return std::sqrt(len);						
	}

	const T&	operator[](size_t idx) const {return points[idx];}
	T& 			operator[](size_t idx) 		 {return points[idx];}

	friend bool operator==(const pos_vec_t& a, const pos_vec_t& b)
	{
		if(a.size() != b.size())
			return false;

		for(int i = 0; i < a.size(); i++)
		{
			if(a[i] != b[i])
				return false;
		}

		return true;
	}
};

template <typename T>
struct position_t
{
	T x = 0;
	T y = 0;
	T z = 0;

	T sum() const {return (x + y + z);}

	T axis(AXIS a) const
	{
		switch(a)
		{
			case X_AXIS:
				return x;
			case Y_AXIS:
				return y;
			case Z_AXIS:
				return z;
			default:
				return 0;
		}
	}

	T length() const
	{
		return static_cast<T>(std::sqrt(x*x + y*y + z*z));
	}


	static double slope(const position_t& v1, const position_t& v2)
	{
		double rise = (double)v2.y - (double)v1.y;
		double run = (double)v2.x - (double)v1.x;

		return run? rise/run : rise;
	}

	static T dot(const position_t<T>& v1, const position_t<T>& v2)
	{
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}

	static double theta_deg(const position_t<T>& v1, const position_t<T>& v2)
	{
		double cos_theta = (double)dot(v1, v2) / v1.length() / v2.length();
		return 180 * std::acos(cos_theta) / 3.1415;
	}

	friend bool operator==(const position_t& p1, const position_t& p2)
	{
/*		double x1 = p1.x;
		double x2 = p2.x;
		double y1 = p1.y;
		double y2 = p2.y;

		double dist = std::sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
*/
		return (p1.x == p2.x) && (p1.y == p2.y);
	}

	friend position_t<T> operator-(const position_t<T>& vl, const position_t<T>& vr)
	{
		position_t<T> diff;
		diff.x = vl.x - vr.x;
		diff.y = vl.y - vr.y;
		diff.z = vl.z - vr.z;
		return diff;
	}

	friend position_t<T> operator+(const position_t<T>& vl, const position_t<T>& vr)
	{
		position_t<T> sum;
		sum.x = vl.x + vr.x;
		sum.y = vl.y + vr.y;
		sum.z = vl.z + vr.z;
		return sum;
	}

	friend std::ostream& operator<<(std::ostream& out, const position_t<T>& vec)
	{
		out << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
		return out;
	}

	friend position_t operator*(T m, const position_t& in)
	{
		position_t vec;
		vec.x = in.x*m;
		vec.y = in.y*m;
		vec.z = in.z*m;

		return vec;
	}
	friend position_t operator*(const position_t& in, T m)	
	{
		return m*in;
	}
};

struct step_vec_t
{
	int x = 0;
	int y = 0;
	int z = 0;

	int sum() const {return (x + y + z);}

	int axis(AXIS a) const
	{
		switch(a)
		{
			case X_AXIS:
				return x;
			case Y_AXIS:
				return y;
			case Z_AXIS:
				return z;
			default:
				return 0;
		}
	}

	static double slope(const step_vec_t& v1, const step_vec_t& v2)
	{
		double rise = v2.y - v1.y;
		double run = v2.x - v1.x;

		return (run != 0)? rise/run : rise;
	}

	friend bool operator==(const step_vec_t& p1, const step_vec_t& p2)
	{		
		double x1 = p1.x;
		double x2 = p2.x;
		double y1 = p1.y;
		double y2 = p2.y;

		double dist = std::sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));

		return dist < 0.9;
		//return (p1.x == p2.x) && (p1.y == p2.y);
	}

	friend step_vec_t operator-(const step_vec_t& vl, const step_vec_t& vr)
	{
		step_vec_t diff;
		diff.x = vl.x - vr.x;
		diff.y = vl.y - vr.y;
		diff.z = vl.z - vr.z;
		return diff;
	}

	friend step_vec_t operator+(const step_vec_t& vl, const step_vec_t& vr)
	{
		step_vec_t sum;
		sum.x = vl.x + vr.x;
		sum.y = vl.y + vr.y;
		sum.z = vl.z + vr.z;
		return sum;
	}

	friend std::ostream& operator<<(std::ostream& out, const step_vec_t& vec)
	{
		out << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
		return out;
	}

	friend step_vec_t operator*(int m, const step_vec_t& in)
	{
		step_vec_t vec;
		vec.x = in.x*m;
		vec.y = in.y*m;
		vec.z = in.z*m;

		return vec;
	}
	friend step_vec_t operator*(const step_vec_t& in, int m)	
	{
		return m*in;
	}
};

struct move_msg_t
{
	uint16_t			func		= MOVE;
	uint8_t				mode		= SYNC_MODE;
	bool				dir			= 1;
	bool				cw			= 1;
	position_t<double>	start		= {};
	position_t<double>	end			= {};
	double				radius		= 0;
	double				vf_mmps		= 0;

	friend move_msg_t& operator<<(move_msg_t& msg, std::vector<uint8_t>& data)
	{
		if(data.size() >= 69)
		{
			std::memcpy(&msg.func,			&data[0],	2);
			std::memcpy(&msg.mode,			&data[2],	1);
			std::memcpy(&msg.dir,			&data[3],	1);
			std::memcpy(&msg.cw,			&data[4],	1);
			std::memcpy(&msg.start.x,		&data[5],	8);
			std::memcpy(&msg.start.y,		&data[13],	8);
			std::memcpy(&msg.start.z,		&data[21],	8);
			std::memcpy(&msg.end.x,			&data[29],	8);
			std::memcpy(&msg.end.y,			&data[37],	8);
			std::memcpy(&msg.end.z,			&data[45],	8);
			std::memcpy(&msg.radius,		&data[53],	8);
			std::memcpy(&msg.vf_mmps,		&data[61],	8);
		}

		return msg;
	}

	friend const move_msg_t& operator>>(const move_msg_t& msg, std::vector<uint8_t>& data)
	{
		if(data.size() < 69)
			return msg;

		std::memcpy(&data[0],	&msg.func,		2);
		std::memcpy(&data[2],	&msg.mode,		1);
		std::memcpy(&data[3],	&msg.dir,		1);
		std::memcpy(&data[4],	&msg.cw,		1);
		std::memcpy(&data[5],	&msg.start.x, 	8);
		std::memcpy(&data[13],	&msg.start.y, 	8);
		std::memcpy(&data[21],	&msg.start.z, 	8);
		std::memcpy(&data[29],	&msg.end.x,	 	8);
		std::memcpy(&data[37],	&msg.end.y,	 	8);
		std::memcpy(&data[45],	&msg.end.z,	 	8);
		std::memcpy(&data[53],	&msg.radius,	8);
		std::memcpy(&data[61],	&msg.vf_mmps,	8);

		return msg;
	}

	friend std::ostream& operator<<(std::ostream& out, const move_msg_t& msg)
	{
		out << "\tMove Message:\n";
		out << "\t\tMode:\t\t"		<< (int)msg.mode	<< '\n';
		out << "\t\tDir:\t\t"		<< msg.dir			<< '\n';
		out << "\t\tCW:\t\t"		<< msg.cw			<< '\n';
		out << "\t\tStart:\t\t"		<< msg.start		<< '\n';
		out << "\t\tEnd:\t\t"		<< msg.end			<< '\n';
		out << "\t\tRadius:\t\t"	<< msg.radius		<< '\n';
		out << "\t\tVf (mm/s):\t"	<< msg.vf_mmps		<< '\n';

		return out;
	}
};

struct stepper_msg_in_t
{
	uint16_t		func		= RECEIVE;
	uint32_t		set_data	= 0;
	step_vec_t		start		= {};
	step_vec_t		end			= {};
	uint32_t		vf_sps		= 0;
	bool			dir			= 0;
	bool			cw			= 0;
	int				radius		= 0;

	friend stepper_msg_in_t& operator<<(stepper_msg_in_t& msg, std::vector<uint8_t>& data)
	{
		if(data.size() >= 40)
		{
			std::memcpy(&msg.func,			&data[0],	2);
			std::memcpy(&msg.set_data,		&data[2],	4);
			std::memcpy(&msg.start.x,		&data[6],	4);
			std::memcpy(&msg.start.y,		&data[10],	4);
			std::memcpy(&msg.start.z,		&data[14],	4);
			std::memcpy(&msg.end.x,			&data[18],	4);
			std::memcpy(&msg.end.y,			&data[22],	4);
			std::memcpy(&msg.end.z,			&data[26],	4);
			std::memcpy(&msg.vf_sps,		&data[30],	4);
			std::memcpy(&msg.dir,			&data[34],	1);
			std::memcpy(&msg.cw,			&data[35],	1);
			std::memcpy(&msg.radius,		&data[36],	4);
		}

		return msg;
	}

	friend const stepper_msg_in_t& operator>>(const stepper_msg_in_t& msg, std::vector<uint8_t>& data)
	{
		if(data.size() < 40)
			return msg;

		std::memcpy(&data[0],	&msg.func,		2);
		std::memcpy(&data[2],	&msg.set_data,	4);
		std::memcpy(&data[6],	&msg.start.x, 	4);
		std::memcpy(&data[10],	&msg.start.y, 	4);
		std::memcpy(&data[14],	&msg.start.z, 	4);
		std::memcpy(&data[18],	&msg.end.x,	 	4);
		std::memcpy(&data[22],	&msg.end.y,	 	4);
		std::memcpy(&data[26],	&msg.end.z,	 	4);
		std::memcpy(&data[30],	&msg.vf_sps,	4);
		std::memcpy(&data[34],	&msg.dir,		1);
		std::memcpy(&data[35],	&msg.cw,		1);
		std::memcpy(&data[36],	&msg.radius,	4);

		return msg;
	}

	friend std::ostream& operator<<(std::ostream& out, const stepper_msg_in_t& msg)
	{
		out << "\n\tPi to Stepper Message\n";
		out << "\t\tFunction Code:\t\t"		<< msg.func			<< "\n";
		out << "\t\tSet Data:\t\t"			<< msg.set_data		<< "\n";
		out << "\t\tStart:\t\t\t"			<< msg.start		<< "\n";
		out << "\t\tEnd:\t\t\t"				<< msg.end			<< "\n";
		out << "\t\tFinal Speed (sps):\t"	<< msg.vf_sps		<< "\n";
		out << "\t\tDirection:\t\t"			<< msg.dir			<< "\n";
		out << "\t\tClockwise:\t\t"			<< msg.cw			<< "\n";
		out << "\t\tRadius:\t\t\t"			<< msg.radius		<< "\n\n";

		return out;
	}
};

struct stepper_msg_out_t
{
	uint8_t		status;
	int			step_position;
	uint32_t	cur_pulse;
	uint32_t	decel_pulse;
	uint32_t	ticks;
	uint32_t	divider;
	double		cur_time;

	friend stepper_msg_out_t& operator<<(stepper_msg_out_t& msg, const std::vector<uint8_t>& data)
	{
		if(data.size() >= 29)
		{
			std::memcpy(&msg.status,		&data[0],	1);
			std::memcpy(&msg.step_position,	&data[1],	4);
			std::memcpy(&msg.cur_pulse,		&data[5],	4);
			std::memcpy(&msg.decel_pulse,	&data[9],	4);
			std::memcpy(&msg.ticks,			&data[13],	4);
			std::memcpy(&msg.divider,		&data[17],	4);
			std::memcpy(&msg.cur_time,		&data[21],	8);
		}

		else
			std::cout << "Not enought fill data:\t" << data.size() << "/29\n";
		return msg;
	}

	friend const stepper_msg_out_t& operator>>(const stepper_msg_out_t& msg, std::vector<uint8_t>& data)
	{
		if(data.size() < 40)
		{
			std::cout << "buffer too small\n";
			return msg;
		}
	
		std::memset(&data[0], 0, 40);

		std::memcpy(&data[0],	&msg.status,		1);
		std::memcpy(&data[1],	&msg.step_position,	4);
		std::memcpy(&data[5],	&msg.cur_pulse,		4);
		std::memcpy(&data[9],	&msg.decel_pulse,	4);
		std::memcpy(&data[13],	&msg.ticks,			4);
		std::memcpy(&data[17],	&msg.divider,		4);
		std::memcpy(&data[21],	&msg.cur_time,		8);

		return msg;
	}

	friend std::ostream& operator<<(std::ostream& out, const stepper_msg_out_t& msg)
	{
		out << "\n\tStepper to Pi Message\n";
		printf("\t\tStatus:\t\t\t%#02x\n", msg.status);
		out << "\t\tStep Position:\t\t"		<< msg.step_position	<< "\n";
		out << "\t\tCurrent Pulse:\t\t"		<< msg.cur_pulse		<< "\n";
		out << "\t\tDecel Pulse:\t\t"		<< msg.decel_pulse		<< "\n";
		out << "\t\tTicks:\t\t\t"			<< msg.ticks			<< "\n";
		out << "\t\tDivider:\t\t"			<< msg.divider			<< "\n";
		out << "\t\tCurrent Time (s):\t"	<< msg.cur_time			<< "\n\n";

		return out;
	}
};

#endif