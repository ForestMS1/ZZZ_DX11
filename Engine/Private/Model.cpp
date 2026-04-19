#include "pch.h"
#include "Model.h"
#include "FileUtils.h"
#include "Material.h"
#include "ModelMesh.h"
#include <filesystem>
#include "ModelAnimation.h"
Model::Model()
	: ResourceBase(ResourceType::MODEL)
{
}

Model::~Model()
{
}

void Model::ReadModel(const wstring& filename)
{
	ResourceBase::SetName(filename);

	wstring fullPath = _modelPath + filename + L".mesh";

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	// Bones
	{
		const uint32 count = file->Read<uint32>();

		for (uint32 i = 0; i < count; i++)
		{
			shared_ptr<ModelBone> bone = make_shared<ModelBone>();
			bone->index = file->Read<int32>();
			bone->name = Utils::ToWString(file->Read<string>());
			bone->parentIndex = file->Read<int32>();
			bone->transform = file->Read<Matrix>();

			_bones.push_back(bone);
		}
	}

	// Mesh
	{
		const uint32 count = file->Read<uint32>();

		for (uint32 i = 0; i < count; i++)
		{
			shared_ptr<ModelMesh> mesh = make_shared<ModelMesh>();

			mesh->name = Utils::ToWString(file->Read<string>());
			mesh->boneIndex = file->Read<int32>();

			// Material
			mesh->materialName = Utils::ToWString(file->Read<string>());

			//VertexData
			{
				const uint32 count = file->Read<uint32>();
				vector<ModelVertexType> vertices;
				vertices.resize(count);

				void* data = vertices.data();
				file->Read(&data, sizeof(ModelVertexType) * count);
				mesh->geometry->AddVertices(vertices);
			}

			//IndexData
			{
				const uint32 count = file->Read<uint32>();

				vector<uint32> indices;
				indices.resize(count);

				void* data = indices.data();
				file->Read(&data, sizeof(uint32) * count);
				mesh->geometry->AddIndices(indices);
			}

			mesh->CreateBuffers();

			_meshes.push_back(mesh);
		}
	}

	BindCacheInfo();
}

void Model::ReadModelRotatedY180(const wstring& filename)
{
	ResourceBase::SetName(filename);

	wstring fullPath = _modelPath + filename + L".mesh";

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	// Bones
	{
		const uint32 count = file->Read<uint32>();

		for (uint32 i = 0; i < count; i++)
		{
			shared_ptr<ModelBone> bone = make_shared<ModelBone>();
			bone->index = file->Read<int32>();
			bone->name = Utils::ToWString(file->Read<string>());
			bone->parentIndex = file->Read<int32>();
			bone->transform = file->Read<Matrix>();

			// 추가: 최상위 부모 본의 기본 포즈를 180도 회전
			if (bone->parentIndex == -1)
			{
				bone->transform = bone->transform * Matrix::CreateRotationY(XM_PI);
			}

			_bones.push_back(bone);
		}
	}

	// Mesh
	{
		const uint32 count = file->Read<uint32>();

		for (uint32 i = 0; i < count; i++)
		{
			shared_ptr<ModelMesh> mesh = make_shared<ModelMesh>();

			mesh->name = Utils::ToWString(file->Read<string>());
			mesh->boneIndex = file->Read<int32>();

			// Material
			mesh->materialName = Utils::ToWString(file->Read<string>());

			//VertexData
			{
				const uint32 count = file->Read<uint32>();
				vector<ModelVertexType> vertices;
				vertices.resize(count);

				void* data = vertices.data();
				file->Read(&data, sizeof(ModelVertexType) * count);
				mesh->geometry->AddVertices(vertices);
			}

			//IndexData
			{
				const uint32 count = file->Read<uint32>();

				vector<uint32> indices;
				indices.resize(count);

				void* data = indices.data();
				file->Read(&data, sizeof(uint32) * count);
				mesh->geometry->AddIndices(indices);
			}

			mesh->CreateBuffers();

			_meshes.push_back(mesh);
		}
	}

	BindCacheInfo();
}

