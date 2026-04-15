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
    // [클래스 이름], [생성방식]
    map<wstring, CreatorFunc> _creators;

public:
    // GameObject 생성방식 등록
    void Register(const wstring& className, CreatorFunc func) { _creators[className] = func; }

    // GameObject 찍어낸다.
    shared_ptr<GameObject> CreateFromFactory(const wstring& className);

public:
    static unique_ptr<GameObjectFactory> Create();
};

NS_END