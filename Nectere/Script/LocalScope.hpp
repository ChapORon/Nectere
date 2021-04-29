#pragma once

#include <string>
#include <unordered_map>
#include "Script/Variable.hpp"

namespace Nectere
{
	namespace Script
	{
		class LocalScope final
		{
		private:
			std::unordered_map<std::string, Variable> m_Locals;

		public:
			Variable Get(const std::string &);
			void Add(const std::string &, Variable &&);
			bool Have(const std::string &);
		};
	}
}