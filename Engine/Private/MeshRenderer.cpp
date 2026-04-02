#include "pch.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"

MeshRenderer::MeshRenderer() : Component(ComponentType::MeshRenderer)
{
}

MeshRenderer::~MeshRenderer()
{
}

//void MeshRenderer::Update()
//{
//	if (_mesh == nullptr || _material == nullptr)
//		return;
//
//	auto shader = _material->GetShader();
//	if (shader == nullptr)
//		return;
//
//	_material->Update();
//
//	// GlobalData
//	shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
//
//	
//	// IA
//	_mesh->GetVertexBuffer()->PushData();
//	_mesh->GetIndexBuffer()->PushData();
//
//	shader->DrawIndexedInstanced(0, _pass, _mesh->GetIndexBuffer()->GetCount(), buffer->GetCount());
//}
