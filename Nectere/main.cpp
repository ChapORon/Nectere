#include "Server.hpp"

#include <iostream>

class MyIDObject
{
private:
	uint16_t id;

public:
	MyIDObject(uint16_t id): id(id) {}
	uint16_t GetID() const { return id; }
};

static void DumpUIDVector(const Nectere::UIDVector<MyIDObject> &vec)
{
	std::cout << '[';
	unsigned int n = 0;
	for (const auto &element : vec.GetElements())
	{
		if (n != 0)
			std::cout << ", ";
		std::cout << element->GetID();
		++n;
	}
	std::cout << ']' << std::endl;
}

int main()
{
 	Nectere::Server server(4242);
 	server.Start();
	return 0;
}
