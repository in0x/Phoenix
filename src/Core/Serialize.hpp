#pragma once

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <Core/Logger.hpp>

namespace Phoenix
{
	struct Archive
	{
		Archive()
			: m_data(nullptr)
			, m_size(0)
		{}

		uint8_t* m_data;
		size_t m_size;
	
		virtual void serialize(void* data, size_t numBytes) = 0;
	};

	struct ReadArchive : public Archive
	{
		size_t m_numBytesRead;

		virtual void serialize(void* data, size_t numBytes) override
		{
			if (m_numBytesRead + numBytes > m_size)
			{
				assert(false);
				return;
			}

			memcpy(data, m_data + m_numBytesRead, numBytes);
			m_numBytesRead += numBytes;
		}
	};

	struct WriteArchive : public Archive
	{
		size_t m_numBytesWritten;

		virtual void serialize(void* data, size_t numBytes) override
		{
			if (m_size - m_numBytesWritten < numBytes)
			{
				size_t newSize = m_size * 2;
				uint8_t* newBuffer = new uint8_t[newSize];
				
				memset(newBuffer, 0, newSize);
				memcpy(newBuffer, m_data, m_numBytesWritten);

				delete[] m_data;
				m_data = newBuffer;
			}

			memcpy(m_data + m_numBytesWritten, data, numBytes);
			m_numBytesWritten += numBytes;
		}
	};

	WriteArchive createWriteArchive(size_t initialBytes)
	{
		WriteArchive ar;
		
		ar.m_numBytesWritten = 0;
		ar.m_data = new uint8_t[initialBytes];
		ar.m_size = initialBytes;

		memset(ar.m_data, 0, initialBytes);
		
		return ar;
	}

	void writeArchiveToDisk(const char* path, WriteArchive& ar)
	{
		FILE* file = fopen(path, "w");

		if (!file)
		{
			Logger::errorf("Failed to open file for WriteArchive %s", path);
			return;
		}
		
		fwrite(ar.m_data, 1, ar.m_numBytesWritten, file);
		
		fclose(file);
	}

	ReadArchive createReadArchive(const char* path)
	{
		FILE* file = fopen(path, "r");

		ReadArchive ar;

		if (!file)
		{
			Logger::errorf("Failed to open file for ReadArchive %s", path);
			return ar;
		}

		fseek(file, 0, SEEK_END);
		size_t length = ftell(file);
		
		ar.m_data = new uint8_t[length];
		ar.m_numBytesRead = 0;
		ar.m_size = length;

		memset(ar.m_data, 0, length);

		fseek(file, 0, SEEK_SET);
		fread(ar.m_data, 1, length, file);
		fclose(file);

		return ar;
	}

	void destroyArchive(Archive& ar)
	{
		delete ar.m_data;
	}

	Archive& serialize(Archive& ar, size_t& data)
	{
		ar.serialize(&data, sizeof(data));
		return ar;
	}
}