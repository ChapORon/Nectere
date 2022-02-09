#pragma once

namespace Nectere
{
	namespace Network
	{
		struct Header //24 bytes
		{
			uint8_t error;
			uint16_t canalID;
			uint16_t applicationID;
			uint16_t userID;
			uint16_t messageType;
			uint16_t apiVersion;
			uint64_t messageLength;
		};
	}
}