#include "../pch.h"

namespace Ez
{
	EzException::EzException(EzException* This) : m_ExceptionClassName(This->m_ExceptionClassName),
		m_ExceptionFuncName(This->m_ExceptionFuncName), m_ExceptionName(This->m_ExceptionName),
		m_ExceptionInfo(This->m_ExceptionInfo)
	{
	}
	char const* EzException::what() const
	{
		return std::string("")
			.append(m_ExceptionClassName)
			.append(" : ")
			.append(m_ExceptionFuncName)
			.append(" -> ")
			.append(m_ExceptionName)
			.append(" Exception info: ")
			.append(m_ExceptionInfo)
			.c_str();
	}

	EzException EzException::SetExceptionClass(std::string_view ClassName)
	{
		m_ExceptionClassName = ClassName;
		return EzException(this);
	}
	EzException EzException::SetExceptionFunc(std::string_view FuncName)
	{
		m_ExceptionFuncName = FuncName;
		return EzException(this);
	}
	EzException EzException::SetExceptionName(std::string_view ExceptionName)
	{
		m_ExceptionName = ExceptionName;
		return EzException(this);
	}
	EzException EzException::SetExceptionInfo(std::string_view ExceptionInfo)
	{
		m_ExceptionInfo = ExceptionInfo;
		return EzException(this);
	}
}
