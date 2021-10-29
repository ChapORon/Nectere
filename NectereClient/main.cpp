#include <iostream>
#include "NecterePrompt.hpp"

static void HandleClear()
{
	::system("CLS");
}

static void HandlePrint(const std::vector<std::string> &args)
{
	for (unsigned int n = 1; n < args.size(); ++n)
		std::cout << args[n] << std::endl;
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
