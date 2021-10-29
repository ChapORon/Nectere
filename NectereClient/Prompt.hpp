#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class Prompt
{
private:
	bool m_Running;
	std::string m_Prompt;
	std::unordered_map<std::string, std::function<void(const std::vector<std::string> &)>> m_Commands;
	std::unordered_map<std::string, std::function<void()>> m_EmptyCommands;

private:
	std::vector<std::string> SplitCommand(const std::string &) const;
	void TreatCommand(const std::string &) const;

protected:
	virtual void RecomputePrompt() {}
	virtual void OnStop() {}

public:
	Prompt() : m_Running(true), m_Prompt("$> ") {}
	Prompt(const std::string &prompt): m_Running(true), m_Prompt(prompt) {}
	bool IsRunning() const { return m_Running; }
	void Stop();
	void SetPrompt(const std::string &prompt) { m_Prompt = prompt; }
	void AddCommand(const std::string &key, const std::function<void(const std::vector<std::string> &)> &fct) { m_Commands[key] = fct; }
	template <typename T>
	void AddCommand(const std::string &key, T *obj, void (T::*fct)(const std::vector<std::string> &)) { m_Commands[key] = std::bind(std::mem_fn(fct), obj, std::placeholders::_1); }
	void AddCommand(const std::string &key, const std::function<void()> &fct) { m_EmptyCommands[key] = fct; }
	template <typename T>
	void AddCommand(const std::string &key, T *obj, void (T::*fct)()) { m_EmptyCommands[key] = std::bind(std::mem_fn(fct), obj); }
	void Run();
	virtual ~Prompt() { OnStop(); }
};