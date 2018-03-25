#pragma once

namespace Phoenix
{
	struct World;

	class ISystem
	{
	public:
		virtual ~ISystem() {}
		virtual void tick(World* world, float dt) = 0;
	};
}