#pragma once

#include "PhiCoreRequired.hpp"
#include "PhiMath.hpp"

namespace Phoenix
{

	class Engine
	{
	public:
		struct Options
		{
			enum RenderAPI
			{
				OPENGL
			};

			RenderAPI Renderer;	


		};

		void init();
		void update();
		void exit();
	};
}