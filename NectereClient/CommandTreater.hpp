#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class CommandTreater
{
private:
	std::unordered_map<std::string, std::function<void(const std::vector<std::string> &)>> m_Commands;
	std::unordered_map<std::string, std::function<void()>> m_EmptyCommands;

private:
	std::vector<std::string> SplitCommand(const std::string &) const;
	void HandleSubCommand(const std::vector<std::string> &, const std::string &, const CommandTreater &);

public:
	CommandTreater() = default;
	void TreatCommand(const std::string &command) const { TreatCommand(SplitCommand(command)); }
	void TreatCommand(const std::vector<std::string> &) const;
	void AddCommand(const std::string &key, const std::function<void(const std::vector<std::string> &)> &fct) { m_Commands[key] = fct; }
	template <typename T>
	void AddCommand(const std::string &key, T *obj, void (T:: *fct)(const std::vector<std::string> &)) { m_Commands[key] = std::bind(std::mem_fn(fct), obj, std::placeholders::_1); }
	void AddCommand(const std::string &key, const std::function<void()> &fct) { m_EmptyCommands[key] = fct; }
	template <typename T>
	void AddCommand(const std::string &key, T *obj, void (T:: *fct)()) { m_EmptyCommands[key] = std::bind(std::mem_fn(fct), obj); }
	void AddSubCommand(const std::string &key, const CommandTreater &subCommand) { m_Commands[key] = std::bind(std::mem_fn(&CommandTreater::HandleSubCommand), this, std::placeholders::_1, key, subCommand); }
};