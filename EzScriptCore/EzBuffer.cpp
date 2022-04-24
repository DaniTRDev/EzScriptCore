#include "pch.h"
#include "EzBuffer.h"


namespace Ez
{
	EzBuffer::EzBuffer() : m_BufferSize(0), m_BufferPos(0), m_BufferStart(0)
	{
	}
	EzBuffer::EzBuffer(std::istream& Stream) : EzBuffer() /*call the original constructor to set std::size_t to 0*/
	{
		if (auto Res = FromStream(Stream); Res != EzBufferStatus::NoError)
			throw std::runtime_error("EzBuffer::EzBuffer(std::istream&) -> Could not create buffer from stream!");
	}
	Ez::EzBuffer::~EzBuffer()
	{
		FreeBuffer();
	}

	EzBufferStatus EzBuffer::FromStream(std::istream& Stream)
	{
		if (!Stream.good())
			return EzBufferStatus::InvalidStream;

		auto OrigPos = Stream.tellg();

		Stream.seekg(0, Stream.beg);
		m_BufferStart = Stream.tellg();

		Stream.seekg(0, Stream.end);
		m_BufferSize = std::size_t(Stream.tellg()) - m_BufferStart;

		Stream.seekg(0, Stream.beg); /*put the stream pos at the begining*/

		auto Data = std::make_unique<std::uint8_t[]>(m_BufferSize);
		Stream.read((char*)Data.get(), m_BufferSize);

		if (auto Result = AllocateBuffer(m_BufferSize); Result != EzBufferStatus::NoError)
			return Result;

		if (auto Result = Write(std::move(Data), m_BufferSize); Result != EzBufferStatus::NoError)
			return Result;

		SetPos(m_BufferStart); /*reset our position as well*/
		Stream.seekg(OrigPos, Stream.beg); /*reset the stream pos to its original state*/

		return EzBufferStatus::NoError;
	}

	EzBufferStatus EzBuffer::AllocateBuffer(std::size_t Size)
	{
		if (!Size)
			return EzBufferStatus::InvalidAllocationSize;;
		
		m_Buffer = std::make_unique<std::uint8_t[]>(Size);
		m_BufferSize = Size;

		return EzBufferStatus::NoError;
	}
	EzBufferStatus EzBuffer::Rellocate(std::size_t NewSize)
	{
		if (NewSize < m_BufferSize)
			return EzBufferStatus::InvalidRelocationSize;

		auto NewBuffer = std::make_unique<std::uint8_t[]>(NewSize);
		auto CurrentPos = GetPos();

		if (auto Result = SetPos(m_BufferStart); Result != EzBufferStatus::NoError)
			return Result; /*set the position of the buffer to 0 to prevent undefined behaviour*/

		if (auto Result = Read(NewBuffer, GetBufferSize()); Result != EzBufferStatus::NoError)
			return Result;

		if (auto Result = FreeBuffer(); Result != EzBufferStatus::NoError)
			return Result; /*release the current buffer since it's already copied in NewBuffer*/

		if (auto Result = AllocateBuffer(NewSize); Result != EzBufferStatus::NoError)
			return Result; /*allocate a new buffer*/

		if (auto Result = Write(std::move(NewBuffer), NewSize); Result != EzBufferStatus::NoError)
			return Result;

		return EzBufferStatus::NoError;
	}
	EzBufferStatus EzBuffer::FreeBuffer()
	{
		if (!m_Buffer)
			return EzBufferStatus::NotAllocated;

		m_Buffer.release();
		m_BufferSize = 0;
		m_BufferPos = 0;
		m_BufferStart = 0;

		return EzBufferStatus::NoError;
	}

	EzBufferStatus EzBuffer::Write(std::unique_ptr<std::uint8_t[]> Data, std::size_t DataSize)
	{
		if (!Data)
			return EzBufferStatus::InvalidBuffer;

		else if (!DataSize)
			return EzBufferStatus::InvalidBufferSize;

		else if (!m_Buffer)
			return EzBufferStatus::NotAllocated;

		else if ((DataSize > m_BufferSize) || (DataSize > m_BufferSize - m_BufferPos))
			return EzBufferStatus::InputSizeGreaterThanBuffer;

		std::copy_n(Data.get(), DataSize, &m_Buffer.get()[m_BufferPos]);
		Data.release();

		m_BufferPos += DataSize;
		return EzBufferStatus::NoError;
	}
	EzBufferStatus EzBuffer::Read(std::unique_ptr<std::uint8_t[]>& Output, std::size_t DataSize)
	{
		if (!Output)
			return EzBufferStatus::InvalidBuffer;

		else if (!DataSize)
			return EzBufferStatus::InvalidBufferSize;

		else if (!m_Buffer)
			return EzBufferStatus::NotAllocated;

		else if ((DataSize > m_BufferSize) || (DataSize > m_BufferSize - m_BufferPos))
			return EzBufferStatus::InputSizeGreaterThanBuffer;

		std::copy_n(&m_Buffer.get()[m_BufferPos], DataSize, Output.get());

		m_BufferPos += DataSize;
		return EzBufferStatus::NoError;
	}

	EzBufferStatus EzBuffer::Write(char* Data, std::size_t DataSize)
	{
		if (!Data)
			return EzBufferStatus::InvalidBuffer;

		else if (!DataSize)
			return EzBufferStatus::InvalidBufferSize;

		else if (!m_Buffer)
			return EzBufferStatus::NotAllocated;

		else if ((DataSize > m_BufferSize) || (DataSize > m_BufferSize - m_BufferPos))
			return EzBufferStatus::InputSizeGreaterThanBuffer;

		std::copy_n(Data, DataSize, (char*)&m_Buffer.get()[m_BufferPos]);
		m_BufferPos += DataSize;

		return EzBufferStatus::NoError;
	}
	EzBufferStatus EzBuffer::Read(char* Output, std::size_t DataSize)
	{
		if (!Output)
			return EzBufferStatus::InvalidBuffer;

		else if (!DataSize)
			return EzBufferStatus::InvalidBufferSize;

		else if (!m_Buffer)
			return EzBufferStatus::NotAllocated;

		else if ((DataSize > m_BufferSize) || (DataSize > m_BufferSize - m_BufferPos))
			return EzBufferStatus::InputSizeGreaterThanBuffer;

		std::copy_n((char*)&m_Buffer.get()[m_BufferPos], DataSize, (char*)Output);
		m_BufferPos += DataSize;

		return EzBufferStatus::NoError;
	}

	EzBufferStatus EzBuffer::AdvancePos(std::size_t N)
	{
		if ((N > m_BufferSize) || (N > m_BufferSize))
			return EzBufferStatus::InputSizeGreaterThanBuffer;

		m_BufferPos += N;

		return EzBufferStatus::NoError;
	}
	EzBufferStatus EzBuffer::SetPos(std::size_t Pos)
	{
		if ((Pos > m_BufferSize))
			return EzBufferStatus::InputSizeGreaterThanBuffer;

		m_BufferPos = Pos;

		return EzBufferStatus::NoError;
	}
	std::size_t EzBuffer::GetPos()
	{
		return m_BufferPos;
	}

	std::size_t EzBuffer::GetBufferStart()
	{
		return m_BufferStart;
	}
	std::size_t EzBuffer::GetBufferSize()
	{
		return m_BufferSize;
	}
	std::uint8_t* EzBuffer::GetData()
	{
		return m_Buffer.get();
	}
}

