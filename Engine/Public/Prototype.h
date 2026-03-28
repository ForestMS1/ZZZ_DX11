#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL Prototype abstract
{
public:
	Prototype() {}
	virtual ~Prototype() {}
public:
	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) = 0;
};

NS_END
