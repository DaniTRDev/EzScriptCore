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
		CouldNotRetrieveSignatureForAddress,
		NoError
	};

	struct EzSignatureInstr
	{
		std::uintptr_t m_Address;
		std::size_t m_Size; /*determines how many opcodes the instruction has*/
		bool m_HasWildCards; /*if this is true then the signature will only contain the first
							opcode which is the instruction id the rest will be filled with '?'*/
	};

	class EzSignature
	{
	public:

		EzSignatureStatus SetSignatureContext(std::uint8_t* OpCodes, std::size_t OpCodeNum,
			std::shared_ptr<EzInstruction>* Instructions, std::size_t NumInstrs, rage::RageResourceVersion Version);

		EzSignatureStatus CreateAtAddress(std::uintptr_t Address);
		EzSignatureStatus ScanFromNewSig(std::string Signature);

		EzSignatureStatus ScanAtAddress(std::uintptr_t Address);

		std::string GetSignature();
		EzSignatureStatus Signature2Bytes();

	private:

		void AddWhiteSpace();
		void AddWildCard();

		void AddByte(std::uint8_t Byte);
		void AddInstruction(const EzSignatureInstr& Instr);

	private:
		std::shared_ptr<EzInstruction>* m_Instructions;
		std::size_t m_NumInstructions;

		std::uint8_t* m_OpCodes;
		std::size_t m_NumOpCodes;
		rage::RageResourceVersion m_Version;

		std::uintptr_t m_SignatureAddress; /*the address where it was generated*/
		std::uintptr_t m_FoundAddress; /*the last address where it was found*/
	
	
		std::ostringstream m_Signature;
		std::vector<std::uint8_t> m_SignatureBytes;
	};
}

