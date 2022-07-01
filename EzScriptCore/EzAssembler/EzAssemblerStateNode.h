#pragma once

namespace Ez
{
	class EzAssemblerStateNode
	{
	public:

		EzAssemblerStateNode(std::string_view Name);

		std::string_view GetName();

	public:

		void Nop(std::size_t Num = 1);

		/*INT arithmetic */
		void Add();
		void Sub();
		void Mul();
		void Div();
		void Mod();

		/*INT condicionals, set RAGE's VM flags*/
		void Not();
		void Neg();
		void IsEqual();
		void IsNotEqual();
		void IsGreaterThan();
		void IsGreaterOrEqual();
		void IsLowerThan();
		void IsLowerOrEqual();

		/*FLOAT arithmetic*/
		void FAdd();
		void FSub();
		void FMul();
		void FDiv();
		void FMod();
		void FNeg();

		/*FLOAT condicionals, set RAGE's VM flags*/
		void FIsEqual();
		void FIsNotEqual();
		void FIsGreaterThan();
		void FIsGreaterOrEqual();
		void FIsLowerThan(); 
		void FIsLowerOrEqual();

		/*more conditionals*/
		void And();
		void Or();
		void Xor();

		void IntToFloat();
		void FloatToInt();

		void Dup();
		void Drop();

		void Native(std::uint64_t NativeHash, std::uint8_t NativeParamCount, std::uint8_t NativeReturnSize);

		void Load();
		void Store();
		void StoreRev();

		void LoadN();
		void StoreN();

		template<typename T, T ArraySize> /*ONLY SUPPORTS T = int8,16 or signed 16*/
		void Array();
		template<typename T, T ArraySize>
		void ArrayLoad();
		template<typename T, T ArraySize>
		void ArrayStore();

		template<typename T, T Index>
		void Local(); /*func params or func variables*/
		template<typename T, T Index>
		void LocalLoad();
		template<typename T, T Index>
		void LocalStore();

		template<typename T, T Index>
		void Static();
		template<typename T, T Index>
		void StaticLoad();
		template<typename T, T Index>
		void StaticStore();

		template<typename T, T OffsetId, bool IsSigned = std::is_signed_v<T>>
		void Offset();
		template<typename T, T OffsetId, bool IsSigned = std::is_signed_v<T>>
		void OffsetLoad();
		template<typename T, T OffsetId, bool IsSigned = std::is_signed_v<T>>
		void OffsetStore();

		template<typename T, T GlobalId>
		void Global();
		template<typename T, T GlobalId>
		void GlobalLoad();
		template<typename T, T GlobalId>
		void GlobalStore();

		template<typename T, T Value, bool IsSiged = std::is_signed_v<T> && !std::is_floating_point_v<T>>
		void Push();

	private:

		void CheckLastNInstrs(std::string_view ThisInstrName, std::size_t N = 2); /*check if there are two instructions in the instr array*/

		std::uint32_t GetNativeId(std::uint64_t NativeHash);

	private:

		std::vector<EzPtr<EzAsmInstruction>> m_Instructions;
		std::map<std::uint32_t, std::uint64_t> m_NativeMap; /*index, native hash*/

		std::string_view m_Name;
	};

	template<typename T>
	inline rage::RageInstr GetConstFromIntValue(T Value, rage::RageInstr Default)
	{
		if (Value == -1)
			return rage::RageInstr::PUSH_CONST_M1;

		else if (Value == 0)
			return rage::RageInstr::PUSH_CONST_0;

		else if (Value == 1)
			return rage::RageInstr::PUSH_CONST_1;

		else if (Value == 2)
			return rage::RageInstr::PUSH_CONST_2;

		else if (Value == 3)
			return rage::RageInstr::PUSH_CONST_3;

		else if (Value == 4)
			return rage::RageInstr::PUSH_CONST_4;

		else if (Value == 5)
			return rage::RageInstr::PUSH_CONST_5;

		else if (Value == 6)
			return rage::RageInstr::PUSH_CONST_6;

		else if (Value == 7)
			return rage::RageInstr::PUSH_CONST_7;

		return Default;
	}

	inline rage::RageInstr GetConstFromFloatValue(float Value)
	{
		if (Value == -1)
			return rage::RageInstr::PUSH_CONST_FM1;

		else if (Value == 0.f)
			return rage::RageInstr::PUSH_CONST_F0;

		else if (Value == 1.f)
			return rage::RageInstr::PUSH_CONST_F1;

		else if (Value == 2.f)
			return rage::RageInstr::PUSH_CONST_F2;

		else if (Value == 3.f)
			return rage::RageInstr::PUSH_CONST_F3;

		else if (Value == 4.f)
			return rage::RageInstr::PUSH_CONST_F4;

		else if (Value == 5.f)
			return rage::RageInstr::PUSH_CONST_F5;

		else if (Value == 6.f)
			return rage::RageInstr::PUSH_CONST_F6;

		else if (Value == 7.f)
			return rage::RageInstr::PUSH_CONST_F7;

		return rage::RageInstr::PUSH_CONST_F;
	}

