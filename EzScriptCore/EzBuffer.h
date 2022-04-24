#pragma once

namespace Ez
{
	enum class EzBufferStatus : std::uint8_t
	{
		NoError,
		InvalidStream,
		InvalidAllocationSize, /*returned when 0 is passed to AllocateBuffer*/
		InvalidRelocationSize, /*returned by Relocate when the passed size is lower than m_BufferSize*/
		NotAllocated, /*returned when AllocateBuffer was not called*/
		InvalidBuffer, /*returned by Relocate, Write or Read whenever the input/output buffer is not valid*/
		InvalidBufferSize, /*returned by Relocate, Write or Read whenever the input/ouput buffer size is not valid*/
		InputSizeGreaterThanBuffer, /*returned by Relocate, Write or Read whenever the DataSize is bigger than m_BufferSize*/
	};
	class EzBuffer
	{
	public:

		EzBuffer();
		/// <summary>
		///		<param name='Stream'>Initializes the buffer with the content inside Stream</param> 
		/// </summary>
		/// <returns>EzBufferStatus</returns>
		EzBuffer(std::istream& Stream);
		~EzBuffer();

		/// <summary>
		///		<param name='Stream'>Initializes the buffer with the content inside Stream</param> 
		/// </summary>
		/// <returns>EzBufferStatus</returns>
		EzBufferStatus FromStream(std::istream& Stream);

		/// <summary>
		///		<param name='Size'>The size of the allocated buffer</param> 
		/// </summary>
		/// <returns>EzBufferStatus</returns>
		EzBufferStatus AllocateBuffer(std::size_t Size);
		/// <summary>
		///		<param name='NewSize'>The size to growth the buffer</param>
		/// </summary> 
		/// <returns>EzBufferStatus</returns>
		EzBufferStatus Rellocate(std::size_t NewSize);
		/// <summary>
		/// </summary> Deallocates the current buffer
		/// <returns>EzBufferStatus</returns>
		EzBufferStatus FreeBuffer();

		/// <summary>
		///		<param name='Data'>The buffer that contains the data that is going to be written in this buffer</param>
		///		<param name='DataSize'>The size of Data buffer</param>
		///	Safe method for big chunks of memory
		/// </summary>
		/// <returns>EzBufferStatus</returns>
		EzBufferStatus Write(std::unique_ptr<std::uint8_t[]> Data, std::size_t DataSize);
		/// <summary>
		///		<param name='Output'>The buffer that will receive the data, must be ALLOCATED before using this</param> 
		///		<param name='DataSize'>The size of the Output buffer</param>
		/// </summary>
		/// Safe method for big chunks of memory
		/// <returns>EzBufferStatus</returns>
		EzBufferStatus Read(std::unique_ptr<std::uint8_t[]>& Output, std::size_t DataSize);

		/// <summary>
		///		<param name='Output'>The buffer that will receive the data, must be ALLOCATED before using this</param> 
		///		<param name='DataSize'>The size of the Output buffer</param>
		/// </summary>
		/// Safe method for small chunks of memory
		/// <returns>EzBufferStatus</returns>
		EzBufferStatus Write(char* Data, std::size_t DataSize);
		/// <summary>
		///		<param name='Output'>The buffer that will receive the data, must be ALLOCATED before using this</param> 
		///		<param name='DataSize'>The size of the Output buffer</param>
		/// </summary>
		/// Safe method for small chunks of memory
		/// <returns>EzBufferStatus</returns>
		EzBufferStatus Read(char* Output, std::size_t DataSize);

		/// <summary>
		///		<param name='N'>Increments m_BufferPos by N </param> 
		/// </summary>
		/// <returns>EzBufferStatus</returns>
		EzBufferStatus AdvancePos(std::size_t N);
		/// <summary>
		///		<param name='Pos'>Sets m_BufferPos</param> 
		/// </summary>
		/// <returns>EzBufferStatus</returns>
		EzBufferStatus SetPos(std::size_t Pos);
		/// <summary>
		/// </summary>
		/// <returns>Returns m_BufferPos</returns>
		std::size_t GetPos();

		/// <summary>
		/// </summary>
		/// <returns>Returns m_BufferStart</returns>
		std::size_t GetBufferStart();
		/// <summary>
		/// </summary>
		/// <returns>Returns m_BufferSize</returns>
		std::size_t GetBufferSize();

		/// <summary>
		/// </summary>
		/// <returns>Returns a ptr to the data without transfering ownership of m_buffer</returns>
		std::uint8_t* GetData();

	private:
		std::unique_ptr<std::uint8_t[]> m_Buffer;
		std::size_t m_BufferSize;

		std::size_t m_BufferPos;
		std::size_t m_BufferStart;
	};

}

