#include "Command/ACanalCommand.hpp"

namespace Nectere
{
	namespace Command
	{
		ACanalCommand::ACanalCommand(uint16_t id, const std::string &name, const Ptr<CanalManager> &canalManager) : ACommand(id, name), m_CanalManager(canalManager) {}
		bool ACanalCommand::IsValid(const std::string &) const { return true; }
	}
}