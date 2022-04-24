#pragma once

namespace Ez
{
	namespace rage
	{
		struct EzLocal
		{
			std::uint32_t m_Index;
			std::int64_t m_Value;
		};

		struct EzScriptParam
		{
			std::uint32_t m_Index;
			std::int64_t m_Value;
		};

		struct EzGlobal
		{
			std::uint32_t m_Index;
		};

		
	}
}