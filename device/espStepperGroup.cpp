#include "espStepperGroup.h"

espStepperGroup::espStepperGroup(QWidget* parent) : QGroupBox(parent)
{
	gpioSetMode(SYNC_PIN, PI_OUTPUT);
	gpioSetPullUpDown(SYNC_PIN, PI_PUD_DOWN);
	gpioWrite(SYNC_PIN, 0);
}

bool espStepperGroup::allStatus(uint8_t mask, bool get)
{

	for(auto s : m_stepper_map)
	{
		if(get)
			s.second->receive();
		if(!(s.second->status() & mask))
			return false;
	}

	return true;
}

bool espStepperGroup::anyStatus(uint8_t mask, bool get)
{
	bool result = false;

	for(auto s : m_stepper_map)
	{
		if(get)
			s.second->receive();
		result = result | (s.second->status() & mask);
	}

	return result;
}
                              
void espStepperGroup::waitUntil(uint8_t mask, uint32_t delay)
{
	while(!allStatus(mask))
		gpioDelay(delay);

	return;
}

bool espStepperGroup::addStepper(espStepperMotor* stepper)
{

	m_steppers.push_back(stepper);
	return m_stepper_map.insert(std::make_pair((AXIS)stepper->axis(), stepper)).second;
}

position_t<double> espStepperGroup::position()
{
	position_t<double> pos = {0,0,0};
	pos.x = m_stepper_map.at(X_AXIS)->mm_position();
	pos.y = m_stepper_map.at(Y_AXIS)->mm_position();
	return pos;
}

position_t<int> espStepperGroup::step_position()
{
	position_t<int> pos = {0,0,0};
	pos.x = m_stepper_map.at(X_AXIS)->step_position();
	pos.y = m_stepper_map.at(Y_AXIS)->step_position();
	return pos;
}

void espStepperGroup::updatePosition()
{
	position_t<double> position = {};

	for(auto s : m_stepper_map)
	{
		s.second->receive();
		switch(s.first)
		{
			case X_AXIS:
				position.x = s.second->mm_position();
				break;
			case Y_AXIS:
				position.y = s.second->mm_position();
				break;
			case Z_AXIS:
				position.z = s.second->mm_position();
				break;
			default:
				break;
		}
	}

	emit positionChange(position);
}

void espStepperGroup::stop()
{
	if(m_timerID)
	{
		killTimer(m_timerID);
		m_timerID = 0;
	}
	updatePosition();
	for(auto s : m_stepper_map)
		s.second->stop();
}

void espStepperGroup::pause()
{
	if(m_timerID)
	{
		killTimer(m_timerID);
		m_timerID = 0;
	}
	updatePosition();
	for(auto s : m_stepper_map)
		s.second->pause();
}

void espStepperGroup::resume()
{
	for(auto s : m_stepper_map)
		s.second->resume();
	waitUntil(SYNC_READY);
	gpioWrite(SYNC_PIN, 1);
	startTimer(m_timerID);
	gpioWrite(SYNC_PIN, 0);
}

void espStepperGroup::jog(AXIS axis, bool dir)
{
	if(m_stepper_map.count(axis))
		m_stepper_map.at(axis)->jog(dir);
}

void espStepperGroup::move(const move_msg_t& msg)
{
	std::cout << msg << '\n';

//	for(auto s : m_stepper_map)
//		s.second->send(msg);

	std::scoped_lock(m_steppers[0]->host()->lock());

	spiMsg* tmp = new spiMsg();
	msg >> tmp->sendbuf();
	m_steppers[0]->host()->groupSend(m_steppers, tmp);

	waitUntil(SYNC_READY);

	gpioWrite(SYNC_PIN, 1);
}

void espStepperGroup::runBlock(const gBlock& blk)
{
	move_msg_t msg;

	std::cout << "Stepper Group Run Block\n";

	switch((int)blk.num())
	{
		case 0:
		case 1:
		{
			msg.mode = SYNC_MODE | LINE_MODE;

			for(auto arg : blk.args())
			{
				double val = arg.second;
				
				switch(arg.first)
				{

					case 'X': case 'x':
						msg.end.x = val - m_stepper_map.at(X_AXIS)->mm_position();
						break;

					case 'Y': case 'y':
						msg.end.y = val - m_stepper_map.at(Y_AXIS)->mm_position();
						break;

					case 'U': case 'u':
						msg.end.x = val;
						break;

					case 'V': case 'v':
						msg.end.y = val;
						break;

					case 'F': case 'f':
						m_feedrate = val;

					default:
						break;
				}
			}

			msg.vf_mmps = m_feedrate;
			move(msg);
			startTimer(m_timerPeriod);
			break;
		}

		case 2:
		case 3:
		{
			msg.mode = SYNC_MODE | CURV_MODE;

			for(auto arg : blk.args())
			{
				double val = std::round(100*arg.second)/100;

				switch(arg.first)
				{
					case 'X': case 'x':
						msg.end.x = val;
						break;
					case 'Y': case 'y':
						msg.end.y = val;
						break;
					case 'U': case 'u':
						msg.end.x = val;
						break;
					case 'V': case 'v':
						msg.end.y = val;
						break;
					case 'I': case 'i':
						msg.start.x = -val;
						break;
					case 'J': case 'j':
						msg.start.y = -val;
						break;
					case 'R': case 'r':
						msg.radius = val;
						break;
					case 'F': case 'f':
						m_feedrate = val;
						break;
					default:
						break;
				}
			}

			std::cout << "\t\tAbs. Start:\t" << msg.start << '\n';
			std::cout << "\t\tAbs. End\t" << msg.end << '\n';

			position_t<double> d = msg.end - position();
			std::cout << "\t\tD:\t\t" << d << '\n';
			msg.end = msg.start + d;
			msg.cw = blk.num() == 2;
			msg.vf_mmps = m_feedrate;

			if(!msg.radius)
			{
				double r1 = std::round(100 * msg.start.length()) / 100;
				double r2 = std::round(100 * msg.end.length()) / 100;

				msg.radius = (r1 + r2) / 2;
			}

			double angle = position_t<double>::theta_deg(msg.start, msg.end);
			std::cout << "\t\tAngle:\t" << angle << '\n';
			
			if(angle < 0.1 && angle > 0.0001)
			{
				msg.mode = SYNC_MODE | LINE_MODE;

				msg.end = msg.end - msg.start;
				msg.start = {};
			}

			move(msg);
			m_timerID = startTimer(m_timerPeriod, Qt::PreciseTimer);
			break;
		}

		case 101:
		{

		}

		default:
			break;
	}

	gpioWrite(SYNC_PIN, 0);
}

void espStepperGroup::set_jog_mm(double mm)
{
	for(auto s : m_stepper_map)
		s.second->set_jog_mm(mm);
}

void espStepperGroup::timerEvent(QTimerEvent* event)
{
	updatePosition();

	//check status without sending new messages to esp motors
	if(allStatus(MOVE_READY, false))
	{
		killTimer(event->timerId());
		m_timerID = 0;
		emit blockCompleted();
	}
}