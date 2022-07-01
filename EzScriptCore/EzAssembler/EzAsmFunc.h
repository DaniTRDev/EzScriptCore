#pragma once

namespace Ez
{
	using AsmFuncHandler = void(EzPtr<EzAssemblerStateNode> Node);

	class EzAsmFunc
	{
	public:

		friend class EzAssembler;

		EzAsmFunc(std::string_view Name, std::uint32_t ParamCount, std::uint32_t ReturnSize);

		void SetHandler(AsmFuncHandler* Handler);
		AsmFuncHandler* GetHandler();

		std::string_view GetName();
		std::uint32_t GetParamCount();
		std::uint32_t GetReturnSize();
		std::uint32_t GetVarCount();

	private:

		void CallHandler(EzPtr<EzAssemblerStateNode> Node);

	private:

		std::string_view m_Name;
		std::uint32_t m_ParamCount;
		std::uint32_t m_ReturnSize;
		std::uint32_t m_VarCount;

		AsmFuncHandler* m_Handler; /*this will be used to add instructions to the func*/
	};
}

