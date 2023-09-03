#ifndef SPI_MSG_H
#define SPI_MSG_H

#include <vector>
#include <mutex>
#include <cstring>
#include <linux/spi/spidev.h>
#include <iostream>

class spiMsg
{

public:

	spiMsg(uint32_t length = 128);
	~spiMsg() {}

	void resize(uint32_t length);

	void set_speed_hz(uint32_t hz)		{m_transfer.speed_hz	= hz;}
	void set_delay_us(uint32_t usecs)	{m_transfer.delay_usecs	= usecs;}
	void clear_send_buf()				{std::memset(m_sendbuf.data(), 0, m_transfer.len);}
	void clear_recv_buf()				{std::memset(m_recvbuf.data(), 0, m_transfer.len);}

	uint32_t				length()	const	{return m_transfer.len;}
	spi_ioc_transfer*		tr_ptr()			{return &m_transfer;}
	std::mutex&				lock()				{return m_lock;}
	std::vector<uint8_t>&	sendbuf()			{return m_sendbuf;}
	std::vector<uint8_t>&	recvbuf()			{return m_recvbuf;}

protected:

	std::vector<uint8_t>	m_sendbuf;
	std::vector<uint8_t>	m_recvbuf;
	std::mutex				m_lock;

	spi_ioc_transfer		m_transfer;
};

#endif