#include "../pch.h"

namespace Ez
{
	EzSignatureStatus EzSignature::SetSignatureContext(std::uint8_t* OpCodes, std::size_t OpCodeNum,
		EzInstruction** Instructions, std::size_t NumInstrs, rage::RageResourceVersion Version)
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

		return EzSignatureStatus::NoError;
	}
	EzSignatureStatus EzSignature::RetrieveFromAddress(const std::uintptr_t& Address)
	{
		if (!m_Instructions)
			return EzSignatureStatus::ContextNotSet;

		for (auto i = 0; i < m_NumInstructions; i++)
		{
			auto Instruction = m_Instructions[i];
			auto InstructionBytes = &m_OpCodes[Instruction->GetStartAddr()];

			if (Instruction->GetStartAddr() != Address)
				continue;

			auto SignatureBytes = Instruction->GetInstructionSize(); 
			/*the entire instruction is going to be added to the sig if the if above doesnt get 
			executed*/

			if (Instruction->IsCall() || Instruction->IsJump()
				|| Instruction->IsVarLoad() || Instruction->IsVarPush() || Instruction->IsVarStore())
			{
				//the bytes of the operand(s) of these instructions will surely change between updates
				m_Signature += std::to_string(Instruction->GetInstructionId());
				SignatureBytes = 1;
			}
			//the rest of cases won't change betweem updates
			for (auto i = 0; i < Instruction->GetInstructionSize(); i++)
			{
				if (SignatureBytes != Instruction->GetInstructionSize())
				{
					/*we are dealing with opcodes that can change between updates, mark them as wildcards*/
					if (i == 0) /*we already added the first opcode*/
						continue;

					if (i > (SignatureBytes - 1))
						m_Signature += ' ??';
				}
				else
					m_Signature += std::to_string(InstructionBytes[i]);
			}
			/*now that we added an instruction to the signature, scan for it in the script so we know
			when we have a good signature*/
		}

		return EzSignatureStatus::CouldNotCreateSignatureForAddress;
	}
	EzSignatureStatus EzSignature::ScanAtAddress(const std::uintptr_t& Address)
	{
		if (!m_Instructions)
			return EzSignatureStatus::ContextNotSet;

		for (auto i = 0; i < m_NumInstructions; i++)
		{

		}

		return EzSignatureStatus::NoError;
	}
}
