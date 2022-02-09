#pragma once

#include "nectere_export.h"

namespace Nectere
{
	class Application;
	class ACommand;

	namespace Dp
	{
		class Node;
	}

	class NECTERE_EXPORT AApplicationHandler
	{
		friend class Application;
	private:
		Application *m_Application;

	protected:
		virtual void OnCommandAdded(ACommand *) = 0;
		virtual void OnUpdate(float) = 0;
		virtual void OnBeforeReload(Dp::Node &) = 0;
		virtual void OnAfterReload(const Dp::Node &) = 0;
	};
}