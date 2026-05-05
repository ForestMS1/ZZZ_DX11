#pragma once

enum class GameDataType
{
	OBJECT_SYNC,
	HEARTBEAT,
	BROADCAST_MESSAGE,
	END
};

namespace SyncFlag {
	const uint32_t POSITION = 1 << 0;	// 0x01
	const uint32_t ANIMATION = 1 << 1;	// 0x02
	const uint32_t STATUS = 1 << 2;		// 0x04 (HP, MP 등)
}


#pragma pack(push, 1) // 1바이트 단위로 딱 붙임

struct PlayerPosition
{
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Vector3 rotation;
	DirectX::SimpleMath::Vector3 position;
	uint32_t timestamp;
};

struct GamePacket
{
	GameDataType type;
	uint32_t objectId;
	uint32_t sequenceNumber;
	uint32_t syncFlags;     // 어떤 데이터가 뒤에 붙어있는지 (1: Pos, 2: Anim, 4: HP 등)
	uint32_t dataSize;
};

#pragma pack(pop)