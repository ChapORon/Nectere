#include "ANectereCommand.hpp"

namespace Nectere
{
	namespace Command
	{
		ANectereCommand::ANectereCommand(uint16_t id, const std::string &name, Network::AServer *server, Concurrency::ThreadSystem *threadSystem): ACommand(id, name),
			m_Server(server),
			m_ThreadSystem(threadSystem) {}
	}
}