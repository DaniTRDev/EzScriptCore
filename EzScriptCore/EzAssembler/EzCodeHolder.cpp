#include "../pch.h"

namespace Ez
{
	EzCodeHolder::~EzCodeHolder()
	{
		m_CompiledCode.reset();
	}
}
