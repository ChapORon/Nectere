#pragma once

#include "CommandTreater.hpp"

class Prompt : public CommandTreater
{
private:
	bool m_Running;
	std::string m_Prompt;

protected:
	virtual void RecomputePrompt() {}
	virtual void OnStop() {}

public:
	Prompt() : CommandTreater(), m_Running(true), m_Prompt("$> ") {}
	Prompt(const std::string &prompt): CommandTreater(), m_Running(true), m_Prompt(prompt) {}
	bool IsRunning() const { return m_Running; }
	void Stop();
	void SetPrompt(const std::string &prompt) { m_Prompt = prompt; }
	void Run();
	virtual ~Prompt() { OnStop(); }
};