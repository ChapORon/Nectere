#pragma once

#include "Script/Callable.hpp"
#include "Script/CallStack.hpp"
#include "Script/Variable.hpp"

namespace Nectere
{
	namespace Script
	{
		class NativeUtils;
		namespace NativeFunction
		{
			template <typename t_RawType>
			class RawFunction final : public Callable
			{
				using super = Callable;
				friend class NativeUtils;
			private:
				EVariableType m_ExpectedReturn;
				const t_RawType &(*m_ConstRefFunction)(CallStack &);
				t_RawType &(*m_RefFunction)(CallStack &);
				t_RawType(*m_Function)(CallStack &);

			private:
				RawFunction(EVariableType expectedReturn, const t_RawType &(*constRefFunction)(CallStack &)) : m_ExpectedReturn(expectedReturn), m_ConstRefFunction(constRefFunction), m_RefFunction(nullptr), m_Function(nullptr) {}
				RawFunction(EVariableType expectedReturn, t_RawType &(*refFunction)(CallStack &)) : m_ExpectedReturn(expectedReturn), m_ConstRefFunction(nullptr), m_RefFunction(refFunction), m_Function(nullptr) {}
				RawFunction(EVariableType expectedReturn, t_RawType(*function)(CallStack &)) : m_ExpectedReturn(expectedReturn), m_ConstRefFunction(nullptr), m_RefFunction(nullptr), m_Function(function) {}
				bool Call(CallStack &stack) override
				{
					if (m_Function)
					{
						stack.SetReturn(Variable("", m_Function(stack)));
						return true;
					}
					else if (m_RefFunction)
					{
						stack.SetReturn(Variable("", m_RefFunction(stack)));
						return true;
					}
					else if (m_ConstRefFunction)
					{
						stack.SetReturn(Variable("", m_ConstRefFunction(stack)));
						return true;
					}
					return false;
				}
				inline bool CheckSignature() override { return GetReturnType() == m_ExpectedReturn; }
			};
		}
	}
}