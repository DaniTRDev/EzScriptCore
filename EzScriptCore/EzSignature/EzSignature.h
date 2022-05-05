#pragma once

namespace Ez
{
	enum class EzSignatureStatus
	{
		ContextNotSet,
		ContextAlreadySet,
		InvalidInstructionArray, 
		InvalidInstructionCount,
		InvalidOpCodes,
		InvalidOpCodeCount,
		CouldNotCreateSignatureForAddress,
		NoError
	};

	class EzSignature
	{
	public:

		EzSignatureStatus SetSignatureContext(std::uint8_t* OpCodes, std::size_t OpCodeNum,
			EzInstruction** Instructions, std::size_t NumInstrs, rage::RageResourceVersion Version);

		EzSignatureStatus RetrieveFromAddress(const std::uintptr_t& Address);
		EzSignatureStatus ScanAtAddress(const std::uintptr_t& Address);

	private:
		EzInstruction** m_Instructions;
		std::size_t m_NumInstructions;

		std::uint8_t* m_OpCodes;
		std::size_t m_NumOpCodes;
		rage::RageResourceVersion m_Version;

		std::string m_Signature;
	};
}

