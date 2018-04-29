#pragma once

#include <stdint.h>

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
		virtual bool isReading() = 0;
		virtual bool isWriting() = 0;
	};

	// Represents a series of bytes which may be read from sequentially.
	struct ReadArchive : public Archive
	{
		size_t m_numBytesRead;

		// Reads numBytes from the archive.
		virtual void serialize(void* data, size_t numBytes) override;

		virtual bool isReading() { return true; };
		virtual bool isWriting() { return false; };
	};

	// Represents a series of bytes written into the archive.
	struct WriteArchive : public Archive
	{
		size_t m_numBytesWritten;

		// Writes numBytes from data to the buffer, growing the buffer
		// if neceassry.
		virtual void serialize(void* data, size_t numBytes) override;

		// Ensures numBytes are available in the archive's 
		// buffer. Grows the buffer if necessary.
		void reserve(size_t numBytes);

		virtual bool isReading() { return false; };
		virtual bool isWriting() { return true; };
	};

	enum class EArchiveError
	{
		NoError,
		Open,
		Write,
		Read,
		ReadEmpty,
		ReadEarlyEOF,
		NumErrorTypes
	};

	//WriteArchive createWriteArchive(size_t initialBytes);
	void createWriteArchive(size_t initialBytes, WriteArchive* outAr);

	EArchiveError writeArchiveToDisk(const char* path, const WriteArchive& ar);

	EArchiveError createReadArchive(const char* path, ReadArchive* outAr);

	void destroyArchive(Archive& ar);

	namespace Tests
	{
		void runSerializeTests();
	}
}