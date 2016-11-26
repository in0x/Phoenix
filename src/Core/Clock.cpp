#include "Clock.hpp"
#include <ctime>

namespace Phoenix
{
	void Clock::start()
	{
		m_bStarted = true;
		m_timeAtStart = f64(std::clock());
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

	f64 Clock::getElapsedTime()
	{
		if (!m_bStarted)
			return 0;

		return f64(std::clock()) - m_timeAtStart;
	}

	f64 Clock::getElapsedS()
	{
		return getElapsedTime() / CLOCKS_PER_SEC;
	}
	
	f64 Clock::getElapsedMS()
	{
		return (getElapsedTime() / CLOCKS_PER_SEC) * 1000;
	}
	
	bool Clock::started()
	{
		return m_bStarted;
	}
}