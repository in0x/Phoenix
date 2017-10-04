#pragma once

#include <stdint.h>

namespace Phoenix
{
	class Texture
	{
	public:
		Texture();

		~Texture();

		void load(const char* path);

		void clear();

		uint8_t* m_data;
		
		uint32_t m_height;
		
		uint32_t m_width;
		
		uint8_t m_components;
	};

	void flipTextureHorizontal(Texture& texture);
}