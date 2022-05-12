#pragma once

namespace Ez
{
	class EzBuffer
	{
	public:

		EzBuffer();
		/// <summary>
		///		<param name='Stream'>Initializes the buffer with the content inside Stream</param> 
		/// </summary>
		EzBuffer(std::istream& Stream);
		~EzBuffer();

		/// <summary>
		///		<param name='Stream'>Initializes the buffer with the content inside Stream</param> 
		/// </summary>
		void FromStream(std::istream& Stream);

		/// <summary>
		///		<param name='Size'>The size of the allocated buffer</param> 
		/// </summary>
		void AllocateBuffer(std::size_t Size);
		
		/// <summary>
		///		<param name='NewSize'>The size to growth the buffer</param>
		/// </summary> 	
		void Rellocate(std::size_t NewSize);

		/// <summary>
		/// </summary> Deallocates the current buffer
		void FreeBuffer();

		/// <summary>
		///		<param name='Data'>The buffer that contains the data that is going to be written in this buffer</param>
		///		<param name='DataSize'>The size of Data buffer</param>
		///	Safe method for big chunks of memory
		/// </summary>		
		void Write(std::unique_ptr<std::uint8_t[]> Data, std::size_t DataSize);

		/// <summary>
		///		<param name='Output'>The buffer that will receive the data, must be ALLOCATED before using this</param> 
		///		<param name='DataSize'>The size of the Output buffer</param>
		/// </summary>
		/// Safe method for big chunks of memory	
		void Read(std::unique_ptr<std::uint8_t[]>& Output, std::size_t DataSize);

		/// <summary>
		///		<param name='Output'>The buffer that will receive the data, must be ALLOCATED before using this</param> 
		///		<param name='DataSize'>The size of the Output buffer</param>
		/// </summary>
		/// Safe method for small chunks of memory	
		void Write(char* Data, std::size_t DataSize);

		/// <summary>
		///		<param name='Output'>The buffer that will receive the data, must be ALLOCATED before using this</param> 
		///		<param name='DataSize'>The size of the Output buffer</param>
		/// </summary>
		/// Safe method for small chunks of memory
		void Read(char* Output, std::size_t DataSize);

		/// <summary>
		///		<param name='N'>Increments m_BufferPos by N </param> 
		/// </summary>
		void AdvancePos(std::size_t N);
		/// <summary>
		///		<param name='Pos'>Sets m_BufferPos</param> 
		/// </summary>
		
		void SetPos(std::size_t Pos);

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

