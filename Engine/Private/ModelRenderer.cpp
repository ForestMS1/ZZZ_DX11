#include "pch.h"
#include "ModelRenderer.h"
#include "Shader.h"
#include "Model.h"
#include "Material.h"
#include "Camera.h"
#include "ModelMesh.h"
#include "Transform.h"

ModelRenderer::ModelRenderer(shared_ptr<Shader> shader)
	: Component(ComponentType::ModelRenderer)
	, _shader(shader)
{
}

ModelRenderer::~ModelRenderer()
{
}



HRESULT ModelRenderer::Render()
{
	if (_model == nullptr)
		return E_FAIL;

	//GlobalData
	_shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
	auto world = GetTransform()->GetWorldMatrix();
	_shader->PushTransformData(TransformDesc{ world });

	// TODO : Light
	//auto lightObj = SCENE->GetCurrentScene()->GetLight();
	//if (lightObj)
	//	_shader->PushLightData(lightObj->GetLight()->GetLightDesc());

	//Bones
	BoneDesc boneDesc;

	const uint32 boneCount = _model->GetBoneCount();
	for (uint32 i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
		boneDesc.transforms[i] = bone->transform;
	}
	_shader->PushBoneData(boneDesc);

	const auto& meshes = _model->GetMeshes();
	for (auto& mesh : meshes)
	{
		if (mesh->material)
			mesh->material->Update();

		//BoneIndex
		_shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		//IA
		mesh->vertexBuffer->PushData();
		mesh->indexBuffer->PushData();

		_shader->DrawIndexed(0, _pass, mesh->indexBuffer->GetCount(), 0, 0);
	}

	return S_OK;
}

void ModelRenderer::SetModel(shared_ptr<Model> model)
{
	_model = model;

	const auto& materials = _model->GetMaterials();
	for (auto& material : materials)
	{
		material->SetShader(_shader);
	}
}
