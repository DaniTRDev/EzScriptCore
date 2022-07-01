#include "../pch.h"

namespace Ez
{
	EzAssemblerStateNode::EzAssemblerStateNode(std::string_view Name) : m_Name(Name)
	{
	}
	std::string_view EzAssemblerStateNode::GetName()
	{
		return m_Name;
	}

	void EzAssemblerStateNode::Nop(std::size_t Num)
	{
		for (std::size_t i = 0; i < Num; i++)
			m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::NOP));
	}

	void EzAssemblerStateNode::Add()
	{
		CheckLastNInstrs("Add");

		auto LastInstr = m_Instructions[m_Instructions.size() - 1];
		auto LastInstr2 = m_Instructions[m_Instructions.size() - 2];

		if (LastInstr->IsSigned() || LastInstr2->IsSigned())
			m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::IADD_S16));

		else
			m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::IADD));
	}
	void EzAssemblerStateNode::Sub()
	{
		CheckLastNInstrs("Sub");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::ISUB));
	}
	void EzAssemblerStateNode::Mul()
	{
		CheckLastNInstrs("Mul");
		
		auto LastInstr = m_Instructions[m_Instructions.size() - 1];
		auto LastInstr2 = m_Instructions[m_Instructions.size() - 2];

		if (LastInstr->IsSigned() || LastInstr2->IsSigned())
			m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::IMUL_S16));

		else
			m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::IMUL));
	}
	void EzAssemblerStateNode::Div()
	{
		CheckLastNInstrs("Div");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FDIV));
	}
	void EzAssemblerStateNode::Mod()
	{
		CheckLastNInstrs("Mod");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::IMOD));
	}

	void EzAssemblerStateNode::Not()
	{
		CheckLastNInstrs("Not");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::INOT));
	}
	void EzAssemblerStateNode::Neg()
	{
		CheckLastNInstrs("Neg");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::INEG));
	}
	void EzAssemblerStateNode::IsEqual()
	{
		CheckLastNInstrs("EQ");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::IEQ));
	}
	void EzAssemblerStateNode::IsNotEqual()
	{
		CheckLastNInstrs("NE");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::INE));
	}
	void EzAssemblerStateNode::IsGreaterThan()
	{
		CheckLastNInstrs("GT");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::IGT));
	}
	void EzAssemblerStateNode::IsGreaterOrEqual()
	{
		CheckLastNInstrs("GE");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::IGE));
	}
	void EzAssemblerStateNode::IsLowerThan()
	{
		CheckLastNInstrs("LT");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::ILT));
	}
	void EzAssemblerStateNode::IsLowerOrEqual()
	{
		CheckLastNInstrs("LE");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::ILE));
	}

	void EzAssemblerStateNode::FAdd()
	{
		CheckLastNInstrs("FAdd");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FADD));
	}
	void EzAssemblerStateNode::FSub()
	{
		CheckLastNInstrs("FSub");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FSUB));
	}
	void EzAssemblerStateNode::FMul()
	{
		CheckLastNInstrs("FMul");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FMUL));
	}
	void EzAssemblerStateNode::FDiv()
	{
		CheckLastNInstrs("FDiv");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FDIV));
	}
	void EzAssemblerStateNode::FMod()
	{
		CheckLastNInstrs("FMod");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FMOD));
	}

	void EzAssemblerStateNode::FNeg()
	{
		CheckLastNInstrs("FNeg");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FNEG));
	}
	void EzAssemblerStateNode::FIsEqual()
	{
		CheckLastNInstrs("FEQ");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FEQ));
	}
	void EzAssemblerStateNode::FIsNotEqual()
	{
		CheckLastNInstrs("FNE");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FNE));
	}
	void EzAssemblerStateNode::FIsGreaterThan()
	{
		CheckLastNInstrs("FGT");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FGT));
	}
	void EzAssemblerStateNode::FIsGreaterOrEqual()
	{
		CheckLastNInstrs("FGE");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FGE));
	}
	void EzAssemblerStateNode::FIsLowerThan()
	{
		CheckLastNInstrs("FLT");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FLT));
	}
	void EzAssemblerStateNode::FIsLowerOrEqual()
	{
		CheckLastNInstrs("FLE");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::FLE));
	}

	void EzAssemblerStateNode::And()
	{
		CheckLastNInstrs("AND");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::IAND));
	}
	void EzAssemblerStateNode::Or()
	{
		CheckLastNInstrs("OR");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::IOR));
	}
	void EzAssemblerStateNode::Xor()
	{
		CheckLastNInstrs("XOR");
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::IXOR));
	}

	void EzAssemblerStateNode::IntToFloat()
	{
		CheckLastNInstrs("I2F", 1);
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::I2F));
	}
	void EzAssemblerStateNode::FloatToInt()
	{
		CheckLastNInstrs("F2I", 1);
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::F2I));
	}

	void EzAssemblerStateNode::Dup()
	{
		CheckLastNInstrs("Dup", 1);
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::DUP));
	}
	void EzAssemblerStateNode::Drop()
	{
		CheckLastNInstrs("Drop", 1);
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::DROP));
	}

	void EzAssemblerStateNode::Native(std::uint64_t Hash, std::uint8_t ParamCount, std::uint8_t ReturnSize)
	{
		CheckLastNInstrs("Native", ParamCount);

		auto Instruction = std::make_shared<EzAsmInstruction>(rage::RageInstr::NATIVE);
		Instruction->m_IsNativeCall = true;
		Instruction->m_NativeIndex = GetNativeId(Hash);
		Instruction->m_NativeParameterCount = ParamCount;
		Instruction->m_NativeReturnCount = ReturnSize;

		m_Instructions.push_back(Instruction);
	}

	void EzAssemblerStateNode::Load()
	{
		CheckLastNInstrs("Load", 1);
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::LOAD));
	}
	void EzAssemblerStateNode::Store()
	{
		CheckLastNInstrs("Store", 1);
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::STORE));
	}
	void EzAssemblerStateNode::StoreRev()
	{
		CheckLastNInstrs("StoreRev", 1);
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::STORE_REV));
	}

	void EzAssemblerStateNode::LoadN()
	{
		CheckLastNInstrs("LoadN", 1);
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::LOAD_N));
	}
	void EzAssemblerStateNode::StoreN()
	{
		CheckLastNInstrs("StoreN", 1);
		m_Instructions.push_back(std::make_shared<EzAsmInstruction>(rage::RageInstr::STORE_N));
	}

	void EzAssemblerStateNode::CheckLastNInstrs(std::string_view ThisInstrName, std::size_t N)
	{
		if (!m_Instructions.size() || m_Instructions.size() < N)
			throw EzException().SetExceptionClass(typeid(this).name())
			.SetExceptionFunc(__func__)
			.SetExceptionInfo(std::string("Cannot put a(n)")
				.append(ThisInstrName)
				.append(" instruction when there are not enough operands!"));
	}

	std::uint32_t EzAssemblerStateNode::GetNativeId(std::uint64_t NativeHash)
	{
		for (auto& [Id, Hash] : m_NativeMap)
		{
			if (Hash == NativeHash)
				return Id;
		}

		m_NativeMap.insert({ std::uint32_t(m_NativeMap.size()), NativeHash });
		return std::uint32_t(m_NativeMap.size() - 1);
	}
}