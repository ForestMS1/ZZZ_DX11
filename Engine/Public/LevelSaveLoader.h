#pragma once
#include "Collider.h"
NS_BEGIN(Engine)

struct TransformData
{
	Vec3 localScale;
	Vec3 localRotation;
	Vec3 localPosition;
	Matrix matLocal;
	Matrix matWorld;
};

struct UIData
{
	//Save 할 때 저장이 된건지 여부
	bool isSave = false;
	float x = 0.f;
	float y = 0.f;
	float width = 0.f;
	float height = 0.f;
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

struct GameObjectData
{
	char className[64];
	wchar_t objectName[64];
	UUID objectId; // 본인의 id
	UUID parentId; // 부모가 없으면 -1
	TransformData transformData;
	UIData uiData;
	ColliderData colliderData;
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