void Model::ReadModelCombined(const wstring& filename)
{
	ResourceBase::SetName(filename);

	wstring fullPath = _modelPath + filename + L".mesh";

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	// Bones
	{
		const uint32 count = file->Read<uint32>();

		for (uint32 i = 0; i < count; i++)
		{
			shared_ptr<ModelBone> bone = make_shared<ModelBone>();
			bone->index = file->Read<int32>();
			bone->name = Utils::ToWString(file->Read<string>());
			bone->parentIndex = file->Read<int32>();
			bone->transform = file->Read<Matrix>();

			// 추가: 최상위 부모 본의 기본 포즈를 180도 회전
			if (bone->parentIndex == -1)
			{
				bone->transform = bone->transform * Matrix::CreateRotationY(XM_PI);
			}

			_bones.push_back(bone);
		}
	}

	// Mesh 통합 로직
	{
		const uint32 meshCount = file->Read<uint32>();

		// 하나로 합칠 메쉬 객체 생성
		shared_ptr<ModelMesh> combinedMesh = make_shared<ModelMesh>();
		combinedMesh->name = L"CombinedMesh_" + filename;
		combinedMesh->boneIndex = 0; // 통합 시 기준이 될 본 인덱스

		uint32 vertexOffset = 0; // 인덱스 재계산을 위한 오프셋

		for (uint32 i = 0; i < meshCount; i++)
		{
			// 파일 구조상 있는 메타데이터들 읽기 (개별 mesh 객체는 안 만듦)
			wstring name = Utils::ToWString(file->Read<string>());
			int32 boneIndex = file->Read<int32>();
			wstring materialName = Utils::ToWString(file->Read<string>());

			// 첫 번째 메쉬의 머티리얼을 대표로 쓰거나, 
			// 통합 시 머티리얼이 다르면 처리가 복잡해짐 (아래 주의사항 참고)
			if (i == 0) combinedMesh->materialName = materialName;

			// VertexData 읽기 및 추가
			{
				const uint32 vCount = file->Read<uint32>();
				vector<ModelVertexType> vertices(vCount);
				void* data = vertices.data();
				file->Read(&data, sizeof(ModelVertexType) * vCount);

				combinedMesh->geometry->AddVertices(vertices);
			}

			// IndexData 읽기 및 추가 (오프셋 수정 필수)
			{
				const uint32 iCount = file->Read<uint32>();
				vector<uint32> indices(iCount);
				void* data = indices.data();
				file->Read(&data, sizeof(uint32) * iCount);

				// 현재까지 쌓인 정점 개수만큼 인덱스 번호를 밀어줘야 함
				for (uint32& index : indices)
				{
					index += vertexOffset;
				}

				combinedMesh->geometry->AddIndices(indices);

				// 다음 메쉬를 위해 오프셋 갱신
				vertexOffset += (uint32)combinedMesh->geometry->GetVertices().size() - vertexOffset;
				// 위 식은 사실 방금 추가한 vCount와 같다
			}
		}

		combinedMesh->CreateBuffers();
		_meshes.push_back(combinedMesh); // 결과적으로 _meshes에는 1개만 들어감
	}

	BindCacheInfo();
}

void Model::ReadMaterial(const wstring& filename)
{
	wstring fullPath = _texturePath + filename + L".xml";
	auto parentPath = filesystem::path(fullPath).parent_path();

	tinyxml2::XMLDocument document;
	tinyxml2::XMLError error = document.LoadFile(Utils::ToString(fullPath).c_str());
	assert(error == tinyxml2::XML_SUCCESS);

	tinyxml2::XMLElement* root = document.FirstChildElement();
	tinyxml2::XMLElement* materialNode = root->FirstChildElement();

	while (materialNode)
	{
		shared_ptr<Material> material = make_shared<Material>();

		tinyxml2::XMLElement* node = nullptr;

		node = materialNode->FirstChildElement();
		material->SetName(Utils::ToWString(node->GetText()));

		//Diffuse Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			wstring textureStr = Utils::ToWString(node->GetText());
			if (textureStr.length() > 0)
			{
				auto texture = GAME.GetOrAddTexture(textureStr, (parentPath / textureStr).wstring());
				material->SetDiffuseMap(texture);
			}
		}

		//Specular Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			wstring texture = Utils::ToWString(node->GetText());
			if (texture.length() > 0)
			{
				wstring textureStr = Utils::ToWString(node->GetText());
				if (textureStr.length() > 0)
				{
					auto texture = GAME.GetOrAddTexture(textureStr, (parentPath / textureStr).wstring());
					material->SetSpecularMap(texture);
				}
			}
		}

		//Normal Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			wstring textureStr = Utils::ToWString(node->GetText());
			if (textureStr.length() > 0)
			{
				auto texture = GAME.GetOrAddTexture(textureStr, (parentPath / textureStr).wstring());
				material->SetNormalMap(texture);
			}
		}

		//Ambient
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");
			material->GetMaterialDesc().ambient = color;
		}

		// Diffuse
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");
			material->GetMaterialDesc().diffuse = color;
		}

		// Specular
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");
			material->GetMaterialDesc().specular = color;
		}

		// Emissive
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");
			material->GetMaterialDesc().emissive = color;
		}

		_materials.push_back(material);

		// Next Material
		materialNode = materialNode->NextSiblingElement();
	}

	BindCacheInfo();
}

