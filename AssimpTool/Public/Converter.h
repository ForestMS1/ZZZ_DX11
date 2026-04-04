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
	void ExportAnimationData(const wstring& savePath, uint32 index = 0);
	void ExportAnimationDataAll(const wstring& savePath);

private:
	void ReadModelData(aiNode* node, int32 index, int32 parent);
	void ReadMeshData(aiNode* node, int32 bone);
	void ReadSkinData();
	void WriteModeFile(const wstring& finalPath);

private:
	void ReadMaterialData();
	void WriteMaterialData(wstring finalPath);
	string WriteTexture(string saveFolder, string file);

private:
	shared_ptr<asAnimation> ReadAnimationData(aiAnimation* srcAnimation);
	shared_ptr<asAnimationNode> ParseAnimationNode(shared_ptr<asAnimation> animation, aiNodeAnim* srcNode);
	void ReadKeyframeData(shared_ptr<asAnimation> animation, aiNode* srcNode, map<string, shared_ptr<asAnimationNode>>& cache);
	void WriteAnimationData(shared_ptr<asAnimation> animation, wstring finalPath);

private:
	uint32 GetBoneIndex(const string& name);
	void ReadAllMeshes();

	Matrix GetMeshTransform(uint32 meshIndex);
	Matrix FindMeshTransform(aiNode* node, uint32 meshIndex, Matrix parent);


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
