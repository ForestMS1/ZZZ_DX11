#pragma once

#include "ResourceBase.h"

NS_BEGIN(Engine)

class Texture;
class Mesh;
class Material;

class ENGINE_DLL Resource_Manager
{
private:
	Resource_Manager();
public:
	~Resource_Manager();

	HRESULT Initialize();

	template<typename T>
	shared_ptr<T> Load(const wstring& key, const wstring& path);

	template<typename T>
	bool AddResource(const wstring& key, shared_ptr<T> object);

	template<typename T>
	shared_ptr<T> GetResource(const wstring& key);

	shared_ptr<Texture> GetOrAddTexture(const wstring& key, const wstring& path);

private:
	void CreateDefaultMesh();

	template<typename T>
	ResourceType GetResourceType();

private:
	wstring _resourcePath;
private:
	using KeyObjMap = map<wstring, shared_ptr<ResourceBase>>;
	array<KeyObjMap, RESOURCE_TYPE_COUNT> _resources;

public:
	static unique_ptr<Resource_Manager> Create();
};


template<typename T>
shared_ptr<T> Resource_Manager::Load(const wstring& key, const wstring& path)
{
	ResourceType objectType = GetResourceType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	shared_ptr<T> object = make_shared<T>();
	object->Load(path); // Texture, Mesh, Material 같은 리소스클래스에 구현되어있는 Load함수
	keyObjMap[key] = object;

	return object;
}

template<typename T>
bool Resource_Manager::AddResource(const wstring& key, shared_ptr<T> object)
{
	ResourceType resourceType = GetResourceType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(resourceType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return false;

	keyObjMap[key] = object;
	return true;
}

template<typename T>
shared_ptr<T> Resource_Manager::GetResource(const wstring& key)
{
	ResourceType resourceType = GetResourceType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(resourceType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	return nullptr;
}


template<typename T>
ResourceType Resource_Manager::GetResourceType()
{
	if (std::is_same_v<T, Texture>)
		return ResourceType::TEXTURE;
	if (std::is_same_v<T, Mesh>)
		return ResourceType::MESH;
	if (std::is_same_v <T, Material>)
		return ResourceType::MATERIAL;

	assert(false);
	return ResourceType::NONE;
}

NS_END