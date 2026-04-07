#include "pch.h"
#include "Resource_Manager.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"
#include <filesystem>

Resource_Manager::Resource_Manager()
{
}

Resource_Manager::~Resource_Manager()
{
	/*for (auto& keyObjMap : _resources)
	{
		for (auto& pair : keyObjMap)
		{
			pair.second.reset();
		}
		keyObjMap.clear();
	}*/
}

HRESULT Resource_Manager::Initialize()
{
	_defaultMeshIcon = Resource_Manager::Load<Texture>(L"DefaultMeshIcon", L"..\\..\\Resources\\Textures\\zzz.png");
	CreateDefaultMesh();

	return S_OK;
}

shared_ptr<Texture> Resource_Manager::GetOrAddTexture(const wstring& key, const wstring& path)
{
	shared_ptr<Texture> texture = GetResource<Texture>(key);

	if (filesystem::exists(filesystem::path(path)) == false)
		return nullptr;

	texture = Resource_Manager::Load<Texture>(key, path);

	if (nullptr == texture)
	{
		texture = make_shared<Texture>();
		texture->Load(path);
		AddResource(key, texture);
	}

	return texture;
}

void Resource_Manager::ClearResource()
{
	for (auto& map : _resources)
	{
		for (auto& pair : map)
		{
			pair.second.reset();
		}
		map.clear();
	}
	CreateDefaultMesh();
}

void Resource_Manager::ShowResourceList()
{
	if(_defaultMeshIcon == nullptr)
		_defaultMeshIcon = Resource_Manager::Load<Texture>(L"DefaultMeshIcon", L"..\\..\\Resources\\Textures\\zzz.png");

	ImGui::Begin("ResourceWindow");
	if (ImGui::BeginTabBar("ResourceTabs")) {
		if (ImGui::BeginTabItem("Textures")) {

			auto& textureMap = _resources[static_cast<uint8>(ResourceType::TEXTURE)];

			float button_size = 64.0f;
			float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			ImGuiStyle& style = ImGui::GetStyle();

			int i = 0;
			for (auto& pair : textureMap)
			{
				if (pair.second == _defaultMeshIcon)
					continue;

				ImGui::PushID(i);

				// Texture 객체에서 SRV 추출
				shared_ptr<Texture> tex = static_pointer_cast<Texture>(pair.second);
				if (tex && tex->GetComPtr()) // GetComPtr()은 ID3D11ShaderResourceView*를 반환
				{
					ImTextureID texID = (ImTextureID)tex->GetComPtr().Get();

					// 그룹으로 묶어서 아이콘, 텍스트 출력
					ImGui::BeginGroup();

					if (ImGui::ImageButton("##icon", texID, ImVec2(button_size, button_size)))
					{
						// 클릭 시 로직
					}

					// 리소스 키(이름) 출력
					string keyStr(pair.first.begin(), pair.first.end());
					ImGui::TextUnformatted(keyStr.c_str());

					ImGui::EndGroup();
				}

				// 그리드 배치 계산
				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_size;

				// 다음 아이템이 창 크기를 넘지 않으면 옆으로 배치
				if (++i < textureMap.size() && next_button_x2 < window_visible_x2)
					ImGui::SameLine();

				ImGui::PopID();
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Meshes")) {
			auto& meshMap = _resources[static_cast<uint8>(ResourceType::MESH)];
			ImTextureID defaultID = (ImTextureID)_defaultMeshIcon->GetComPtr().Get();

			float button_size = 64.0f;
			float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			ImGuiStyle& style = ImGui::GetStyle();

			int i = 0;
			for (auto& pair : meshMap) {
				ImGui::PushID(i);
				ImGui::BeginGroup();

				// 메쉬는 실제 모양 대신 "박스 모양 아이콘" 등을 출력
				ImGui::ImageButton("##meshIcon", defaultID, ImVec2(64, 64));

				string name(pair.first.begin(), pair.first.end());
				ImGui::Text("%s", name.c_str());

				ImGui::EndGroup();

				// 그리드 배치 계산
				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_size;

				// 다음 아이템이 창 크기를 넘지 않으면 옆으로 배치
				if (++i < meshMap.size() && next_button_x2 < window_visible_x2)
					ImGui::SameLine();

				ImGui::PopID();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Models")) {
			auto& modelMap = _resources[static_cast<uint8>(ResourceType::MODEL)];

			// 모델용 아이콘이 따로 있다면 그것을 사용하고, 없다면 Mesh 아이콘을 공유
			ImTextureID modelIconID = (ImTextureID)_defaultMeshIcon->GetComPtr().Get();

			float button_size = 64.0f;
			float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			ImGuiStyle& style = ImGui::GetStyle();

			int i = 0;
			for (auto& pair : modelMap) {
				ImGui::PushID(i);
				ImGui::BeginGroup();

				// 1. 버튼
				ImGui::ImageButton("##modelIcon", modelIconID, ImVec2(button_size, button_size));

				// 2. 버튼 바로 다음에 드래그 소스 로직을 배치 (클릭 체크문 밖)
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
					// wstring 키값 전달
					const wstring& modelKey = pair.first;

					// 주의: wstring 객체 자체의 주소를 보낼 때는 드래그가 끝날 때까지 데이터가 유지되어야 함
					// 매니저의 맵에 들어있는 pair.first는 안전
					ImGui::SetDragDropPayload("CONTENT_BROWSER_MODEL", &modelKey, sizeof(wstring));

					// 드래그 중 마우스 커서 옆에 보일 미리보기
					string name(modelKey.begin(), modelKey.end());
					ImGui::Text("Adding Model: %s", name.c_str());

					ImGui::EndDragDropSource();
				}

				// 모델 이름 출력
				string name(pair.first.begin(), pair.first.end());
				ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + button_size); // 텍스트 줄바꿈 제한
				ImGui::TextUnformatted(name.c_str());
				ImGui::PopTextWrapPos();

				ImGui::EndGroup();

				// 그리드 배치 계산
				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_size;

				if (++i < modelMap.size() && next_button_x2 < window_visible_x2)
					ImGui::SameLine();

				ImGui::PopID();
			}
			ImGui::EndTabItem();
		}
	}
	ImGui::EndTabBar();
	ImGui::End();
}

void Resource_Manager::CreateDefaultMesh()
{
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateQuad();
		AddResource(L"Quad", mesh);
	}
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateCube();
		AddResource(L"Cube", mesh);
	}
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateSphere();
		AddResource(L"Sphere", mesh);
	}
}



unique_ptr<Resource_Manager> Resource_Manager::Create()
{
	auto pInstance = unique_ptr<Resource_Manager>(new Resource_Manager);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Resource_Manager Create Failed");
		return nullptr;
	}

	return pInstance;
}