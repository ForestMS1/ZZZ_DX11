#pragma once

NS_BEGIN(Engine)

class Level_Manager final
{
private:
	Level_Manager();
public:
	~Level_Manager() = default;

public:
	HRESULT Change_Level(uint32 iNewLevelIndex, unique_ptr<class Level> pNewLevel); // A씬 -> B씬 넘어갈때 호출하는 함수 pNewLevel은 로딩씬으로 넣어라. (결과 : A씬 -> 로딩씬으로 넘어감)
	HRESULT Change_Loading_toNext(uint32 iNewLevelIndex, unique_ptr<class Level> pNewLevel); //로딩씬 -> B씬 넘어갈때 호출하는 함수 (결과 : 로딩씬 -> B씬으로 넘어감)
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FixedUpdate();
	HRESULT Render();

public:
	uint32 GetCurrentLevelIndex() const { return _currentLevelIndex; }

private:
	uint32 _currentLevelIndex = {};
	unique_ptr<class Level> _currentLevel = { nullptr };

public:
	static unique_ptr<Level_Manager> Create();
};

NS_END
