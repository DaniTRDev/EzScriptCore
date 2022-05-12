#include "../pch.h"

namespace Ez
{
	bool EzAssembler::SetCodeHolder(std::unique_ptr<EzCodeHolder> Holder)
	{
		if (!Holder)
		{
			m_LastStatus = EzAssemblerStatus::InvalidCodeHolder;
			return false;
		}

		m_CodeHolder.reset(); /*just in case we already had something in m_CodeHolder*/
		m_CodeHolder = std::move(Holder);
		
		m_LastStatus = EzAssemblerStatus::NoError;
		return true;
	}

	bool EzAssembler::Compile()
	{
		m_LastStatus = EzAssemblerStatus::NoError;
		return true;
	}

	std::shared_ptr<EzAsmFunc> EzAssembler::CreateFunc(std::string_view Name,
		std::uint32_t ParamCount, std::uint32_t ReturnSize)
	{
		if (GetFuncByName(Name))
		{
			m_LastStatus = EzAssemblerStatus::FunctionAlreadyCreated;
			return nullptr;
		}

		auto Func = std::make_shared<EzAsmFunc>(Name, ParamCount, ReturnSize);

		m_LastStatus = EzAssemblerStatus::NoError;
		return Func;
	}
	std::shared_ptr<EzAsmFunc> EzAssembler::GetFuncByName(std::string_view Name)
	{
		for (auto& Func : m_Functions)
		{
			if (Func->GetName() == Name)
				return Func;
		}

		return nullptr;
	}
}