	template<typename T, T Value, bool IsSiged>
	inline void EzAssemblerStateNode::Push()
	{
		static_assert(!std::is_class_v<T>, "Cannot push class objects!");
		static_assert(!std::is_pointer_v<T>, "Cannot push pointers!");
		static_assert(!(sizeof(T) < 0 || sizeof(T) > 4), "Value type is not valid!");
		static_assert(!(IsSiged && Value > INT16_MAX), "Cannot push signed values bigger than INT16_MAX!");

		auto Instr = rage::NONE;
		auto OperandCount = sizeof(T);
		auto IsFloat = std::is_floating_point_v<T>;
		auto Copy = Value;

		if (IsSiged)
			Instr = rage::RageInstr::PUSH_CONST_S16;

		else
			switch (sizeof(T)) /*note, signed an unsiged types have the same size on bytes, the difference is that
							  signed have a lower MAX because they use some bits to store the sign*/
			{
			case 1: /*sizeof(std::uint8_t)*/
			{
				Instr = GetConstFromIntValue(Value, rage::RageInstr::PUSH_CONST_U8);
			} break;

			case 2: /*sizeof(std::uint16_t)*/
			{
				Instr = GetConstFromIntValue(Value, rage::RageInstr::PUSH_CONST_U8_U8);
			} break;

			default:
			{ /*due to VS, every constant smaller than int32 will be put in an int so we need to check for them here*/

				if (Value > UINT16_MAX && Value < UINT32_MAX) /*we are storing a 24bit value*/
					Instr = GetConstFromIntValue(Value, rage::RageInstr::PUSH_CONST_U8_U8_U8);

				else if (IsFloat)
					Instr = GetConstFromFloatValue(float(Value));

				else
					Instr = GetConstFromIntValue(Value, rage::RageInstr::PUSH_CONST_U32);

			} break;
			}

		auto Instruction = std::make_shared<EzAsmInstruction>(Instr, OperandCount);
		Instruction->SaveOperand((char*)&Copy);
		Instruction->m_IsOperandSigned = IsSiged;
		Instruction->m_IsFloat = IsFloat;

		m_Instructions.push_back(Instruction);
	}

