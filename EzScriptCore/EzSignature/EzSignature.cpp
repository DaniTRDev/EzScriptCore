#include "../pch.h"

namespace Ez
{
	void EzSignature::SetSignatureContext(
		std::uint8_t* OpCodes, std::size_t OpCodeNum, std::shared_ptr<EzInstruction>* Instructions, 
		std::size_t NumInstrs, rage::RageResourceVersion Version)
	{
		if (m_Instructions)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("Could not set signature context it has already been set!");

		else if (!OpCodes)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::uint8_t*")
			.SetExceptionInfo("Could not set signature context because opcodes array is not valid!");

		else if (!OpCodeNum)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::size_t")
			.SetExceptionInfo("Could not set signature context because opcodes number is not valid!");

		else if (!Instructions)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::shared_ptr<EzInstruction>*")
			.SetExceptionInfo("Could not set signature context because instructions array is not valid!");

		else if (!NumInstrs)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::istream&")
			.SetExceptionInfo("Could not create buffer from std::istream because it is not valid!");

		m_OpCodes = OpCodes;
		m_NumOpCodes = OpCodeNum;
		m_Instructions = Instructions;
		m_NumInstructions = NumInstrs;
		m_Version = Version;
		m_SignatureAddress = 0;
		m_FoundAddress = 0;
	}

	void EzSignature::CreateAtAddress(std::uintptr_t Address)
	{
		if (!m_Instructions)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("Could not create signature because context is not set!");

		m_SignatureAddress = Address;
		bool AreWeOnCorrectAddr = false;
		for (auto i = 0; i < m_NumInstructions; i++)
		{
			auto Instruction = m_Instructions[i];
			auto InstructionBytes = &m_OpCodes[Instruction->GetStartAddr()];
			EzSignatureInstr SigInstr{ Instruction->GetStartAddr(), Instruction->GetInstructionSize(), false };

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
			else if (Instruction->IsSwitch())
			{ /*we need to specifically check for this instruction because of the particular
				way RAGE's interpreter handle it*/

				AddByte(Instruction->GetInstructionId()); /*add the opcode to the sig*/
				AddWildCards(sizeof(rage::FuncSwitch::m_NumCases) / sizeof(std::uint8_t)); 
				/*in case r* add more cases put the case number as a wildcard*/

				for (auto& Case : Instruction->GetSwitchCases()) 
					/*loop throught each case and add it to the signature*/
				{
					auto CaseVal = Case.first;

					AddBytes((std::uint8_t*)&CaseVal, sizeof(CaseVal));
					AddWildCards(sizeof(rage::FuncSwitchCase::m_JumpOffset));
					/*put as many wildcards as jump offset's size*/

					if (CheckForSigMatchAddr(m_Signature.str()))
						return; /*we got a good sig, no need of adding more instructions to sig*/
					
				}

				continue; /*if we finished adding switch opcodes to the sig and still don't have
						  a good sig, we are going to add more instructions, so kip the switch that
						  has already been added*/
			}

			AddInstruction(SigInstr);

			if (CheckForSigMatchAddr(m_Signature.str()))
				return;
		}

		throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionName("std::istream&")
			.SetExceptionInfo("Could not create signature for this address!");
	}
	bool EzSignature::ScanFromNewSig(std::string Signature)
	{
		m_Signature.clear();
		m_Signature.str("");
		m_SignatureBytes.clear();
		m_FoundAddress = 0;
		m_Signature << Signature;

		Signature2Bytes();

		return ScanAtAddress(0); /*0 is the starting addr of m_OpCodes array*/
	}

	bool EzSignature::ScanAtAddress(std::uintptr_t Address)
	{
		if (!m_Instructions)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo("Could not scan for signature because context is not set!");

		for (auto OpCodeId = 0; OpCodeId < m_NumOpCodes; OpCodeId++)
		{
			auto Bytes = (std::uint8_t*)&m_OpCodes[OpCodeId];
			for (auto i = 0; i < m_SignatureBytes.size(); i++)
			{
				if (m_SignatureBytes[i].m_IsWildCard)
				{
					goto PosibleMatch;
					continue;
				}

				else if (m_SignatureBytes[i].m_Byte != Bytes[i])
					break;

				PosibleMatch:
				if (i == (m_SignatureBytes.size() - 1))
				{
					m_FoundAddress = OpCodeId;
					return true;
				}
			
			}
		}

		return false;
	}

	std::string EzSignature::GetSignature()
	{
		return m_Signature.str();
	}
	void EzSignature::Signature2Bytes()
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

				m_SignatureBytes.push_back({ 0, true }); /*we don't take care of wildcards*/

				continue;
			}

			char Bytes[] = { Str[i], Str[i + std::uint64_t(1)] }; /*store the 2bytes in a single char*/

			auto ConvertedBytes = strtoull(Bytes, nullptr, 16);

			m_SignatureBytes.push_back({ std::uint8_t(ConvertedBytes), false });

			i++;
		}
	}

	void EzSignature::AddWhiteSpace()
	{
		m_Signature << " ";
	}
	void EzSignature::AddWildCards(std::uint8_t Num)
	{
		for (std::uint8_t i = 0; i < Num; i++)
		{
			AddWhiteSpace();
			m_Signature << "?";
		}
	}

	void EzSignature::AddByte(std::uint8_t Byte)
	{
		AddWhiteSpace();
		m_Signature << std::setw(2) << std::setfill('0') << std::hex << std::uint16_t(Byte);
	}
	void EzSignature::AddBytes(std::uint8_t* Bytes, std::size_t NumBytes)
	{
		for (std::size_t i = 0; i < NumBytes; i++)
			AddByte(Bytes[i]);	
	}
	void EzSignature::AddInstruction(const EzSignatureInstr& Instr)
	{
		for (auto i = Instr.m_Address; i < Instr.m_Address + Instr.m_Size; i++)
		{
			if (Instr.m_HasWildCards && (i != Instr.m_Address))
				AddWildCards();

			else
				AddByte(m_OpCodes[i]);

		}
	}

	bool EzSignature::CheckForSigMatchAddr(std::string Sig)
	{
		if (ScanFromNewSig(m_Signature.str()))
			/*sig had a match, lets check if it's the one we want*/
			return (m_FoundAddress == m_SignatureAddress);

		return false;
	}
}
