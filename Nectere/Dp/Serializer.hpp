#pragma once

#include <string>
#include <vector>

#define NECTERE_DP_SERIALIZER_DEFINITION(Type) template <> struct Nectere::Dp::Serializer<Type>\
{\
	Nectere::Dp::Node ToNode(const Type &value);\
	Type FromNode(const Nectere::Dp::Node &node);\
}
#define NECTERE_DP_SERIALIZER_DEFINITION_TONODE(Type) template <> struct Nectere::Dp::Serializer<Type>\
{\
	Nectere::Dp::Node ToNode(const Type &value);\
	Type FromNode(const Nectere::Dp::Node &node) = delete;\
}
#define NECTERE_DP_SERIALIZER_DEFINITION_FROMNODE(Type) template <> struct Nectere::Dp::Serializer<Type>\
{\
	Nectere::Dp::Node ToNode(const Type &value) = delete;\
	Type FromNode(const Nectere::Dp::Node &node);\
}

#define NECTERE_DP_SERIALIZER_TONODE(Type) Nectere::Dp::Node Nectere::Dp::Serializer<Type>::ToNode(const Type &value)
#define NECTERE_DP_SERIALIZER_FROMNODE(Type) Type Nectere::Dp::Serializer<Type>::FromNode(const Nectere::Dp::Node &node)

namespace Nectere
{
	namespace Dp
	{
		class Node;
		template <typename t_TypeToSerialize>
		struct Serializer
		{
			Node ToNode(const t_TypeToSerialize &) = delete;
			t_TypeToSerialize FromNode(const Node &) = delete;
		};
	}
}

NECTERE_DP_SERIALIZER_DEFINITION(bool);
NECTERE_DP_SERIALIZER_DEFINITION(char);
NECTERE_DP_SERIALIZER_DEFINITION(unsigned char);
NECTERE_DP_SERIALIZER_DEFINITION(int);
NECTERE_DP_SERIALIZER_DEFINITION(unsigned int);
NECTERE_DP_SERIALIZER_DEFINITION(float);
NECTERE_DP_SERIALIZER_DEFINITION(double);
NECTERE_DP_SERIALIZER_DEFINITION(long);
NECTERE_DP_SERIALIZER_DEFINITION(unsigned long);
NECTERE_DP_SERIALIZER_DEFINITION(long long);
NECTERE_DP_SERIALIZER_DEFINITION(unsigned long long);
NECTERE_DP_SERIALIZER_DEFINITION(std::string);