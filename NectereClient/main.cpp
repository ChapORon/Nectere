#include <iostream>
#include <sstream>
#include "Connection.hpp"

int main()
{
		NectereClient::Connection connection;
		std::string line;
		do 
		{
			if (!line.empty())
			{
				std::vector<std::string> args;
				std::stringstream buffer;
				bool inQuote = false;
				for (char c : line)
				{
					if (c == '"')
					{
						inQuote = !inQuote;
						if (!inQuote)
						{
							args.emplace_back(buffer.str());
							buffer.str("");
						}
					}
					else if (c == ' ' && !inQuote)
					{
						args.emplace_back(buffer.str());
						buffer.str("");
					}
					else
						buffer << c;
				}
				args.emplace_back(buffer.str());
				connection.TreatLine(args);
			}
			if (connection.IsRunning())
				std::cout << "$> ";
		} while (connection.IsRunning() && std::getline(std::cin, line));
	return 0;
}
