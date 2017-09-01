#include "UniformCache.hpp"

namespace Phoenix
{
	struct LocationCache
	{
		std::map<std::string, uint32_t> m_locations;
	};

	class UniformCache
	{
		UniformCache(IRenderContext* rc) : m_pRenderContext(rc) {}

		std::map<ProgramHandle, LocationCache> m_caches;

		uint32_t getUniformLocation(ProgramHandle program, UniformHandle uniform)
		{
			const uint32_t* cachedLocation = getUniformLocationIfCached(program, uniform);

			if (cachedLocation)
			{
				return *cachedLocation;
			}

			// Otherwise try to find location.
		}

		const uint32_t* getUniformLocationIfCached(ProgramHandle program, UniformHandle uniform)
		{
			const std::string& name = mockGetUniformName(uniform);
			const LocationCache& cache = m_caches.at(program);
			return &cache.m_locations.at(name);
		}

	private:
		IRenderContext* m_pRenderContext;
	};
}