#pragma once

#include "AsTypes.h"

NS_BEGIN(AssimpTool)

class Converter
{
public:
	Converter();
	~Converter();

public:
	void ReadAssetFile(const wstring& file);
	void ExportModelData(const wstring& savePath);
	void ExportMaterialData(const wstring& savePath);


private:
	void ReadModelData(aiNode* node, int32 index, int32 parent);
	void ReadMeshData(aiNode* node, int32 bone);
	void ReadSkinData();
	void WriteModeFile(const wstring& finalPath);


private:
	wstring _assetPath = L"../../Resources/Assets/";
	wstring _modelPath = L"../../Resources/Models/";
	wstring _texturePath = L"../../Resources/Textures/";

private:
	shared_ptr<Assimp::Importer> _importer;
	const aiScene* _scene;

private:
	vector<shared_ptr<asBone>> _bones;
	vector<shared_ptr<asMesh>> _meshes;
	vector<shared_ptr<asMaterial>> _materials;
};

NS_END
