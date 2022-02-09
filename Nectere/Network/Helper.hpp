#pragma once

#include "Event.hpp"
#include "Logger.hpp"
#include "Network/Header.hpp"

namespace Nectere
{
	namespace Network
	{
		class Helper final
		{
		public:
			static Event Convert(const Header &, const char *);
			static Header GenerateHeader(const Event &event);
			static void LogHeaderInfo(const Logger *);
		};
	}
}