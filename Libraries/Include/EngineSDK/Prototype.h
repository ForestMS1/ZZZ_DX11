#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL Prototype abstract : public enable_shared_from_this<Prototype>
{
protected:
	Prototype() {}
public:
	virtual ~Prototype() {}
public:
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) = 0;
};

NS_END