	template<typename T, T ArraySize>
	inline void EzAssemblerStateNode::Array()
	{
		static_assert(!std::is_class_v<T>, "ArraySize cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "ArraySize cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "ArraySize cannot be a float!");
		static_assert(!(ArraySize > UINT16_MAX), "ArraySize cannot be greater than UINT16_MAX!");
		static_assert(!(ArraySize < 0), "ArraySize cannot be below 0!");

		auto Copy = ArraySize;

		EzPtr<EzAsmInstruction> Instr;

		if (ArraySize > UINT8_MAX)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::ARRAY_U16, 2);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::ARRAY_U8, 1);

		Instr->SaveOperand((char*)&Copy);
		m_Instructions.push_back(Instr);
	}
	template<typename T, T ArraySize>
	inline void EzAssemblerStateNode::ArrayLoad()
	{
		static_assert(!std::is_class_v<T>, "ArraySize cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "ArraySize cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "ArraySize cannot be a float!");
		static_assert(!(ArraySize > UINT16_MAX), "ArraySize cannot be greater than UINT16_MAX!");
		static_assert(!(ArraySize < 0), "ArraySize cannot be below 0!");

		auto Copy = ArraySize;
		EzPtr<EzAsmInstruction> Instr;

		if (ArraySize > UINT8_MAX)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::ARRAY_U16_LOAD, 2);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::ARRAY_U8_LOAD, 1);

		Instr->SaveOperand((char*)&Copy);
		m_Instructions.push_back(Instr);
	}
	template<typename T, T ArraySize>
	inline void EzAssemblerStateNode::ArrayStore()
	{
		static_assert(!std::is_class_v<T>, "ArraySize cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "ArraySize cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "ArraySize cannot be a float!");
		static_assert(!(ArraySize > UINT16_MAX), "ArraySize cannot be greater than UINT16_MAX!");
		static_assert(!(ArraySize < 0), "ArraySize cannot be below 0!");

		auto Copy = ArraySize;
		EzPtr<EzAsmInstruction> Instr;

		if (ArraySize > UINT8_MAX)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::ARRAY_U16_STORE, 2);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::ARRAY_U8_STORE, 1);

		Instr->SaveOperand((char*)&Copy);
		m_Instructions.push_back(Instr);
	}

	template<typename T, T Index>
	inline void EzAssemblerStateNode::Local() /*func variables or params*/
	{
		static_assert(!std::is_class_v<T>, "Index cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "Index cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "Index cannot be a float!");
		static_assert(!(Index < 0), "Index cannot be below 0!");
		static_assert(!(Index > UINT16_MAX), "Index cannot be greater than UINT16_MAX!");

		auto Copy = Index;
		EzPtr<EzAsmInstruction> Instr;

		if (Index > UINT8_MAX)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::LOCAL_U16, 2);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::LOCAL_U8, 1);

		Instr->SaveOperand((char*)&Copy);
		m_Instructions.push_back(Instr);
	}
	template<typename T, T Index>
	inline void EzAssemblerStateNode::LocalLoad()
	{
		static_assert(!std::is_class_v<T>, "Index cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "Index cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "Index cannot be a float!");
		static_assert(!(Index < 0), "Index cannot be below 0!");
		static_assert(!(Index > UINT16_MAX), "Index cannot be greater than UINT16_MAX!");

		auto Copy = Index;
		EzPtr<EzAsmInstruction> Instr;

		if (Index > UINT8_MAX)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::LOCAL_U16_LOAD, 2);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::LOCAL_U8_LOAD, 1);

		Instr->SaveOperand((char*)&Copy);
		m_Instructions.push_back(Instr);
	}
	template<typename T, T Index>
	inline void EzAssemblerStateNode::LocalStore()
	{
		static_assert(!std::is_class_v<T>, "Index cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "Index cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "Index cannot be a float!");
		static_assert(!(Index < 0), "Index cannot be below 0!");
		static_assert(!(Index > UINT16_MAX), "Index cannot be greater than UINT16_MAX!");

		auto Copy = Index;
		EzPtr<EzAsmInstruction> Instr;

		if (Index > UINT8_MAX)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::LOCAL_U16_STORE, 2);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::LOCAL_U8_STORE, 1);

		Instr->SaveOperand((char*)&Copy);
		m_Instructions.push_back(Instr);
	}

	template<typename T, T Index>
	inline void EzAssemblerStateNode::Static() /*script params or locals*/
	{
		static_assert(!std::is_class_v<T>, "Index cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "Index cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "Index cannot be a float!");
		static_assert(!(Index < 0), "Index cannot be below 0!");
		static_assert(!(Index > UINT16_MAX), "Index cannot be greater than UINT16_MAX!");

		auto Copy = Index;
		EzPtr<EzAsmInstruction> Instr;

		if (Index > UINT8_MAX)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::STATIC_U16, 2);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::STATIC_U8, 1);

		Instr->SaveOperand((char*)&Copy);
		m_Instructions.push_back(Instr);
	}
	template<typename T, T Index>
	inline void EzAssemblerStateNode::StaticLoad()
	{
		static_assert(!std::is_class_v<T>, "Index cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "Index cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "Index cannot be a float!");
		static_assert(!(Index < 0), "Index cannot be below 0!");
		static_assert(!(Index > UINT16_MAX), "Index cannot be greater than UINT16_MAX!");

		auto Copy = Index;
		EzPtr<EzAsmInstruction> Instr;

		if (Index > UINT8_MAX)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::STATIC_U16_LOAD, 2);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::STATIC_U8_LOAD, 1);

		Instr->SaveOperand((char*)&Copy);
		m_Instructions.push_back(Instr);
	}
	template<typename T, T Index>
	inline void EzAssemblerStateNode::StaticStore()
	{
		static_assert(!std::is_class_v<T>, "Index cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "Index cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "Index cannot be a float!");
		static_assert(!(Index < 0), "Index cannot be below 0!");
		static_assert(!(Index > UINT16_MAX), "Index cannot be greater than UINT16_MAX!");

		auto Copy = Index;
		EzPtr<EzAsmInstruction> Instr;

		if (Index > UINT8_MAX)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::STATIC_U16_STORE, 2);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::STATIC_U8_STORE, 1);

		Instr->SaveOperand((char*)&Copy);
		m_Instructions.push_back(Instr);
	}

	template<typename T, T OffsetId, bool IsSigned>
	inline void EzAssemblerStateNode::Offset()
	{ /*not quite sure if this is really signed, i'm basing on maybegreat48's research*/
		static_assert(!std::is_class_v<T>, "OffsetId cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "OffsetId cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "OffsetId cannot be a float!");
		static_assert(!(!IsSigned && OffsetId < 0), "Non-signed OffsetIds cannot be below 0!");
		static_assert(!(!IsSigned && OffsetId > UINT8_MAX), "Non-signed OffsetIds cannot be greater than UINT8_MAX!");
		static_assert(!(OffsetId > INT16_MAX), "OffsetId cannot be greater than INT16_MAX!");

		auto Copy = OffsetId;
		EzPtr<EzAsmInstruction> Instr;

		if (IsSigned)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::IOFFSET_S16, 2);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::IOFFSET_U8, 1);

		Instr->SaveOperand((char*)&Copy);
		Instr->m_IsOperandSigned = IsSigned;
		m_Instructions.push_back(Instr);
	}
	template<typename T, T OffsetId, bool IsSigned>
	inline void EzAssemblerStateNode::OffsetLoad()
	{
		static_assert(!std::is_class_v<T>, "OffsetId cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "OffsetId cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "OffsetId cannot be a float!");
		static_assert(!(!IsSigned && OffsetId < 0), "Non-signed OffsetIds cannot be below 0!");
		static_assert(!(!IsSigned && OffsetId > UINT8_MAX), "Non-signed OffsetIds cannot be greater than UINT8_MAX!");
		static_assert(!(OffsetId > INT16_MAX), "OffsetId cannot be greater than INT16_MAX!");
		
		auto Copy = OffsetId;
		EzPtr<EzAsmInstruction> Instr;

		if (IsSigned)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::IOFFSET_S16_LOAD, 2);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::IOFFSET_U8_LOAD, 1);

		Instr->SaveOperand((char*)&Copy);
		Instr->m_IsOperandSigned = IsSigned;
		m_Instructions.push_back(Instr);
	}
	template<typename T, T OffsetId, bool IsSigned>
	inline void EzAssemblerStateNode::OffsetStore()
	{
		static_assert(!std::is_class_v<T>, "OffsetId cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "OffsetId cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "OffsetId cannot be a float!");
		static_assert(!(!IsSigned && OffsetId < 0), "Non-signed OffsetIds cannot be below 0!");
		static_assert(!(!IsSigned && OffsetId > UINT8_MAX), "Non-signed OffsetIds cannot be greater than UINT8_MAX!");
		static_assert(!(OffsetId > INT16_MAX), "OffsetId cannot be greater than INT16_MAX!");

		auto Copy = OffsetId;
		EzPtr<EzAsmInstruction> Instr;

		if (IsSigned)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::IOFFSET_S16_STORE, 2);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::IOFFSET_U8_STORE, 1);

		Instr->SaveOperand((char*)&Copy);
		Instr->m_IsOperandSigned = IsSigned;
		m_Instructions.push_back(Instr);
	}

	template<typename T, T GlobalId>
	inline void EzAssemblerStateNode::Global()
	{
		static_assert(!std::is_class_v<T>, "OffsetId cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "OffsetId cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "OffsetId cannot be a float!");
		static_assert(!(GlobalId < 0), "OffsetId cannot be below 0!");
		static_assert(!(GlobalId > (UINT32_MAX - UINT8_MAX)), "OffsetId cannot be greater than 24bits!");

		auto Copy = GlobalId;
		EzPtr<EzAsmInstruction> Instr;

		if (GlobalId > UINT16_MAX)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::GLOBAL_U24, 3);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::GLOBAL_U16, 2);

		Instr->SaveOperand((char*)&Copy);
		m_Instructions.push_back(Instr);
	}
	template<typename T, T GlobalId>
	inline void EzAssemblerStateNode::GlobalLoad()
	{
		static_assert(!std::is_class_v<T>, "OffsetId cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "OffsetId cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "OffsetId cannot be a float!");
		static_assert(!(GlobalId < 0), "OffsetId cannot be below 0!");
		static_assert(!(GlobalId > (UINT32_MAX - UINT8_MAX)), "OffsetId cannot be greater than 24bits!");

		auto Copy = GlobalId;
		EzPtr<EzAsmInstruction> Instr;

		if (GlobalId > UINT16_MAX)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::GLOBAL_U24_LOAD, 3);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::GLOBAL_U16_LOAD, 2);

		Instr->SaveOperand((char*)&Copy);
		m_Instructions.push_back(Instr);
	}
	template<typename T, T GlobalId>
	inline void EzAssemblerStateNode::GlobalStore()
	{
		static_assert(!std::is_class_v<T>, "OffsetId cannot be a class obj!");
		static_assert(!std::is_pointer_v<T>, "OffsetId cannot be a ptr!");
		static_assert(!std::is_floating_point_v<T>, "OffsetId cannot be a float!");
		static_assert(!(GlobalId < 0), "OffsetId cannot be below 0!");
		static_assert(!(GlobalId > (UINT32_MAX - UINT8_MAX)), "OffsetId cannot be greater than 24bits!");

		auto Copy = GlobalId;
		EzPtr<EzAsmInstruction> Instr;

		if (GlobalId > UINT16_MAX)
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::GLOBAL_U24_STORE, 3);

		else
			Instr = std::make_shared<EzAsmInstruction>(rage::RageInstr::GLOBAL_U16_STORE, 2);

		Instr->SaveOperand((char*)&Copy);
		m_Instructions.push_back(Instr);
	}
}

