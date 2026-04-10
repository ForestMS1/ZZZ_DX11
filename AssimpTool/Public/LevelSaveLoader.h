#pragma once

NS_BEGIN(AssimpTool)

struct TransformData
{
	Vec3 localScale;
	Vec3 localRotation;
	Vec3 localPosition;
	Matrix matLocal;
	Matrix matWorld;
};

struct GameObjectData
{
	char className[64];
	wchar_t objectName[64];
	UUID objectId; // 본인의 id
	UUID parentId; // 부모가 없으면 -1
	TransformData transformData;
};


class LevelSaveLoader
{
public:
	LevelSaveLoader();
	~LevelSaveLoader();

public:
	void Save(LEVEL eLevel, const wstring& strLayerTag);
	void Load(LEVEL eLevel, const wstring& strLayerTag);

private:

};

NS_END
