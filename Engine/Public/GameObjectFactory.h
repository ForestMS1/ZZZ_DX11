#pragma once

NS_BEGIN(Engine)

using CreatorFunc = std::function<shared_ptr<GameObject>()>;

class ENGINE_DLL GameObjectFactory 
{
private:
    GameObjectFactory();
public:
    ~GameObjectFactory();
private:
    map<wstring, CreatorFunc> _creators;

public:
    void Register(const wstring& className, CreatorFunc func) { _creators[className] = func; }

    shared_ptr<GameObject> CreateFromFactory(const wstring& className);

public:
    static unique_ptr<GameObjectFactory> Create();
};

NS_END