void Model::ReadAnimation(const wstring& filename)
{
	wstring fullPath = _modelPath + filename + L".clip";

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	shared_ptr<ModelAnimation> animation = make_shared<ModelAnimation>();

	animation->name = Utils::ToWString(file->Read<string>());
	animation->duration = file->Read<float>();
	animation->frameRate = file->Read<float>();
	animation->frameCount = file->Read<uint32>();

	uint32 keyframesCount = file->Read<uint32>();

	for (uint32 i = 0; i < keyframesCount; i++)
	{
		shared_ptr<ModelKeyframe> keyframe = make_shared<ModelKeyframe>();
		keyframe->boneName = Utils::ToWString(file->Read<string>());

		uint32 size = file->Read<uint32>();

		if (size > 0)
		{
			keyframe->transforms.resize(size);
			void* ptr = &keyframe->transforms[0];
			file->Read(&ptr, sizeof(ModelKeyframeData) * size);
		}

		animation->keyframes[keyframe->boneName] = keyframe;
	}

	_animations.push_back(animation);
}

void Model::ReadAnimationNoMove(const wstring& filename)
{
	wstring fullPath = _modelPath + filename + L".clip";

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	shared_ptr<ModelAnimation> animation = make_shared<ModelAnimation>();

	animation->name = Utils::ToWString(file->Read<string>());
	animation->duration = file->Read<float>();
	animation->frameRate = file->Read<float>();
	animation->frameCount = file->Read<uint32>();

	uint32 keyframesCount = file->Read<uint32>();

	for (uint32 i = 0; i < keyframesCount; i++)
	{
		shared_ptr<ModelKeyframe> keyframe = make_shared<ModelKeyframe>();
		keyframe->boneName = Utils::ToWString(file->Read<string>());

		uint32 size = file->Read<uint32>();

		if (size > 0)
		{
			keyframe->transforms.resize(size);
			void* ptr = &keyframe->transforms[0];
			file->Read(&ptr, sizeof(ModelKeyframeData) * size);
			if (keyframe->boneName == GetBoneByIndex(2)->name)
			{
				for (auto& data : keyframe->transforms)
				{
					// 이동하는 bone이 2번 인덱스였음
					// 제자리걸음시킨다.
					data.translation.x = 0.f;
					//data.translation.y = 0.f;
					data.translation.z = 0.f;
				}
			}
		}

		animation->keyframes[keyframe->boneName] = keyframe;
	}

	_animations.push_back(animation);
}

void Model::ReadAnimationRotatedY180(const wstring& filename)
{
	wstring fullPath = _modelPath + filename + L".clip";

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	shared_ptr<ModelAnimation> animation = make_shared<ModelAnimation>();

	animation->name = Utils::ToWString(file->Read<string>());
	animation->duration = file->Read<float>();
	animation->frameRate = file->Read<float>();
	animation->frameCount = file->Read<uint32>();

	uint32 keyframesCount = file->Read<uint32>();

	for (uint32 i = 0; i < keyframesCount; i++)
	{
		shared_ptr<ModelKeyframe> keyframe = make_shared<ModelKeyframe>();
		keyframe->boneName = Utils::ToWString(file->Read<string>());

		uint32 size = file->Read<uint32>();

		if (size > 0)
		{
			keyframe->transforms.resize(size);
			void* ptr = &keyframe->transforms[0];
			file->Read(&ptr, sizeof(ModelKeyframeData) * size);

			// 루트노드가 캐릭터들은 인덱스가 1이더라.. 다른것들도 잘 될지 모르겠음
			if (keyframe->boneName == GetBoneByIndex(1)->name)
			{
				// Y축 180도 회전 쿼터니언 생성
				// XMQuaternionRotationRollPitchYaw(Pitch, Yaw, Roll) 순서임에 주의 (Yaw가 Y축)
				Quaternion rot180Q = Quaternion::CreateFromYawPitchRoll(XM_PI, 0.f, 0.f);

				for (auto& data : keyframe->transforms)
				{
					// 기존 회전값에 180도 회전 쿼터니언을 곱함
					// 순서: 기존 회전(data.rotation)에 180도 회전(rot180Q)을 적용
					// 로컬 축 기준 회전이라면 뒤에 곱함
					data.rotation = data.rotation * rot180Q;

					// 루트 본이 이동(Translation)도 한다
					// 캐릭터가 제자리에서 도는 것뿐만 아니라 이동 방향도 뒤집는다
					data.translation.x *= -1.0f;
					data.translation.z *= -1.0f;
				}
			}
		}

		animation->keyframes[keyframe->boneName] = keyframe;
	}

	_animations.push_back(animation);
}

