#include <iostream>
#include "UIDSet.hpp"

using namespace Nectere;

class ID
{
private:
	uint16_t m_ID;

public:
	ID(uint16_t id): m_ID(id) {}
	uint16_t GetID() const { return m_ID; }
	~ID() { std::cout << "Deleting " << m_ID << std::endl; }
};

int main(int argc, char **argv)
{
	UIDSet<ID> uidVector;
	uidVector.Add(new ID(12));
	uidVector.Add(new ID(86));
	uidVector.Add(new ID(209));
	uidVector.Add(new ID(105));
	uidVector.Add(new ID(8));
	uidVector.Add(new ID(63));
	for (const ID *id : uidVector)
		std::cout << id->GetID() << std::endl;
	std::cout << "==================================================" << std::endl;
	uidVector.Remove(86);
	for (const ID *id : uidVector)
		std::cout << id->GetID() << std::endl;
}