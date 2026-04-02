#pragma once
#include "ResourceBase.h"

NS_BEGIN(Engine)

struct ModelBone;
struct ModelMesh;
//struct ModelAnimation;

class ENGINE_DLL Model : public enable_shared_from_this<Model>
{
public:
	Model();
	~Model();



private:
	wstring _modelPath = L"../../Resources/Models/";
	wstring _texturePath = L"../../Resources/Textures/";

private:
	shared_ptr<ModelBone> _root;
	vector<shared_ptr<Material>> _materials;
	vector<shared_ptr<ModelBone>> _bones;
	vector<shared_ptr<ModelMesh>> _meshes;
	//vector<shared_ptr<ModelAnimation>> _animations;
};

NS_END