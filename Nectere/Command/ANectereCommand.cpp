#include "ANectereCommand.hpp"

namespace Nectere
{
	namespace Command
	{
		ANectereCommand::ANectereCommand(uint16_t id, Server *server): ACommand(id), m_Server(server) {}
	}
}