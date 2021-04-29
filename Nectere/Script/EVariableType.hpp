#pragma once

namespace Nectere
{
	namespace Script
	{
		enum class EVariableType : char
		{
			Void,
			Array,
			Group,
			Bool,
			Char,
			UnsignedChar,
			Short,
			UnsignedShort,
			Int,
			UnsignedInt,
			Long,
			UnsignedLong,
			LongLong,
			UnsignedLongLong,
			Float,
			Double,
			LongDouble,
			String,
			Object,
			Count,
			Invalid
		};

		constexpr int EVARIABLETYPE_SIZE = static_cast<int>(EVariableType::Count);
	}
}