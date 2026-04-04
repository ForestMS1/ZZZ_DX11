#pragma once

NS_BEGIN(Engine)

enum class ResourceType : uint8
{
	NONE,
	MESH,
	MODEL,
	SHADER,
	TEXTURE,
	MATERIAL,
	ANIMATION,

	END
};

enum
{
	RESOURCE_TYPE_COUNT = static_cast<uint8>(ResourceType::END)
};


class ENGINE_DLL ResourceBase abstract : public enable_shared_from_this<ResourceBase>
{
protected:
	ResourceBase(ResourceType eType);
public:
	virtual ~ResourceBase();

public:
	ResourceType GetType() { return _type; }

	void SetName(const wstring& name) { _name = name; }
	const wstring& GetName() { return _name; }
	uint32 GetID() const { return _id; }

protected:
	virtual void Save(const wstring& path) {}
	virtual void Load(const wstring& path) {}

protected:
	ResourceType _type = ResourceType::NONE;
	wstring _name;
	wstring _path;
	uint32 _id = 0;
};

NS_END