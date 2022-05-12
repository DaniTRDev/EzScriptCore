#include "../pch.h"
#include "EzBuffer.h"


namespace Ez
{
	EzBuffer::EzBuffer() : m_BufferSize(0), m_BufferPos(0), m_BufferStart(0)
	{
	}
	EzBuffer::EzBuffer(std::istream& Stream) : EzBuffer() /*call the original constructor to set std::size_t to 0*/
	{
		FromStream(Stream);
	}
	Ez::EzBuffer::~EzBuffer()
	{
		FreeBuffer();
	}

	void EzBuffer::FromStream(std::istream& Stream)
	{
		if (!Stream.good())
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::istream&")
			.SetExceptionInfo("Could not create buffer from std::istream because it is not valid!");

		auto OrigPos = Stream.tellg();

		Stream.seekg(0, Stream.beg);
		m_BufferStart = Stream.tellg();

		Stream.seekg(0, Stream.end);
		m_BufferSize = std::size_t(Stream.tellg()) - m_BufferStart;

		Stream.seekg(0, Stream.beg); /*put the stream pos at the begining*/

		auto Data = std::make_unique<std::uint8_t[]>(m_BufferSize);
		Stream.read((char*)Data.get(), m_BufferSize);

		AllocateBuffer(m_BufferSize);
		Write(std::move(Data), m_BufferSize);

		SetPos(m_BufferStart); /*reset our position as well*/
		Stream.seekg(OrigPos, Stream.beg); /*reset the stream pos to its original state*/

	}

	void EzBuffer::AllocateBuffer(std::size_t Size)
	{
		if (!Size)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::size_t")
			.SetExceptionInfo("Could not allocated space for buffer because the given size is not valid!");
		
		m_Buffer.reset(); /*just in case...*/

		m_Buffer = std::make_unique<std::uint8_t[]>(Size);
		m_BufferSize = Size;
	}
	void EzBuffer::Rellocate(std::size_t NewSize)
	{
		if (NewSize < m_BufferSize)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::size_t")
			.SetExceptionInfo("Could not rellocate buffer because the given size is not valid!");

		auto NewBuffer = std::make_unique<std::uint8_t[]>(NewSize);
		auto CurrentPos = GetPos();

		SetPos(m_BufferStart); /*set the position of the buffer to 0 to prevent undefined behaviour*/
		Read(NewBuffer, GetBufferSize());

		FreeBuffer(); /*release the current buffer since it's already copied in NewBuffer*/	 
		
		AllocateBuffer(NewSize); /*allocate a new buffer*/
		Write(std::move(NewBuffer), NewSize); /*write the data of the previous buffer to the new one*/
	}
	void EzBuffer::FreeBuffer()
	{
		if (!m_Buffer)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::size_t")
			.SetExceptionInfo("Could not free buffer because it was already free!");

		m_Buffer.release();
		m_BufferSize = 0;
		m_BufferPos = 0;
		m_BufferStart = 0;
	}

	void EzBuffer::Write(std::unique_ptr<std::uint8_t[]> Data, std::size_t DataSize)
	{
		if (!Data)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::unique_ptr<std::uint8_t[]>")
			.SetExceptionInfo("Could not write to buffer because the given data is not valid!");

		else if (!DataSize)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::size_t")
			.SetExceptionInfo("Could not write to buffer because the given data size is not valid!");

		else if (!m_Buffer)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("Could not write to buffer because it is not allocated!");

		else if ((DataSize > m_BufferSize) || (DataSize > m_BufferSize - m_BufferPos))
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("Could not write to buffer because the data size is greater than buffer size!");

		std::copy_n(Data.get(), DataSize, &m_Buffer.get()[m_BufferPos]);
		Data.release();

		m_BufferPos += DataSize;

	}
	void EzBuffer::Read(std::unique_ptr<std::uint8_t[]>& Output, std::size_t DataSize)
	{
		if (!Output)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::unique_ptr<std::uint8_t[]>")
			.SetExceptionInfo("Could not read from buffer because the out pointer is not valid!");

		else if (!DataSize)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::size_t")
			.SetExceptionInfo("Could not read from buffer because the given data size is not valid!");

		else if (!m_Buffer)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("Could not read from buffer because it is not allocated!");

		else if ((DataSize > m_BufferSize) || (DataSize > m_BufferSize - m_BufferPos))
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("Could not read from buffer because the data size is greater than buffer size!");

		std::copy_n(&m_Buffer.get()[m_BufferPos], DataSize, Output.get());

		m_BufferPos += DataSize;

	}

	void EzBuffer::Write(char* Data, std::size_t DataSize)
	{
		if (!Data)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("char*")
			.SetExceptionInfo("Could not write to buffer because the given data is not valid!");

		else if (!DataSize)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::size_t")
			.SetExceptionInfo("Could not write to buffer because the given data size is not valid!");

		else if (!m_Buffer)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("Could not write to buffer because it is not allocated!");

		else if ((DataSize > m_BufferSize) || (DataSize > m_BufferSize - m_BufferPos))
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("Could not write to buffer because the data size is greater than buffer size!");


		std::copy_n(Data, DataSize, (char*)&m_Buffer.get()[m_BufferPos]);
		m_BufferPos += DataSize;


	}
	void EzBuffer::Read(char* Output, std::size_t DataSize)
	{
		if (!Output)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("char*")
			.SetExceptionInfo("Could not read from buffer because the out pointer is not valid!");

		else if (!DataSize)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::size_t")
			.SetExceptionInfo("Could not read from buffer because the given data size is not valid!");

		else if (!m_Buffer)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("Could not read from buffer because it is not allocated!");

		else if ((DataSize > m_BufferSize) || (DataSize > m_BufferSize - m_BufferPos))
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("Could not read from buffer because the data size is greater than buffer size!");


		std::copy_n((char*)&m_Buffer.get()[m_BufferPos], DataSize, (char*)Output);
		m_BufferPos += DataSize;


	}

	void EzBuffer::AdvancePos(std::size_t N)
	{
		if ((N > m_BufferSize) || (N > m_BufferSize))
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("std::size_t")
			.SetExceptionInfo("Could not advance buffer pos because N is greater than buffer size!");

		m_BufferPos += N;


	}
	void EzBuffer::SetPos(std::size_t Pos)
	{
		if ((Pos > m_BufferSize))
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("std::size_t")
			.SetExceptionInfo("Could not advance buffer pos because new pos is greater than buffer size!");

		m_BufferPos = Pos;


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

