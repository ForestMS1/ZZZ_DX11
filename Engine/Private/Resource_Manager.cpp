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