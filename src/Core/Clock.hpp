#pragma once

#include "PhiCoreRequired.hpp"

namespace Phoenix
{
	class Clock
	{
	public:
		void start();
		void stop();
		void restart();

		f64 getElapsedS();
		f64 getElapsedMS();

		bool started();

	private:
		f64 m_timeAtStart;
		int32 m_bStarted : 1;

		f64 getElapsedTime();
	};
}