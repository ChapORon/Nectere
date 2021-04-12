#include <Nectere/Application.hpp>
#include <Nectere/Logger.hpp>

NECTERE_APPLICATION("Discord")
{
	Nectere::Logger::out.Log("Loading application ", application->GetName());
}