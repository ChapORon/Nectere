#pragma once

#include "Script/Callable.hpp"

namespace Nectere
{
	namespace Script
	{
		class NativeUtils;
		namespace NativeFunction
		{
			class VoidFunction final : public Callable
			{
				using super = Callable;
				friend class NativeUtils;
			private:
				void (*m_Function)(CallStack &);

			private:
				VoidFunction(void (*)(CallStack &));
				bool Call(CallStack &) override;
				inline bool CheckSignature() override { return GetReturnType() == EVariableType::Void; }
			};
		}
	}
}