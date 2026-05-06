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
	float scaleX, scaleY, scaleZ;
	float rotX, rotY, rotZ;
	float posX, posY, posZ;
	uint32_t timestamp;
};

// 애니메이션 파라미터 개별 데이터
struct AnimParamData
{
	uint32_t paramHash; // FNV-1a로 변환된 파라미터 이름 해시
	float value;        // bool은 0.0f/1.0f로, float은 그대로 사용
};

struct AnimationData
{
	// 현재 재생 중인 애니메이션 정보
	int32_t animIndex;
	uint32_t currFrame; // 현재 몇 번째 프레임인지 (중요!)

	uint16_t paramCount; // 이 캐릭터가 가진 전체 파라미터 개수
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