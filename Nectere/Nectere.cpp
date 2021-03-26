#include <boost/regex.hpp>
#include <iostream>
#include <Dp/Json.hpp>
#include <Dp/Serializer.hpp>
#include <Dp/Xml.hpp>
#include "Discord/Discord.h"
#include "Twitch/Twitch.h"

struct Obj
{
	std::string name;
	bool isInit;
	int value;
};

std::ostream &operator<<(std::ostream &os, const Obj &obj)
{
	return os << "{name: \"" << obj.name << "\", isInit: " << (obj.isInit ? "true" : "false") << ", value: " << obj.value << '}';
}

namespace Dp
{
	template <>
	struct Serializer<Obj>
	{
		Node ToNode(const Obj &obj)
		{
			Node node;
			node.Add("name", obj.name);
			node.Add("isInit", obj.isInit);
			node.Add("value", obj.value);
			return node;
		}

		Obj FromNode(const Node &node)
		{
			return Obj{ node.Get<const std::string &>("name"), node.Get<bool>("isInit") , node.Get<int>("value") };
		}
	};
}

int main()
{
	std::string line = "Subject: Re: Will Success Spoil Rock Hunter?";
	boost::regex pat("^Subject: (Re: |Aw: )*(.*)");

	boost::smatch matches;
	if (boost::regex_match(line, matches, pat))
		std::cout << matches[2] << std::endl;


	Obj obj{ "test", true, 42 };
	Dp::Node head("head");
	head.Add("test.array", 1);
	head.Add("test.array", 2);
	head.Add("test.array", 3);
	head.Remove("test.array(3)");
	head.Add("test.value", 42);
	head.Add("test.obj", obj);
	std::cout << "In: " << obj << std::endl;
	std::cout << "Out: " << head.Get<Obj>("test.obj") << std::endl;
	//std::string expected = "{\"head\":{\"test\":{\"value\":[1,2]}}}";
	std::cout << Dp::Json::Str(head, 0, 0, false) << std::endl;
	std::cout << Dp::Xml::Str(head) << std::endl;
	Dp::Node xml = Dp::Xml::LoadFromFile("D:/Programmation/Nectere/DataParser/test/xml/dp_small.xml");
	std::cout << Dp::Json::Str(xml) << std::endl;
	std::cout << Dp::Xml::Str(xml) << std::endl;
	return 0;
}
