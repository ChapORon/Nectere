#include "Prompt.hpp"

#include <iostream>

void Prompt::Run()
{
	std::string line;
	do
	{
		if (!line.empty())
			TreatCommand(line);
		if (IsRunning())
		{
			RecomputePrompt();
			std::cout << m_Prompt;
		}
	} while (IsRunning() && std::getline(std::cin, line));
	OnStop();
}

void Prompt::Stop()
{
	m_Running = false;
	OnStop();
}