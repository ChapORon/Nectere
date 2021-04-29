#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Script/Variable.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace NativeFunction
		{
			template <typename t_DecalredObjectType>
			class DeclaredObjectFunction;
			template <typename t_RawType>
			class RawFunction;
			class VoidFunction;
		}
		class LocalScope;
		class Native;
		class Parameter;
		class Script;
		class CallStack final
		{
			friend class Function;
			friend class Interpreter;
			template <typename t_DecalredObjectType>
			friend class NativeFunction::DeclaredObjectFunction;
			template <typename t_RawType>
			friend class NativeFunction::RawFunction;
			friend class NativeFunction::VoidFunction;
		private:
			Native *m_Native;
			Script *m_Current;
			std::unordered_map<std::string, Variable> m_Parameters;
			std::vector<LocalScope *> m_Locals;
			Variable m_Return;

		private:
			CallStack(Native *);
			CallStack(Native *, Script *);
			CallStack(Native *, const std::vector<Parameter> &, const std::vector<Variable> &);
			CallStack(Native *, Script *, const std::vector<Parameter> &, const std::vector<Variable> &);
			bool CreateVar(const std::string &, const std::string &, const std::string &, const std::vector<Tag> &);
			inline Variable GetReturn() { return m_Return; }
			void SetReturn(Variable &&);
			bool OpenScope();
			void CloseScope();

		public:
			Variable operator[](const std::string &);
			~CallStack();
		};
	}
}