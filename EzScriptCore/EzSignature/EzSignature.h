#pragma once

namespace Ez
{

	struct EzSignatureInstr
	{
		std::uintptr_t m_Address;
		std::size_t m_Size; /*determines how many opcodes the instruction has*/
		bool m_HasWildCards; /*if this is true then the signature will only contain the first
							opcode which is the instruction id the rest will be filled with '?'*/
	};

	struct EzSignatureByte
	{
		std::uint8_t m_Byte;
		bool m_IsWildCard;
	};

	class EzSignature
	{
	public:

		void SetSignatureContext(std::uint8_t* OpCodes, std::size_t OpCodeNum,
			std::shared_ptr<EzInstruction>* Instructions, std::size_t NumInstrs, rage::RageResourceVersion Version);

		void CreateAtAddress(std::uintptr_t Address);
		bool ScanFromNewSig(std::string Signature);

		bool ScanAtAddress(std::uintptr_t Address);

		std::string GetSignature();
		void Signature2Bytes();

	private:

		void AddWhiteSpace();
		void AddWildCards(std::uint8_t Num = 1);

		void AddByte(std::uint8_t Byte);
		void AddBytes(std::uint8_t* Bytes, std::size_t NumBytes);
		void AddInstruction(const EzSignatureInstr& Instr);

		bool CheckForSigMatchAddr(std::string Sig);

	private:
		std::shared_ptr<EzInstruction>* m_Instructions;
		std::size_t m_NumInstructions;

		std::uint8_t* m_OpCodes;
		std::size_t m_NumOpCodes;
		rage::RageResourceVersion m_Version;

		std::uintptr_t m_SignatureAddress; /*the address where it was generated*/
		std::uintptr_t m_FoundAddress; /*the last address where it was found*/
	
	
		std::ostringstream m_Signature;
		std::vector<EzSignatureByte> m_SignatureBytes;
	};
}

