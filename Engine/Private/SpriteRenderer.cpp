#include "pch.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture.h"
#include "GameObject.h"

SpriteRenderer::SpriteRenderer()
	: Component(ComponentType::SpriteRenderer)
{
}

SpriteRenderer::SpriteRenderer(shared_ptr<Shader> shader)
	: Component(ComponentType::SpriteRenderer)
	, _shader(shader)
{
	_textureEffectBuffer = _shader->GetSRV("DiffuseMap");

	_viewX = GAME.GetEngineDesc().iWinSizeX;
	_viewY = GAME.GetEngineDesc().iWinSizeY;
	_x = _viewX * 0.5f;
	_y = _viewY * 0.5f;
	_width = _viewX * 0.5f;
	_height = _viewY * 0.5f;
	_uiProj = Matrix::CreateOrthographic(_viewX, _viewY, 0.f, 1.f);
}

SpriteRenderer::~SpriteRenderer()
{

}

void SpriteRenderer::Awake()
{

}

void SpriteRenderer::Update()
{
	if (_mesh == nullptr)
		_mesh = GAME.GetResource<Mesh>(L"Quad"); // 기본 DefaultMesh

	if (_textures.size() <= 0)
		return;

	if(_play)
		_curTextureIndex += DT * _speed;

	if (_textures.size() <= (uint32)_curTextureIndex)
	{
		if (_loop)
			_curTextureIndex = 0.f;
		else
			_curTextureIndex = _textures.size() - 1;
	}

	if (_ui)
	{
		auto transform = GetGameObject()->GetTransform();

		transform->SetLocalScale(Vec3(_width, _height, 1.f));
		transform->SetLocalPosition(Vec3(_x - _viewX * 0.5f, -_y + _viewY * 0.5f, 0.f));

		GAME.Add_RenderObject(RENDERGROUP::UI, GetGameObject());
	}
	else
		GAME.Add_RenderObject(_renderGroup, GetGameObject());
}

HRESULT SpriteRenderer::Render()
{
	if (_mesh == nullptr)
		return E_FAIL;
	if (_shader == nullptr)
		return E_FAIL;

	//GlobalData
	if (_ui)
		_shader->PushGlobalData(Matrix::Identity, _uiProj);
	else
		_shader->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);

	auto world = GetTransform()->GetWorldMatrix();
	_shader->PushTransformData(TransformDesc{ world });

	if(_textures[(uint32)_curTextureIndex] != nullptr)
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
	GuiRemoveButton("SpriteRenderer");

    if (ImGui::CollapsingHeader("SpriteRenderer", ImGuiTreeNodeFlags_DefaultOpen))
    {
		// Shader 변경 버튼
		if (ImGui::Button("Change Shader##Shader", ImVec2(-1, 0))) // 너비를 가득 채우려면 ImVec2(-1, 0)
		{
			ImGui::OpenPopup("ShaderResourceSearchPopup");
		}

		// 팝업 내부 로직
		if (ImGui::BeginPopup("ShaderResourceSearchPopup"))
		{
			ImGui::TextDisabled("Shaders");
			ImGui::Separator();

			const auto& resources = GAME.GetResourceArray();

			for (const auto& pair : resources[static_cast<uint8>(ResourceType::SHADER)])
			{
				const auto& shader = pair.second;
				if (ImGui::Selectable(Utils::ToString(shader->GetName()).c_str()))
				{
					_shader = static_pointer_cast<Shader>(shader);
					_textureEffectBuffer = _shader->GetSRV("DiffuseMap");
					_viewX = GAME.GetEngineDesc().iWinSizeX;
					_viewY = GAME.GetEngineDesc().iWinSizeY;
					_x = _viewX * 0.5f;
					_y = _viewY * 0.5f;
					_width = _viewX * 0.5f;
					_height = _viewY * 0.5f;
					_uiProj = Matrix::CreateOrthographic(_viewX, _viewY, 0.f, 1.f);
				}
			}
			ImGui::EndPopup();
		}

		// Shader
		string shaderName = _shader ? Utils::ToString(_shader->GetName()) : "None";
		ImGui::Text("Shader: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), shaderName.c_str());

		ImGui::Separator();

		// --- Pass 선택 ---
		uint32 maxPassCount = _shader->GetPassCount(_techniqueIndex);
		int p = static_cast<int>(_pass);

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
		// 해당 Technique이 가진 Pass 개수 내에서만 조절 가능하게 제한
		if (ImGui::SliderInt("Pass", &p, 0, maxPassCount - 1))
		{
			_pass = static_cast<uint32>(p);
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		if (ImGui::Button("R##Pass")) _pass = 0;

		ImGui::Separator();

        // Mesh
        string meshName = (_mesh != nullptr) ? Utils::ToString(_mesh->GetName()) : "None";
        ImGui::Text("Mesh: %s", meshName.c_str());

        ImGui::Separator();

        // Play / Loop
        ImGui::Checkbox("Play Animation", &_play);
        ImGui::Checkbox("Looping", &_loop);
		ImGui::Checkbox("UIObject", &_ui);

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

		if (ImGui::BeginDragDropTarget())
		{
			// 모델 리소스 매니저로부터 드래그 온 경우
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_TEXTURE"))
			{
				const wstring& textureKey = *(const wstring*)payload->Data;
				_textures.push_back(GAME.GetResource<Texture>(textureKey));
			}
			ImGui::EndDragDropTarget();
		}


		// 렌더그룹 설정
		if (_ui)
		{
			// UI POS
			Vec2 uiPos(_x, _y);
			Vec2 uiScale(_width, _height);
			ImGui::DragFloat2("UI Pos X Y :", (float*)&uiPos, 5.f, 0.0f, max(_viewX, _viewY));
			_x = uiPos.x;
			_y = uiPos.y;
			ImGui::DragFloat2("UI Size :", (float*)&uiScale, 5.f, 0.0f, max(_viewX, _viewY));
			_width = uiScale.x;
			_height = uiScale.y;

			if (ImGui::BeginCombo("RenderGruop", Utils::ToString(RENDERGROUP_NAMES[static_cast<uint8>(RENDERGROUP::UI)]).c_str()))
			{
				ImGui::EndCombo();
			}
		}
		else
		{
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
}
