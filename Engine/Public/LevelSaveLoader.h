#pragma once

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
	float x = 0.f;
	float y = 0.f;
	float width = 0.f;
	float height = 0.f;
};

struct GameObjectData
{
	char className[64];
	wchar_t objectName[64];
	UUID objectId; // 본인의 id
	UUID parentId; // 부모가 없으면 -1
	TransformData transformData;
	UIData uiData;
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
