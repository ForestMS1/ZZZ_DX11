#include "pch.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
SpriteRenderer::SpriteRenderer(shared_ptr<Shader> shader)
	: Component(ComponentType::SpriteRenderer)
	, _shader(shader)
{
	_textureEffectBuffer = _shader->GetSRV("sprite");
}

SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::Update()
{
	GAME.Add_RenderObject(_renderGroup, GetGameObject());

	if (_textures[_curTextureIndex])
		_textureEffectBuffer->SetResource(_textures[_curTextureIndex]->GetComPtr().Get());
}

HRESULT SpriteRenderer::Render()
{
	if (_mesh == nullptr)
		return E_FAIL;

	//GlobalData
	_shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
	auto world = GetTransform()->GetWorldMatrix();
	_shader->PushTransformData(TransformDesc{ world });


	//IA
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_mesh->GetVertexBuffer()->PushData();
	_mesh->GetIndexBuffer()->PushData();

	_shader->DrawIndexed(_techniqueIndex, _pass, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
	return S_OK;
}

void SpriteRenderer::OnInspectorGUI()
{
}
