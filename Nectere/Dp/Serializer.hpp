#pragma once

namespace Nectere
{
	namespace Dp
	{
		class Node;
		template <typename T>
		struct Serializer
		{
			Node ToNode(const T &) = delete;
			T FromNode(const Node &) = delete;
		};
	}
}