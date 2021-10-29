#include "ACommand.hpp"

#include "Application.hpp"

namespace Nectere
{
	ACommand::ACommand(uint16_t id, const std::string &name) : m_CommandID(id), m_Name(name) {}

	void ACommand::SetApplication(const Ptr<Application> &application, const Ptr<IApplicationManager> &applicationManager)
	{
		m_Application = application;
		m_ApplicationManager = applicationManager;
	}

	void ACommand::SendEvent(uint16_t id, const std::string &data)
	{
		m_Application->SendEvent(id, m_CommandID, data);
	}

	void ACommand::SendEvent(const std::vector<uint16_t> &ids, const std::string &data)
	{
		m_Application->SendEvent(ids, m_CommandID, data);
	}
}