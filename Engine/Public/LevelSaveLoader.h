#pragma once
#include "Collider.h"
#include "Light.h"
NS_BEGIN(Engine)

struct TransformData
{
	Vec3 localScale;
	Vec3 localRotation;
	Vec3 localPosition;
	Matrix matLocal;
	Matrix matWorld;
};

struct LightData
{
	bool isSave = false;

	LIGHTTYPE type;
	LightDesc desc;
};

struct MonoBehaviourData
{
	bool isSave = false;

	// 프로토타입 이름만 저장
	// 스크립트 컴포넌트 개수
	uint8 scriptCount = 0;
	// 최대 32개 스크립트 프로토타입 이름 (64자 제한)
	wchar_t scriptPrototypeNames[32][64];

};
// 리소스 이름 = 리소스매니저 map 키값!!

// 메쉬 렌더러 컴포넌트
struct MeshRenderData
{
	bool isSave = false;
	// 메쉬 이름
	wchar_t meshName[64];
	// 머테리얼 이름
	wchar_t materialName[64];

	uint8 techniqueIndex = 0;
	uint8 pass = 0;
};

// 모델 애니메이터에서 사용
struct ModelAnimData
{
	//Save 할 때 저장이 된건지 여부
	bool isSave = false;
	// 쉐이더 이름
	wchar_t shaderName[64];
	// 모델 이름
	wchar_t modelName[64];
	// fsm 이름
	wchar_t animFSMFileName[64];

	uint8 techniqueIndex = 0;
	uint8 pass = 0;
	RENDERGROUP renderGroup = RENDERGROUP::PRIORITY; // 따로 설정 안했으면 PRIORITY
};

// 모델렌더러에서 사용
struct ModelRenderData
{
	//Save 할 때 저장이 된건지 여부
	bool isSave = false;
	// 쉐이더 이름
	wchar_t shaderName[64];
	// 모델 이름
	wchar_t modelName[64];

	uint8 techniqueIndex = 0;
	uint8 pass = 0;
	RENDERGROUP _renderGroup = RENDERGROUP::PRIORITY; // 따로 설정 안했으면 PRIORITY
};

struct UIData
{
	//Save 할 때 저장이 된건지 여부
	bool isSave = false;
	float x = 0.f;
	float y = 0.f;
	float width = 0.f;
	float height = 0.f;

	// 쉐이더 이름
	wchar_t shaderName[64];
	// 텍스쳐 장 수
	uint8 textureCount = 0;
	// 최대 256장 텍스쳐 이름 (128자 제한)
	wchar_t textureNames[256][128];
	// 다음 Texture로 넘어가는 스피드 값
	float speed = 1.f;
	// 루프 여부
	bool loop = false;
	// 재생 여부
	bool play = true;
};


struct ColliderData
{
	bool isSave = false;
	ColliderType type = ColliderType::END;
	bool isTrigger = false;
	bool isFix = true;
	Vec3 extents = { 0.f, 0.f, 0.f };
	Vec3 offset = { 0.f, 0.f, 0.f };
};

struct newGameObjectData
{
	char className[64];
	wchar_t objectName[64];
	UUID objectId; // 본인의 id
	UUID parentId; // 부모가 없으면 -1
	TransformData transformData;
	UIData uiData;
	ColliderData colliderData;
	ModelRenderData modelRenderData;
	ModelAnimData modelAnimData;
	MonoBehaviourData scriptData;
	MeshRenderData meshRenderData;
	LightData lightData;
};

struct GameObjectData
{
	char className[64];
	wchar_t objectName[64];
	UUID objectId; // 본인의 id
	UUID parentId; // 부모가 없으면 -1
	TransformData transformData;
	UIData uiData;
	ColliderData colliderData;
	ModelRenderData modelRenderData;
	ModelAnimData modelAnimData;
	MonoBehaviourData scriptData;
	MeshRenderData meshRenderData;
};


class ENGINE_DLL LevelSaveLoader
{
public:
	LevelSaveLoader();
	~LevelSaveLoader();

public:
	void Save(uint32 iLevelIndex, const wstring& strLayerTag);
	void Load(uint32 iLevelIndex, const wstring& strLayerTag);

private:

};

NS_END
