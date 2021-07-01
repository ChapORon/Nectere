#pragma once

namespace Nectere
{
	namespace Network
	{
		struct Header
		{
			uint16_t applicationID;
			uint16_t messageType;
			uint16_t apiVersion;
			uint64_t messageLength;
		};
	}
}