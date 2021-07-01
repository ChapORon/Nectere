#pragma once

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