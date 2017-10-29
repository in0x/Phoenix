#include "Clock.hpp"
#include <ctime>

namespace Phoenix
{
	void Clock::start()
	{
		m_bStarted = true;
		m_timeAtStart = static_cast<double>(std::clock());
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

	double Clock::getElapsedTime() const
	{
		if (!m_bStarted)
			return 0;

		return static_cast<double>(std::clock()) - m_timeAtStart;
	}

	double Clock::getElapsedS() const
	{
		return getElapsedTime() / CLOCKS_PER_SEC;
	}
	
	double Clock::getElapsedMS() const
	{
		return (getElapsedTime() / CLOCKS_PER_SEC) * 1000;
	}
	
	bool Clock::started() const
	{
		return m_bStarted;
	}
}