void Model::ReadAnimationRotatedY180NoMove(const wstring& filename)
{
	wstring fullPath = _modelPath + filename + L".clip";

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	shared_ptr<ModelAnimation> animation = make_shared<ModelAnimation>();

	animation->name = Utils::ToWString(file->Read<string>());
	animation->duration = file->Read<float>();
	animation->frameRate = file->Read<float>();
	animation->frameCount = file->Read<uint32>();

	uint32 keyframesCount = file->Read<uint32>();

	for (uint32 i = 0; i < keyframesCount; i++)
	{
		shared_ptr<ModelKeyframe> keyframe = make_shared<ModelKeyframe>();
		keyframe->boneName = Utils::ToWString(file->Read<string>());

		uint32 size = file->Read<uint32>();

		if (size > 0)
		{
			keyframe->transforms.resize(size);
			void* ptr = &keyframe->transforms[0];
			file->Read(&ptr, sizeof(ModelKeyframeData) * size);

			// 루트노드가 캐릭터들은 인덱스가 1이더라.. 다른것들도 잘 될지 모르겠음
			if (keyframe->boneName == GetBoneByIndex(1)->name)
			{
				// Y축 180도 회전 쿼터니언 생성
				// XMQuaternionRotationRollPitchYaw(Pitch, Yaw, Roll) 순서임에 주의 (Yaw가 Y축)
				Quaternion rot180Q = Quaternion::CreateFromYawPitchRoll(XM_PI, 0.f, 0.f);

				for (auto& data : keyframe->transforms)
				{
					// 기존 회전값에 180도 회전 쿼터니언을 곱함
					// 순서: 기존 회전(data.rotation)에 180도 회전(rot180Q)을 적용
					// 로컬 축 기준 회전이라면 뒤에 곱함
					data.rotation = data.rotation * rot180Q;
				}
			}
			if (keyframe->boneName == L"Bip001")
			{
				for (auto& data : keyframe->transforms)
				{
					// 캐릭터들 이동을 담당하는 bone이 Bip001이었음
					// 제자리걸음시킨다.
					data.translation.x = 0.f;
					//data.translation.y = 0.f;
					data.translation.z = 0.f;
				}
			}
		}

		animation->keyframes[keyframe->boneName] = keyframe;
	}

	_animations.push_back(animation);
}

shared_ptr<Material> Model::GetMaterialByName(const wstring& name)
{
	for (auto& material : _materials)
	{
		if (material->GetName() == name)
			return material;
	}

	return nullptr;
}

shared_ptr<ModelMesh> Model::GetMeshByName(const wstring& name)
{
	for (auto& mesh : _meshes)
	{
		if (mesh->name == name)
			return mesh;
	}

	return nullptr;
}

shared_ptr<ModelBone> Model::GetBoneByName(const wstring& name)
{
	for (auto& bone : _bones)
	{
		if (bone->name == name)
			return bone;
	}

	return nullptr;
}

shared_ptr<ModelAnimation> Model::GetAnimationByName(const wstring& name)
{
	for (auto& animation : _animations)
	{
		if (animation->name == name)
			return animation;
	}

	return nullptr;
}

UINT Model::GetAnimationIndexByName(const wstring& name)
{
	for (uint32 i = 0; i < _animations.size(); ++i)
	{
		if (_animations[i]->name == name)
		{
			return i;
		}
	}

	return 0;
}

void Model::BindCacheInfo()
{
	// Mesh에 Material 캐싱
	for (const auto& mesh : _meshes)
	{
		// 이미 찾았으면 스킵
		if (mesh->material != nullptr)
			continue;

		mesh->material = GetMaterialByName(mesh->materialName);
	}

	// Mesh에 Bone 캐싱
	for (const auto& mesh : _meshes)
	{
		// 이미 찾았으면 스킵
		if (mesh->bone != nullptr)
			continue;

		mesh->bone = GetBoneByIndex(mesh->boneIndex);

		if (mesh->geometry != nullptr)
		{
			const vector<ModelVertexType>& vertices = mesh->geometry->GetVertices();
			// mesh의 정점들 중 최소,최대값 으로만 단순 상자를 만든다. 메쉬 모양그대로 만드는 거 아님!
			BoundingBox::CreateFromPoints(
				mesh->boundingBox,
				vertices.size(),
				&vertices[0].position,
				sizeof(ModelVertexType)
			);
		}
	}

	// Bone 계층 정보 채우기
	if (_root == nullptr && _bones.size() > 0)
	{
		_root = _bones[0];

		for (const auto& bone : _bones)
		{
			if (bone->parentIndex >= 0)
			{
				bone->parent = _bones[bone->parentIndex];
				bone->parent.lock()->children.push_back(bone);
			}
			else
			{
				bone->parent.reset();
			}
		}
	}
}
