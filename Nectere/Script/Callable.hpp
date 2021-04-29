#pragma once

#include "Script/Signature.hpp"
#include "Script/Taggable.hpp"

namespace Nectere
{
	namespace Script
	{
		class CurrentEnvironment;
		class CallStack;
		class Variable;
		class Callable : public Taggable
		{
			friend class Function;
			friend class Native;
			friend class NativeUtils;
			friend class Script;
			friend class SingleScriptParser;
		protected:
			Signature m_Signature;

		private:
			std::unordered_map<std::string, std::string> m_DefaultValues;

		private:
			bool Compile(const CurrentEnvironment &);
			virtual bool Call(CallStack &) = 0;
			virtual bool CheckSignature() { return true; }
			void SetReturnType(const VariableType &);
			void SetName(const std::string &);
			void AddDefaultValue(const std::string &, const std::string &);
			void AddArgument(const std::string &, const VariableType &, const Parameter::Type &);

		public:
			bool operator()(CallStack &);
			bool CheckParameter(const std::vector<Variable> &) const;
			inline const std::string &GetName() const { return m_Signature.m_Name; }
			inline const VariableType &GetReturnType() const { return m_Signature.m_ReturnType; }
			inline const std::vector<Parameter> &GetParameters() const { return m_Signature.m_Arguments; }
			virtual ~Callable() = default;
		};
	}
}