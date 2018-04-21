#pragma once

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <Core/Logger.hpp>

namespace Phoenix
{
	// Represents a series of bytes which can be used to write or read data.
	// Used to write and read binary data to/from disk.
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

	// Represents a series of bytes which may be read from sequentially.
	struct ReadArchive : public Archive
	{
		size_t m_numBytesRead;

		virtual void serialize(void* data, size_t numBytes) override
		{
			if ( m_numBytesRead + numBytes > m_size)
			{
				assert(false);
				return;
			}

			memcpy(data, m_data + m_numBytesRead, numBytes);
			m_numBytesRead += numBytes;
		}
	};

	// Represents a series of bytes written into the archive.
	struct WriteArchive : public Archive
	{
		size_t m_numBytesWritten;

		virtual void serialize(void* data, size_t numBytes) override
		{
			reserve(numBytes);
			memcpy(m_data + m_numBytesWritten, data, numBytes);
			m_numBytesWritten += numBytes;
		}

		// Ensures numBytes are available in the archive's 
		// buffer. Grows the buffer if necessary.
		void reserve(size_t numBytes)
		{
			if (m_size - m_numBytesWritten >= numBytes)
			{
				return;
			}

			size_t newSize = (m_size + numBytes) * 2;
			uint8_t* newBuffer = new uint8_t[newSize];

			memset(newBuffer, 0, newSize);
			memcpy(newBuffer, m_data, m_numBytesWritten);

			delete[] m_data;
			m_data = newBuffer;
			m_size = newSize;
		}
	};


	enum class ArchiveError
	{
		NoError,
		Open,
		WriteToDisk,
		ReadFromDisk,
		ReadFromDiskEmpty,
		NumErrorTypes
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

	ArchiveError writeArchiveToDisk(const char* path, const WriteArchive& ar)
	{
		ArchiveError err = ArchiveError::NoError;

		FILE* file = fopen(path, "w");

		if (!file)
		{
			Logger::errorf("Failed to open archive \"%s\" for writing.", path);
			return ArchiveError::Open;
		}
		
		size_t bytesWrittenToDisk = fwrite(ar.m_data, 1, ar.m_numBytesWritten, file);
		
		if (bytesWrittenToDisk != ar.m_numBytesWritten)
		{
			Logger::errorf("Failed writing archive %s to disk, file is likely invalid.", path);
			err = ArchiveError::WriteToDisk;
		}

		fclose(file);

		return ArchiveError::NoError;
	}

	ArchiveError createReadArchive(const char* path, ReadArchive* outAr)
	{
		ArchiveError err = ArchiveError::NoError;

		FILE* file = fopen(path, "r");

		if (!file)
		{
			Logger::errorf("Failed to open archive \"%s\" for reading.", path);
			return ArchiveError::Open;
		}

		fseek(file, 0, SEEK_END);
		size_t length = ftell(file);

		if (length == 0)
		{
			Logger::errorf("Archive \"%s\" is empty.", path);
			fclose(file);
			return ArchiveError::ReadFromDiskEmpty;
		}
		
		outAr->m_data = new uint8_t[length];
		outAr->m_numBytesRead = 0;
		outAr->m_size = length;

		memset(outAr->m_data, 0, length);

		fseek(file, 0, SEEK_SET);
		size_t numBytesRead = fread(outAr->m_data, 1, length, file);
		
		if (numBytesRead != length)
		{
			Logger::errorf("Failed reading archive %s from disk, file is likely invalid.", path);
			err = ArchiveError::ReadFromDisk;
		}
		
		fclose(file);

		return err;
	}

	void destroyArchive(Archive& ar)
	{
		delete[] ar.m_data;
	}

	void serialize(Archive& ar, size_t& data)
	{
		ar.serialize(&data, sizeof(data));
	}

	struct SerialTest
	{
		enum TestEnum
		{
			Avalue,
			Bvalue
		};

		TestEnum m_enum;
		int32_t m_int32;
		bool m_bBool;
	};

	void serialize(Archive& ar, SerialTest& data)
	{
		ar.serialize(&data.m_enum, sizeof(SerialTest::TestEnum));
		ar.serialize(&data.m_int32, sizeof(int32_t));
		ar.serialize(&data.m_bBool, sizeof(bool));
	}

	namespace Tests
	{
		void runSerializeTests()
		{
			{
				ReadArchive ar;
				createReadArchive("thisDoesntExist", &ar);
			}

			{
				WriteArchive ar = createWriteArchive(0);
				
				writeArchiveToDisk("", ar);
				
				writeArchiveToDisk("SerialTest/anEmptyArchive.bin", ar);
			}

			{
				ReadArchive ar;
				createReadArchive("SerialTest/anEmptyArchive.bin", &ar);
			}
		
			{
				size_t w = 3;

				WriteArchive ar = createWriteArchive(sizeof(size_t));

				serialize(ar, w);
				writeArchiveToDisk("SerialTest/sizet.bin", ar);

				assert(ar.m_size == sizeof(size_t));

				destroyArchive(ar);
			}

			{
				size_t r = 0;

				ReadArchive ar;
				createReadArchive("SerialTest/sizet.bin", &ar);

				serialize(ar, r);
				destroyArchive(ar);

				assert(r == 3);
			}

			{
				SerialTest t;
				t.m_int32 = 196;
				t.m_bBool = false;
				t.m_enum = SerialTest::Bvalue;

				WriteArchive ar = createWriteArchive(sizeof(WriteArchive));
				
				serialize(ar, t);
				writeArchiveToDisk("SerialTest/serialtst.bin", ar);

				destroyArchive(ar);
			}

			{
				SerialTest t;

				ReadArchive ar;
				createReadArchive("SerialTest/serialtst.bin", &ar);

				serialize(ar, t);

				assert(t.m_int32 == 196);
				assert(t.m_bBool == false);
				assert(t.m_enum == SerialTest::Bvalue);
			}

			{

				WriteArchive ar = createWriteArchive(sizeof(size_t) / 4);

				size_t t = 1024;
				serialize(ar, t);
				
				t = 2048;
				serialize(ar, t);

				writeArchiveToDisk("SerialTest/growTest.bin", ar);

				destroyArchive(ar);
			}

			{
				size_t t;

				ReadArchive ar;
				createReadArchive("SerialTest/growTest.bin", &ar);

				serialize(ar, t);
				assert(t == 1024);
				
				serialize(ar, t);
				assert(t == 2048);
			}
		}
	}
}