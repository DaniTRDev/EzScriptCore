
#ifndef PCH_H
#define PCH_H


#include <Windows.h>
#include <functional>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <chrono>
#include <map>
#include <stack>
#include <filesystem>
#include <thread>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <string_view>

#ifdef EZEXPORT
#define EZAPI __declspec(dllexport)
#else 
#define EZAPI __declspec(dllimport)
#endif

namespace Util
{
	inline std::uint16_t Bits2U16(std::uint8_t First, std::uint8_t Second)
	{
		return std::uint16_t(First << 8 | (Second));
	}
	inline std::int16_t Bits2I16(std::int8_t First, std::int8_t Second)
	{
		return std::uint16_t(First << 8 | (Second));
	}

	inline std::uint32_t Bits2U32(std::uint8_t First, std::uint8_t Second, std::uint8_t Third, std::uint8_t Fourth)
	{
		if (Fourth)
			return ((First << 24) | (Second << 16) | (Third << 8) | (Fourth));

		return ((First << 16) | (Second << 8) | (Third));
	}
	inline std::int32_t Bits2I32(std::int8_t First, std::int8_t Second, std::int8_t Third, std::int8_t Fourth)
	{
		if (Fourth)
			return ((First << 24) | (Second << 16) | (Third << 8) | (Fourth));

		return ((First << 16) | (Second << 8) | (Third));
	}

	inline std::uint16_t SwapBitsU16(std::uint16_t Value)
	{
		auto Bytes = (std::uint8_t*)&Value;
		return Bits2I16(Bytes[1], Bytes[0]);
	}
	inline std::int16_t SwapBitsS16(std::int16_t Value)
	{
		auto Bytes = (std::int8_t*)&Value;
		return Bits2I16(Bytes[1], Bytes[0]);
	}

	inline std::uint32_t SwapBitsU32(std::uint32_t Value)
	{
		auto Bytes = (std::uint8_t*)&Value;
		return Bits2I32(Bytes[3], Bytes[2], Bytes[1], Bytes[0]);
	}
	inline std::int32_t SwapBitsS32(std::int32_t Value)
	{
		auto Bytes = (std::int8_t*)&Value;
		return Bits2I32(Bytes[3], Bytes[2], Bytes[1], Bytes[0]);
	}

}

namespace Ez
{
	template <typename T>
	using EzPtr = std::shared_ptr<T>;

}

#include "Rage/RageEnums.h"

#include "EzBuffer/EzBuffer.h"
#include "Rage/EzSrcProgram.h"
#include "EzInstruction/EzInstruction.h"
#include "EzInstruction/Instructions/EzJmp.h"
#include "EzInstruction/Instructions/EzSwitch.h"
#include "EzInstruction/Instructions/EzNativeCall.h"
#include "EzInstruction/Instructions/EzVarInstr.h"
#include "EzInstruction/Instructions/EzCall.h"
#include "EzInstruction/Instructions/EzFloatPush.h"
#include "EzInstruction/Instructions/EzSigned.h"
#include "EzSignature/EzSignature.h"
#include "EzFunction/EzFunction.h"
#include "EzDisassembler/EzDisassembler.h"
#include "EzException/EzException.h"
#include "EzAssembler/EzCodeHolder.h"
#include "EzAssembler/EzAsmInstruction.h"
#include "EzAssembler/EzAssemblerStateNode.h"
#include "EzAssembler/EzAsmFunc.h"
#include "EzAssembler/EzAssembler.h"

#endif //PCH_H
