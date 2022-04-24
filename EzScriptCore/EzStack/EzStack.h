#pragma once

namespace Ez
{
	class EzStack
	{
	public:

		void Push(std::int32_t Value);
		std::int32_t Pop(std::int32_t Value);

		std::int32_t GetValAtIndex(std::size_t Index);

	private:

		std::stack<std::int32_t> m_Stack;

	};
}

