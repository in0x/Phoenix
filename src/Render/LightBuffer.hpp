#pragma once

#include <Math/Vec3.hpp>

namespace Phoenix
{
	struct alignas(16) GpuVec3
	{
		GpuVec3()
			: x(0.0f), y(0.0f), z(0.0f) {}

		GpuVec3(const Vec3& v)
			: x(v.x), y(v.y), z(v.z) {}

		float x;
		float y;
		float z;
	};

	class LightBuffer
	{
	public:
		enum 
		{
			MAX_DIR = 32
		};
		
		LightBuffer()
			: m_dir_numLights(0) {}

		void addDirectional(const Vec3& direction, const Vec3& color)
		{
			if (m_dir_numLights >= MAX_DIR)
			{
				return;
			}

			m_dir_directions[m_dir_numLights] = direction;
			m_dir_colors[m_dir_numLights] = color;			
			m_dir_numLights++;
		}

		void clear()
		{
			memset(m_dir_directions, 0, sizeof(GpuVec3) * MAX_DIR);
			memset(m_dir_colors, 0, sizeof(GpuVec3) * MAX_DIR);
			m_dir_numLights = 0;
		}

		GpuVec3 m_dir_directions[MAX_DIR];
		GpuVec3 m_dir_colors[MAX_DIR];
		uint32_t m_dir_numLights;
	};
}