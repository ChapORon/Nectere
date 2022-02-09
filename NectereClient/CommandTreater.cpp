#include "CommandTreater.hpp"

#include <iostream>
#include <sstream>

std::vector<std::string> CommandTreater::SplitCommand(const std::string &line) const
{
	std::vector<std::string> args;
	std::stringstream buffer;
	bool inQuote = false;
	bool isEscaped = false;
	for (char c : line)
	{
		if (isEscaped)
		{
			switch (c)
			{
			case 'a': buffer << '\a'; break;
			case 'b': buffer << '\b'; break;
			case 't': buffer << '\t'; break;
			case 'n': buffer << '\n'; break;
			case 'v': buffer << '\v'; break;
			case 'f': buffer << '\f'; break;
			case 'r': buffer << '\r'; break;
			case '"': buffer << '"'; break;
			case '\\': buffer << '\\'; break;
			default: buffer << c; break;
			}
			isEscaped = false;
		}
		else if (c == '\\')
			isEscaped = true;
		else if (c == '"')
		{
			inQuote = !inQuote;
			if (!inQuote)
			{
				std::string tmp = buffer.str();
				if (!tmp.empty())
					args.emplace_back(tmp);
				buffer.str("");
			}
		}
		else if (c == ' ' && !inQuote)
		{
			std::string tmp = buffer.str();
			if (!tmp.empty())
				args.emplace_back(tmp);
			buffer.str("");
		}
		else
			buffer << c;
	}
	std::string tmp = buffer.str();
	if (!tmp.empty())
		args.emplace_back(tmp);
	return args;
}

void CommandTreater::HandleSubCommand(const std::vector<std::string> &args, const std::string &command, const CommandTreater &subCommandTreater)
{
	if (!args.empty())
		subCommandTreater.TreatCommand(args);
	else
		std::cerr << "Error: Not enough argument: " << command << std::endl;
}

void CommandTreater::TreatCommand(const std::vector<std::string> &args) const
{
	if (!args.empty())
	{
		auto command = m_Commands.find(args[0]);
		if (command != m_Commands.end())
		{
			std::vector<std::string> argCopy = args;
			argCopy.erase(argCopy.begin());
			command->second(argCopy);
		}
		else
		{
			auto emptyCommand = m_EmptyCommands.find(args[0]);
			if (emptyCommand != m_EmptyCommands.end())
			{
				if (args.size() == 1)
					emptyCommand->second();
				else
					std::cerr << "Error: Too much argument: " << args[0] << std::endl;
			}
			else
				std::cerr << args[0] << ": Command does not exist" << std::endl;
		}
	}
}