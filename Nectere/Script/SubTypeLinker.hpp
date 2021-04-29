#pragma once

#include "Script/Variable.hpp"

namespace Nectere
{
	namespace Script
	{
		template <typename t_LinkerType>
		class Linker;
		template <typename t_LinkerObjectType>
		class SubTypeLinker
		{
			friend class Linker<t_LinkerObjectType>;
		private:
			bool m_Valid;

		protected:
			SubTypeLinker(bool valid) : m_Valid(valid) {}

		private:
			virtual Variable Convert(const std::string &, const t_LinkerObjectType &, const CurrentEnvironment &) = 0;
			virtual bool Fill(t_LinkerObjectType &, const Variable &) = 0;
			virtual std::string ToString(const t_LinkerObjectType &) = 0;

		public:
			operator bool() const { return m_Valid; }
			virtual ~SubTypeLinker() = default;
		};
	}
}