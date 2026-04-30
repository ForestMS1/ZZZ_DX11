#include "pch.h"
#include "BillBoard.h"
#include "Transform.h"
#include "Camera.h"

BillBoard::BillBoard()
	: Component(ComponentType::BillBoard)
{
	int32 vertexCount = MAX_BILLBOARD_COUNT * 4; // 사각형
	int32 indexCount = MAX_BILLBOARD_COUNT * 6; // 사각형 1개는 인덱스 6개

	_vertices.resize(vertexCount);
	_vertexBuffer = make_shared<VertexBuffer>();
	_vertexBuffer->Create<VertexBillBoard>(_vertices, 0, true);

	_indices.resize(indexCount);

	for (int32 i = 0; i < MAX_BILLBOARD_COUNT; ++i)
	{
		_indices[i * 6 + 0] = i * 4 + 0;
		_indices[i * 6 + 1] = i * 4 + 1;
		_indices[i * 6 + 2] = i * 4 + 2;
		_indices[i * 6 + 3] = i * 4 + 2;
		_indices[i * 6 + 4] = i * 4 + 1;
		_indices[i * 6 + 5] = i * 4 + 3;
	}

	_indexBuffer = make_shared<IndexBuffer>();
	_indexBuffer->Create(_indices);
}

BillBoard::~BillBoard()
{
}

void BillBoard::Update()
{
	GAME.Add_RenderObject(RENDERGROUP::NONBLEND, GetGameObject());
}

HRESULT BillBoard::Render()
{
	if (_drawCount != _prevCount)
	{
		_prevCount = _drawCount;

		D3D11_MAPPED_SUBRESOURCE subResource;
		CONTEXT->Map(_vertexBuffer->GetComPtr().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			memcpy(subResource.pData, _vertices.data(), sizeof(VertexBillBoard) * _vertices.size());
		}
		CONTEXT->Unmap(_vertexBuffer->GetComPtr().Get(), 0);
	}

	if (_material == nullptr)
		return E_FAIL;

	auto shader = _material->GetShader();

	//Transform
	auto world = GetTransform()->GetWorldMatrix();
	shader->PushTransformData(TransformDesc{ world });

	//GlobalData
	shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);

	_material->Update();


	_vertexBuffer->PushData();
	_indexBuffer->PushData();

	shader->DrawIndexed(_technique, _pass, _drawCount * 6);

	return S_OK;
}

void BillBoard::Add(Vec3 position, Vec2 scale)
{
	_vertices[_drawCount * 4 + 0].position = position;
	_vertices[_drawCount * 4 + 1].position = position;
	_vertices[_drawCount * 4 + 2].position = position;
	_vertices[_drawCount * 4 + 3].position = position;

	_vertices[_drawCount * 4 + 0].uv = Vec2(0, 1);
	_vertices[_drawCount * 4 + 1].uv = Vec2(0, 0);
	_vertices[_drawCount * 4 + 2].uv = Vec2(1, 1);
	_vertices[_drawCount * 4 + 3].uv = Vec2(1, 0);

	_vertices[_drawCount * 4 + 0].scale = scale;
	_vertices[_drawCount * 4 + 1].scale = scale;
	_vertices[_drawCount * 4 + 2].scale = scale;
	_vertices[_drawCount * 4 + 3].scale = scale;

	_drawCount++;
}

void BillBoard::OnInspectorGUI()
{
	GuiRemoveButton("BillBoard");
	if (ImGui::CollapsingHeader("BillBoard", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Material 변경 버튼
		if (ImGui::Button("Change Material##Material", ImVec2(-1, 0))) // 너비를 가득 채우려면 ImVec2(-1, 0)
		{
			ImGui::OpenPopup("MaterialResourceSearchPopup");
		}

		// 팝업 내부 로직
		if (ImGui::BeginPopup("MaterialResourceSearchPopup"))
		{
			ImGui::TextDisabled("Materials");
			ImGui::Separator();

			const auto& resources = GAME.GetResourceArray();

			for (const auto& pair : resources[static_cast<uint8>(ResourceType::MATERIAL)])
			{
				const auto& shader = pair.second;
				if (ImGui::Selectable(Utils::ToString(shader->GetName()).c_str()))
				{
					_material = static_pointer_cast<Material>(shader);
				}
			}
			ImGui::EndPopup();
		}

		if(ImGui::Button("AddBillBoard"))
		{
			for (int32 i = 0; i < 10; i++)
			{
				Vec2 scale = Vec2(1 + rand() % 3, 1 + rand() % 3);
				Vec2 position = Vec2(rand() % 3, -3 + rand() % 3);

				Add(Vec3(position.x, scale.y * 0.5f, position.y), scale);
			}
		}
	}
	ImGui::Separator();
}
