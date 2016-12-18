#include "Clock.hpp"

namespace Phoenix
{
	void Clock::start()
	{
		m_bStarted = true;
		m_timeAtStart = double(std::clock());
	}
	
	void Clock::stop()
	{
		m_bStarted = false;
	}

	void Clock::restart()
	{
		m_bStarted = false;
		start();
	}

	double Clock::getElapsedTime()
	{
		if (!m_bStarted)
			return 0;

		return double(std::clock()) - m_timeAtStart;
	}

	double Clock::getElapsedS()
	{
		return getElapsedTime() / CLOCKS_PER_SEC;
	}
	
	double Clock::getElapsedMS()
	{
		return (getElapsedTime() / CLOCKS_PER_SEC) * 1000;
	}
	
	bool Clock::started()
	{
		return m_bStarted;
	}
}