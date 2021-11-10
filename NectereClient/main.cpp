#include <iostream>
#include "NecterePrompt.hpp"

static void HandleClear()
{
	::system("CLS");
}

static void HandlePrint(const std::vector<std::string> &args)
{
	for (const auto &arg : args)
		std::cout << arg << std::endl;
}

int main()
{
	NecterePrompt prompt;
	prompt.AddCommand("quit", [&prompt]() mutable { prompt.Stop(); });
	prompt.AddCommand("clear", &HandleClear);
	prompt.AddCommand("print", &HandlePrint);
	prompt.Run();
	return 0;
}
