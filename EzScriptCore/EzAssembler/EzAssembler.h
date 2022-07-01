#pragma once

namespace Ez
{
	enum class EzAssemblerStatus
	{
		InvalidCodeHolder,
		FunctionAlreadyCreated,
		NoError
	};

	class EzAssembler
	{
	public:

		bool SetCodeHolder(std::unique_ptr<EzCodeHolder> Holder);
		bool Compile();

	public:

		EzPtr<EzAsmFunc> CreateFunc(std::string_view Name, std::uint32_t ParamCount,
			std::uint32_t ReturnSize);

		EzPtr<EzAsmFunc> GetFuncByName(std::string_view Name);

		EzPtr<EzAssemblerStateNode> CreateAsmNode(std::string_view Name);
		/*mostly used to build stubs*/

	private:

		std::vector<EzPtr<EzAssemblerStateNode>> m_AsmNodes;
		std::map<EzPtr<EzAssemblerStateNode>, EzPtr<EzAsmFunc>> m_Functions;

		EzAssemblerStatus m_LastStatus; /*used to know what was the last error when compiling*/

	private:

		std::unique_ptr<EzCodeHolder> m_CodeHolder;
	};
}

