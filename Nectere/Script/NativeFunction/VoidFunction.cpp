#include "Script/NativeFunction/VoidFunction.hpp"

namespace Nectere
{
	namespace Script
	{
		namespace NativeFunction
		{
			VoidFunction::VoidFunction(void (*function)(CallStack &)) : m_Function(function) {}

			bool VoidFunction::Call(CallStack &stack)
			{
				if (m_Function)
				{
					m_Function(stack);
					return true;
				}
				return false;
			}
		}
	}
}