#pragma once

#include <Math/Vec3.hpp>
#include <Math/Vec4.hpp>

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

	struct alignas(16) GpuVec4
	{
		GpuVec4()
			: x(0.0f), y(0.0f), z(0.0f) {}

		GpuVec4(const Vec3& v)
			: x(v.x), y(v.y), z(v.z) {}

		float x;
		float y;
		float z;
		float w;
	};

	class LightBuffer
	{
	public:
		enum 
		{
			MAX_DIR = 32,
			MAX_POINT = 64
		};

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

		void addPointLight(const Vec3& position, float radius, const Vec3& color, float intensity)
		{
			if (m_pl_numLights >= MAX_POINT)
			{
				return;
			}

			m_pl_positionRadius[m_pl_numLights] = position;
			m_pl_positionRadius[m_pl_numLights].w = radius;
			m_pl_colorsIntensity[m_pl_numLights] = color;
			m_pl_colorsIntensity[m_pl_numLights].w = intensity;
			m_pl_numLights++;
		}

		void clear()
		{
			memset(m_dir_directions, 0, sizeof(GpuVec3) * MAX_DIR);
			memset(m_dir_colors, 0, sizeof(GpuVec3) * MAX_DIR);

			memset(m_pl_positionRadius, 0, sizeof(GpuVec4) * MAX_POINT);
			memset(m_pl_colorsIntensity, 0, sizeof(GpuVec3) * MAX_POINT);

			m_dir_numLights = 0;
			m_pl_numLights = 0;
		}

		GpuVec3 m_dir_directions[MAX_DIR];
		GpuVec3 m_dir_colors[MAX_DIR];
		uint32_t m_dir_numLights;

		GpuVec4 m_pl_positionRadius[MAX_POINT];
		GpuVec4 m_pl_colorsIntensity[MAX_POINT];
		uint32_t m_pl_numLights;
	};
}