#pragma once

#include "Script/ILinkerWrapper.hpp"
#include "Script/Linker.hpp"

namespace Nectere
{
	namespace Script
	{
		template <typename t_DecalredObjectType>
		class DeclaredObjectFunction;
		template <typename t_LinkerType>
		class LinkerWrapper final : public ILinkerWrapper
		{
			using super = ILinkerWrapper;
			friend class DeclaredObjectFunction<t_LinkerType>;
			friend class NativeUtils;
		private:
			Linker<t_LinkerType> m_Linker;

		public:
			LinkerWrapper(Linker<t_LinkerType> &linker) : m_Linker(linker) {}
			inline bool Compile(Native *native) { return m_Linker.Compile(native); }
			inline Variable Convert(const std::string &name, const t_LinkerType &value, const CurrentEnvironment &helper) { return m_Linker.Convert(name, value, helper); }
			inline VariableType GetTypeOf() { return m_Linker.GetTypeOf(); }
			inline bool Fill(const Variable &variable, t_LinkerType &value) { return m_Linker.Fill(variable, value); }
		};
	}
}