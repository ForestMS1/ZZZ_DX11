#include "pch.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
SpriteRenderer::SpriteRenderer(shared_ptr<Shader> shader)
	: Component(ComponentType::SpriteRenderer)
	, _shader(shader)
{
	_textureEffectBuffer = _shader->GetSRV("DiffuseMap");
}

SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::Update()
{
	GAME.Add_RenderObject(_renderGroup, GetGameObject());
	if (_mesh == nullptr)
		_mesh = GAME.GetResource<Mesh>(L"Quad"); // 기본 DefaultMesh

	if(_play)
		_curTextureIndex += DT * _speed;

	if (_textures.size() <= (uint32)_curTextureIndex)
	{
		if (_loop)
			_curTextureIndex = 0.f;
		else
			_curTextureIndex = _textures.size() - 1;
	}
}

HRESULT SpriteRenderer::Render()
{
	if (_mesh == nullptr)
		return E_FAIL;

	//GlobalData
	_shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);
	auto world = GetTransform()->GetWorldMatrix();
	_shader->PushTransformData(TransformDesc{ world });

	if (_textures[(uint32)_curTextureIndex])
		_textureEffectBuffer->SetResource(_textures[(uint32)_curTextureIndex]->GetComPtr().Get());

	//IA
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_mesh->GetVertexBuffer()->PushData();
	_mesh->GetIndexBuffer()->PushData();

	_shader->DrawIndexed(_techniqueIndex, _pass, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
	return S_OK;
}

void SpriteRenderer::OnInspectorGUI()
{
    if (ImGui::CollapsingHeader("SpriteRenderer", ImGuiTreeNodeFlags_DefaultOpen))
    {
		// Shader
		string shaderName = _shader ? Utils::ToString(_shader->GetName()) : "None";
		ImGui::Text("Shader: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), shaderName.c_str());

		ImGui::Separator();

        // Mesh
        string meshName = (_mesh != nullptr) ? Utils::ToString(_mesh->GetName()) : "None";
        ImGui::Text("Mesh: %s", meshName.c_str());

        ImGui::Separator();

        // Play / Loop
        ImGui::Checkbox("Play Animation", &_play);
        ImGui::Checkbox("Looping", &_loop);

        // Speed
        ImGui::DragFloat("Animation Speed", &_speed, 0.1f, 0.0f, 100.0f);

        // 현재 텍스처 인덱스 및 정보
        int maxIndex = max(0, (int)_textures.size() - 1);
        int curIdx = (int)_curTextureIndex;

        if (ImGui::SliderInt("Current Index", &curIdx, 0, maxIndex))
        {
            _curTextureIndex = (float)curIdx;
        }

        // 등록된 텍스처 리스트 보기
        if (ImGui::TreeNode("Texture List"))
        {
            ImGui::Text("Total Count: %d", (int)_textures.size());
            for (size_t i = 0; i < _textures.size(); ++i)
            {
                string label = Utils::ToString(_textures[i]->GetName());
                ImGui::BulletText("%s", label.c_str());
            }
            ImGui::TreePop();
        }

		uint8 renderGroupCount = static_cast<uint8>(RENDERGROUP::END);
		if (ImGui::BeginCombo("RenderGruop", Utils::ToString(RENDERGROUP_NAMES[static_cast<uint8>(_renderGroup)]).c_str()))
		{
			for (uint8 i = 0; i < renderGroupCount; ++i)
			{
				bool isSelected = (static_cast<uint8>(_renderGroup) == i);
				if (ImGui::Selectable(Utils::ToString(RENDERGROUP_NAMES[i]).c_str(), isSelected))
				{
					_renderGroup = static_cast<RENDERGROUP>(i);
				}
			}
			ImGui::EndCombo();
		}
    }
}
