
#ifndef PCH_H
#define PCH_H

#include <functional>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <stack>
#include <filesystem>
#include <string_view>

#include <include/magic_enum.hpp>

#ifdef EZEXPORT
#define EZAPI __declspec(dllexport)
#else 
#define EZAPI __declspec(dllimport)
#endif

namespace Util
{
	inline std::int16_t Bits2I16(std::uint8_t First, std::uint8_t Second)
	{
		return std::uint16_t(First << 8 | (Second));
	}
	inline std::int16_t Bits2I16(std::int8_t First, std::int8_t Second)
	{
		return std::uint16_t(First << 8 | (Second));
	}

	inline std::int32_t Bits2I32(std::uint8_t First, std::uint8_t Second, std::uint8_t Third, std::uint8_t Fourth)
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

	inline std::int16_t SwapBits16(std::uint16_t Value)
	{
		auto Bytes = (std::uint8_t*)&Value;
		return Bits2I16(Bytes[1], Bytes[0]);
	}
	inline std::int16_t SwapBits16(std::int16_t Value)
	{
		auto Bytes = (std::int8_t*)&Value;
		return Bits2I16(Bytes[1], Bytes[0]);
	}

	inline std::int32_t SwapBits32(std::uint32_t Value)
	{
		auto Bytes = (std::uint8_t*)&Value;
		return Bits2I32(Bytes[3], Bytes[2], Bytes[1], Bytes[0]);
	}
	inline std::int32_t SwapBits32(std::int32_t Value)
	{
		auto Bytes = (std::int8_t*)&Value;
		return Bits2I32(Bytes[3], Bytes[2], Bytes[1], Bytes[0]);
	}
}

#include "InstructionEnums.h"

#include "EzBuffer.h"
#include "EzSrcProgram.h"
#include "EzStack/EzStack.h"
#include "EzInstruction/EzInstruction.h"
#include "EzInstruction/Instructions/EzJmp.h"
#include "EzInstruction/Instructions/EzSwitch.h"
#include "EzFunction/EzFunction.h"
#include "EzDecompiler/EzDecompiler.h"

#endif //PCH_H
