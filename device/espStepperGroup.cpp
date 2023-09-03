#include "espStepperGroup.h"

espStepperGroup::espStepperGroup(QWidget* parent) : QGroupBox(parent)
{

}

bool espStepperGroup::status(uint8_t mask, bool get)
{
	for(auto s : m_steppers)
	{
		if(get)
			s.second->receive();
		if(!(s.second->status() & mask))
			return false;
	}

	return true;
}
                              
void espStepperGroup::waitUntil(uint8_t mask, uint32_t delay)
{
	while(!status(mask))
		gpioDelay(delay);

	return;
}

bool espStepperGroup::addStepper(espStepperMotor* stepper)
{

	return m_steppers.insert(std::make_pair((AXIS)stepper->axis(), stepper)).second;
}

position_t<double> espStepperGroup::position()
{
	position_t<double> pos = {0,0,0};
	pos.x = m_steppers.at(X_AXIS)->mm_position();
	pos.y = m_steppers.at(Y_AXIS)->mm_position();
	return pos;
}

void espStepperGroup::updatePosition()
{
	position_t<double> position = {};

	for(auto s : m_steppers)
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
	for(auto s : m_steppers)
		s.second->stop();
}

void espStepperGroup::pause()
{
	for(auto s : m_steppers)
		s.second->pause();
}

void espStepperGroup::resume()
{
	for(auto s : m_steppers)
		s.second->resume();
}

void espStepperGroup::jog(AXIS axis, bool dir)
{
	if(m_steppers.count(axis))
		m_steppers.at(axis)->jog(dir);
}

void espStepperGroup::move(const move_msg_t& msg)
{
	std::cout << msg << '\n';

	for(auto s : m_steppers)
		s.second->send(msg);

	waitUntil(SYNC_READY);

	gpioWrite(SYNC_PIN, 1);
	gpioWrite(SYNC_PIN, 0);
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
						msg.end.x = val - m_steppers.at(X_AXIS)->mm_position();
						break;

					case 'Y': case 'y':
						msg.end.y = val - m_steppers.at(Y_AXIS)->mm_position();
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
			startTimer(m_timerPeriod);
			break;
		}

		case 101:
		{

		}

		default:
			break;
	}
}

void espStepperGroup::set_jog_mm(double mm)
{
	for(auto s : m_steppers)
		s.second->set_jog_mm(mm);
}

void espStepperGroup::timerEvent(QTimerEvent* event)
{
	updatePosition();

	//check status without sending new messages to esp motors
	if(status(MOVE_READY, false))
	{
		killTimer(event->timerId());
		//updatePosition();
		emit blockCompleted();
	}
}