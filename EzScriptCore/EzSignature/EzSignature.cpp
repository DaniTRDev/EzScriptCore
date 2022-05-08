#include "../pch.h"

namespace Ez
{
	EzSignatureStatus EzSignature::SetSignatureContext(
		std::uint8_t* OpCodes, std::size_t OpCodeNum, std::shared_ptr<EzInstruction>* Instructions, 
		std::size_t NumInstrs, rage::RageResourceVersion Version)
	{
		if (m_Instructions)
			return EzSignatureStatus::ContextAlreadySet;

		else if (!OpCodes)
			return EzSignatureStatus::InvalidOpCodes;

		else if (!OpCodeNum)
			return EzSignatureStatus::InvalidOpCodeCount;

		else if (!Instructions)
			return EzSignatureStatus::InvalidInstructionArray;

		else if (!NumInstrs)
			return EzSignatureStatus::InvalidInstructionCount;

		m_OpCodes = OpCodes;
		m_NumOpCodes = OpCodeNum;
		m_Instructions = Instructions;
		m_NumInstructions = NumInstrs;
		m_Version = Version;
		m_SignatureAddress = 0;
		m_FoundAddress = 0;

		return EzSignatureStatus::NoError;
	}

	EzSignatureStatus EzSignature::CreateAtAddress(std::uintptr_t Address)
	{
		if (!m_Instructions)
			return EzSignatureStatus::ContextNotSet;

		m_SignatureAddress = Address;
		bool AreWeOnCorrectAddr = false;
		for (auto i = 0; i < m_NumInstructions; i++)
		{
			auto Instruction = m_Instructions[i];
			auto InstructionBytes = &m_OpCodes[Instruction->GetStartAddr()];
			EzSignatureInstr SigInstr{ Instruction->GetStartAddr(), Instruction->GetInstructionSize(), false};

			if (!AreWeOnCorrectAddr)
			{
				if ((Instruction->GetStartAddr() != Address))
					continue;

				AreWeOnCorrectAddr = true;
			}

			if (Instruction->IsCall() || Instruction->IsJump()
				|| Instruction->IsVarLoad() || Instruction->IsVarPush() || Instruction->IsVarStore()
				|| Instruction->IsNativeCall())
			{
				SigInstr.m_HasWildCards = true;
			}
			
			AddInstruction(SigInstr);

			auto IsSigValid = ScanFromNewSig(m_Signature.str()) == EzSignatureStatus::NoError;

			if (IsSigValid) /*sig had a match, lets check if it's the one we want*/
			{
				if (m_FoundAddress == m_SignatureAddress)
					return EzSignatureStatus::NoError;
			}
		}

		return EzSignatureStatus::CouldNotCreateSignatureForAddress;
	}
	EzSignatureStatus EzSignature::ScanFromNewSig(std::string Signature)
	{
		m_Signature.clear();
		m_Signature.str("");
		m_SignatureBytes.clear();
		m_FoundAddress = 0;
		m_Signature << Signature;

		Signature2Bytes();

		return ScanAtAddress(0); /*0 is the starting addr of m_OpCodes array*/
	}

	EzSignatureStatus EzSignature::ScanAtAddress(std::uintptr_t Address)
	{
		if (!m_Instructions)
			return EzSignatureStatus::ContextNotSet;

		for (auto OpCodeId = 0; OpCodeId < m_NumOpCodes; OpCodeId++)
		{
			auto Bytes = (std::uint8_t*)&m_OpCodes[OpCodeId];
			for (auto i = 0; i < m_SignatureBytes.size(); i++)
			{
				if (m_SignatureBytes[i] == 0) /*wildcard*/
				{
					goto SigPosibleMatchFound;
					continue;
				}

				else if (m_SignatureBytes[i] != Bytes[i])
					break;
			

				SigPosibleMatchFound:
				if (i == (m_SignatureBytes.size() - 1))
				{
					m_FoundAddress = OpCodeId;
					return EzSignatureStatus::NoError;
				}
			}
		}

		return EzSignatureStatus::CouldNotRetrieveSignatureForAddress;
	}

	std::string EzSignature::GetSignature()
	{
		return m_Signature.str();
	}
	EzSignatureStatus EzSignature::Signature2Bytes()
	{
		auto Str = m_Signature.str();

		for (auto i = 0; i < Str.length(); i++)
		{
			if (Str[i] == ' ') /*skip spaces*/
				continue;

			if (Str[i] == '?')
			{
				if (Str[i + std::uint64_t(1)] == '?')
					i++;

				m_SignatureBytes.push_back(0); /*we don't take care of wildcards*/

				continue;
			}

			char Bytes[] = { Str[i], Str[i + std::uint64_t(1)] }; /*store the 2bytes in a single char*/

			auto ConvertedBytes = strtoull(Bytes, nullptr, 16);

			m_SignatureBytes.push_back(std::uint8_t(ConvertedBytes));

			i++;
		}

		return EzSignatureStatus::NoError;
	}

	void EzSignature::AddWhiteSpace()
	{
		m_Signature << " ";
	}
	void EzSignature::AddWildCard()
	{
		AddWhiteSpace();
		m_Signature << "?";
	}

	void EzSignature::AddByte(std::uint8_t Byte)
	{
		AddWhiteSpace();
		m_Signature << std::setw(2) << std::setfill('0') << std::hex << std::uint16_t(Byte);
	}
	void EzSignature::AddInstruction(const EzSignatureInstr& Instr)
	{
		for (auto i = Instr.m_Address; i < Instr.m_Address + Instr.m_Size; i++)
		{
			if (Instr.m_HasWildCards && (i != Instr.m_Address))
				AddWildCard();

			else
				AddByte(m_OpCodes[i]);

		}
	}
}
