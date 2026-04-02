#include "pch.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

MeshRenderer::MeshRenderer() : Component(ComponentType::MeshRenderer)
{
}

MeshRenderer::~MeshRenderer()
{
}

HRESULT MeshRenderer::Render()
{
	if (_mesh == nullptr || _material == nullptr)
		return E_FAIL;

	auto shader = _material->GetShader();
	if (shader == nullptr)
		return E_FAIL;

	_material->Update();

	// GlobalData
	shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
	auto world = GetTransform()->GetWorldMatrix();
	shader->PushTransformData(TransformDesc{ world });
	
	// IA
	_mesh->GetVertexBuffer()->PushData();
	_mesh->GetIndexBuffer()->PushData();


	shader->DrawIndexed(0, _pass, _mesh->GetIndexBuffer()->GetCount(), 0, 0);

	return S_OK;
}