#pragma once

namespace Phoenix
{
	class Clock
	{
	public:
		void start();
		void stop();
		void restart();

		double getElapsedS() const;
		double getElapsedMS() const;

		bool started() const;

	private:
		double m_timeAtStart;
		bool m_bStarted;

		double getElapsedTime() const;
	};
}