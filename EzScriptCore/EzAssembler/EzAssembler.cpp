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
		for (auto Func : m_Functions)
			Func.second->CallHandler(Func.first);

		m_LastStatus = EzAssemblerStatus::NoError;
		return true;
	}

	EzPtr<EzAsmFunc> EzAssembler::CreateFunc(std::string_view Name,
		std::uint32_t ParamCount, std::uint32_t ReturnSize)
	{
		if (GetFuncByName(Name))
		{
			m_LastStatus = EzAssemblerStatus::FunctionAlreadyCreated;
			return nullptr;
		}

		auto FuncNode = std::make_shared<EzAssemblerStateNode>(Name);
		auto Func = std::make_shared<EzAsmFunc>(Name, ParamCount, ReturnSize);

		m_AsmNodes.push_back(FuncNode);
		m_Functions.insert( { FuncNode, Func } );

		m_LastStatus = EzAssemblerStatus::NoError;
		return Func;
	}
	EzPtr<EzAsmFunc> EzAssembler::GetFuncByName(std::string_view Name)
	{
		for (auto& [AssemblerNode, Func] : m_Functions)
		{
			if (Func->GetName() == Name)
				return Func;
		}

		return nullptr;
	}

	EzPtr<EzAssemblerStateNode> EzAssembler::CreateAsmNode(std::string_view Name)
	{
		for (auto& Nodes : m_AsmNodes)
		{
			if (Nodes->GetName() == Name)
				throw EzException()
				.SetExceptionClass(typeid(this).name())
				.SetExceptionFunc(__func__)
				.SetExceptionName("std::string_view")
				.SetExceptionInfo("Could not create a new AsmNode because this name was already used!");
		
		}

		return std::make_shared<EzAssemblerStateNode>(Name);
	}
}
