#pragma once

#include "Script/Callable.hpp"
#include "Script/CallStack.hpp"
#include "Script/LinkerWrapper.hpp"
#include "Script/NativeUtils.hpp"

namespace Nectere
{
	namespace Script
	{
		class Native;
		class NativeUtils;
		namespace NativeFunction
		{
			template <typename t_DecalredObjectType>
			class DeclaredObjectFunction final : public Callable
			{
				using super = Callable;
				friend class Native;
				friend class NativeUtils;
			private:
				const CurrentEnvironment &m_Helper;
				Native *m_Native;
				LinkerWrapper<t_DecalredObjectType> *m_Wrapper;
				const t_DecalredObjectType &(*m_ConstRefFunction)(CallStack &);
				t_DecalredObjectType &(*m_RefFunction)(CallStack &);
				t_DecalredObjectType(*m_Function)(CallStack &);

			private:
				DeclaredObjectFunction(const CurrentEnvironment &helper, Native *native, const t_DecalredObjectType &(*constRefFunction)(CallStack &)) : m_Helper(helper), m_Native(native), m_Wrapper(nullptr), m_ConstRefFunction(constRefFunction), m_RefFunction(nullptr), m_Function(nullptr) {}
				DeclaredObjectFunction(const CurrentEnvironment &helper, Native *native, t_DecalredObjectType &(*refFunction)(CallStack &)) : m_Helper(helper), m_Native(native), m_Wrapper(nullptr), m_ConstRefFunction(nullptr), m_RefFunction(refFunction), m_Function(nullptr) {}
				DeclaredObjectFunction(const CurrentEnvironment &helper, Native *native, t_DecalredObjectType(*function)(CallStack &)) : m_Helper(helper), m_Native(native), m_Wrapper(nullptr), m_ConstRefFunction(nullptr), m_RefFunction(nullptr), m_Function(function) {}
				inline void SetLinkerWrapper(LinkerWrapper<t_DecalredObjectType> *wrapper) { m_Wrapper = wrapper; }

				bool CheckSignature() override
				{
					if (std::is_same<Variable, t_DecalredObjectType>::value)
						return true;
					if (m_Wrapper)
						return GetReturnType() == m_Wrapper->GetTypeOf();
					return false;
				}

				bool Call(CallStack &stack) override
				{
					if (!m_Wrapper)
						return false;
					if (m_Function)
					{
						stack.SetReturn(m_Wrapper->Convert("", m_Function(stack), m_Helper));
						return true;
					}
					else if (m_RefFunction)
					{
						stack.SetReturn(m_Wrapper->Convert("", m_RefFunction(stack), m_Helper));
						return true;
					}
					else if (m_ConstRefFunction)
					{
						stack.SetReturn(m_Wrapper->Convert("", m_ConstRefFunction(stack), m_Helper));
						return true;
					}
					return false;
				}
			};
		}
	}
}