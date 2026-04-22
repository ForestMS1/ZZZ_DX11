#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL Prototype abstract : public enable_shared_from_this<Prototype>
{
protected:
	Prototype() {}
	Prototype(const Prototype& rhs);
public:
	virtual ~Prototype() {}

	void SetPrototypeName(const wstring& name) { _prototypeName = name; }
	wstring GetPrototypeName() { return _prototypeName; }
public:
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) = 0;

protected:
	wstring _prototypeName;
};

NS_END
