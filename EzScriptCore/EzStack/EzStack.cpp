#include "../pch.h"

namespace Ez
{
	void EzStack::Push(std::int32_t Value)
	{
		m_Stack.push(Value);
	}
	std::int32_t EzStack::Pop(std::int32_t Value)
	{
		auto Val = m_Stack.top();
		m_Stack.pop();
		return Val;
	}
}
