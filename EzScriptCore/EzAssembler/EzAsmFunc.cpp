#include "../pch.h"

namespace Ez
{
	EzAsmFunc::EzAsmFunc(std::string_view Name, std::uint32_t ParamCount, std::uint32_t ReturnSize) : 
		m_Name(Name), m_ParamCount(ParamCount), m_ReturnSize(ReturnSize), m_VarCount(0)
	{
	}

	void EzAsmFunc::SetHandler(AsmFuncHandler* Handler)
	{
		m_Handler = Handler;
	}

	AsmFuncHandler* EzAsmFunc::GetHandler()
	{
		return nullptr;
	}

	std::string_view EzAsmFunc::GetName()
	{
		return m_Name;
	}
	std::uint32_t EzAsmFunc::GetParamCount()
	{
		return m_ParamCount;
	}
	std::uint32_t EzAsmFunc::GetReturnSize()
	{
		return m_ReturnSize;
	}
	std::uint32_t EzAsmFunc::GetVarCount()
	{
		return m_VarCount;
	}
}
