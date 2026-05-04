#pragma once
enum class GameDataType
{
	PLAYER_POSITION,
	ANIMATION_STATE,
	HEARTBEAT,
	BROADCAST_MESSAGE,
	END
};

struct PlayerPosition
{
	uint32_t playerId;
	float x, y, z;
	float rotX, rotY, rotZ;
	uint32_t timestamp;
};

struct GamePacket
{
	GameDataType type;
	uint32_t sequenceNumber;
	uint32_t dataSize;
};