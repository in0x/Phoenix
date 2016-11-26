#pragma once

namespace Phoenix
{
	class Clock
	{
	public:
		void start();
		void stop();
		void restart();

		double getElapsedS();
		double getElapsedMS();

		bool started();

	private:
		double m_timeAtStart;
		int m_bStarted : 1;

		double getElapsedTime();
	};
}