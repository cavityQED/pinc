#include "spiMsg.h"

spiMsg::spiMsg(uint32_t length)
{
	std::memset(&m_transfer, 0, sizeof(m_transfer));

	m_transfer.bits_per_word	= 8;
	m_transfer.speed_hz			= 1000000;
	m_transfer.delay_usecs		= 100;

	resize(length);
}

void spiMsg::resize(uint32_t length)
{
	m_sendbuf.resize(length);
	m_recvbuf.resize(length);

	m_transfer.len		= length;
	m_transfer.tx_buf	= (unsigned long)m_sendbuf.data();
	m_transfer.rx_buf	= (unsigned long)m_recvbuf.data();
}