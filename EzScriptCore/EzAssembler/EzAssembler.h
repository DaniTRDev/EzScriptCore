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

		std::shared_ptr<EzAsmFunc> CreateFunc(std::string_view Name, std::uint32_t ParamCount,
			std::uint32_t ReturnSize);

		std::shared_ptr<EzAsmFunc> GetFuncByName(std::string_view Name);

	private:

		std::vector<std::shared_ptr<EzAsmFunc>> m_Functions;
		EzAssemblerStatus m_LastStatus; /*used to know what was the last error when compiling*/

	private:

		std::unique_ptr<EzCodeHolder> m_CodeHolder;
	};
}

