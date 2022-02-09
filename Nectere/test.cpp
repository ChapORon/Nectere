#include <iostream>
#include "Dp/Json.hpp"
#include "Dp/Serializer.hpp"
#include "Dp/Xml.hpp"

using namespace Nectere;

struct MyObj
{
	int a;
	int b;
};

NECTERE_DP_SERIALIZER_DEFINITION(MyObj);

NECTERE_DP_SERIALIZER_TONODE(MyObj)
{
	Dp::Node newNode;
	newNode.Add("a", value.a);
	newNode.Add("b", value.b);
	return newNode;
}

NECTERE_DP_SERIALIZER_FROMNODE(MyObj)
{
	return MyObj{ node.Get<int>("a"), node.Get<int>("b") };
}

int main(int argc, char **argv)
{
	{
		std::cout << "==================================================" << std::endl;
		std::vector<int> ret = { 12, 5, 8, 65 };
		std::vector<int> ret1 = { 9 };
		std::vector<int> empty;
		Dp::Node root;
		root.Insert("test", ret);
		root.Insert("test1", ret1);
		root.Insert("test2", 3);
		root.Insert("empty", empty);
		std::cout << Dp::Json::Str(root) << std::endl;
		root.Replace("test2", ret);
		root.Merge("test", ret);
		std::cout << Dp::Json::Str(root) << std::endl;
	}

	{
		std::cout << std::endl;
		std::cout << "==================================================" << std::endl;
		Dp::Node root("test");
		MyObj obj{ 12, 3 };
		root.Insert("my", obj);
		std::cout << Dp::Json::Str(root) << std::endl;
		std::cout << Dp::Xml::Str(root) << std::endl;
		MyObj ret = root.Get<MyObj>("my");
		std::cout << "MyObj{ a=" << ret.a << ", b=" << ret.b << '}' << std::endl;
	}

	{
		std::cout << std::endl;
		std::cout << "==================================================" << std::endl;
		std::vector<int> ret = { 12, 5, 8, 65 };
		Dp::Node root;
		root.Add("test", ret);
		root.Add("test", ret);
		std::cout << Dp::Json::Str(root) << std::endl;
		std::cout << Dp::Xml::Str(root) << std::endl;
	}

	return 0;
}