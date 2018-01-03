#pragma once

namespace Phoenix
{
	class World;

	class ISystem
	{
	public:
		virtual ~ISystem() {}
		virtual void tick(World* world, float dt) = 0;
	};
}