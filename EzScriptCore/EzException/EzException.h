#pragma once

namespace Ez
{
	class EzException : public std::exception
	{
	public:

		EzException() = default;

		char const* what() const override;

		EzException SetExceptionClass(std::string_view ClassName);
		EzException SetExceptionFunc(std::string_view FuncName);
		EzException SetExceptionName(std::string_view ExceptionName);
		EzException SetExceptionInfo(std::string_view ExceptionInfo);

	private:
		EzException(EzException* This);

	private:

		std::string_view m_ExceptionClassName;
		std::string_view m_ExceptionFuncName;
		std::string_view m_ExceptionName;
		std::string_view m_ExceptionInfo;

	};
}

