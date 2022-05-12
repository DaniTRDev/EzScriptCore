#pragma once

namespace Ez
{
	/*Entry function == "ENTRY"*/
	class EzCodeHolder
	{
	public:

		friend class EzAssembler;

		EzCodeHolder& operator=(const EzCodeHolder&) = delete;
		EzCodeHolder(const EzCodeHolder&) = delete;

		~EzCodeHolder();

	private:
		std::unique_ptr<EzBuffer> m_CompiledCode; /*this won't be nullptr only when we compiled code*/
	};
}

