#include <iostream>
#include <Dp/Json.hpp>
#include <Dp/Xml.hpp>
#include "Discord/Discord.h"
#include "Twitch/Twitch.h"

int main()
{
	Dp::Dt::Node head("head");
	head.Add("test.array", Dp::Dt::Data(1));
	head.Add("test.array", Dp::Dt::Data(2));
	head.Add("test.array", Dp::Dt::Data(3));
	head.Remove("test.array(3)");
	head.Add("test.value", Dp::Dt::Data(42));
	//std::string expected = "{\"head\":{\"test\":{\"value\":[1,2]}}}";
	std::cout << Dp::Json::Str(head, 0, 0, false) << std::endl;
	std::cout << Dp::Xml::Str(head) << std::endl;
	return 0;
}
