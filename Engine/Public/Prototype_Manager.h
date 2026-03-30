#pragma once

NS_BEGIN(Engine)

class Prototype;


class Prototype_Manager
{
private:
	Prototype_Manager();
public:
	~Prototype_Manager() = default;


public:
	HRESULT Initialize(uint32 iNumLevels);
	HRESULT Add_Prototype(uint32 iLevelIndex, const wstring& strPrototypeTag, unique_ptr<Prototype> pPrototype);
	shared_ptr<Prototype> Clone_Prototype(uint32 iLevelIndex, const wstring& strPrototypeTag, void* pArg);

private:
	uint32 _numLevels = {};
private:
	unique_ptr<map<wstring, unique_ptr<Prototype>>[]>		_protoTypes = { nullptr };
	typedef map<wstring, unique_ptr<Prototype>>				PROTOTYPES;

private:
	Prototype* Find_Prototype(uint32 iLevelIndex, const wstring& strPrototypeTag);

public:
	static unique_ptr<Prototype_Manager> Create(uint32 iNumLevels);

};

NS_END

