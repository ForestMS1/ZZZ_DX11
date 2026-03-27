#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL Level_Manager final
{
private:
	Level_Manager();
public:
	~Level_Manager() = default;

public:
	HRESULT Change_Level(uint32 iNewLevelIndex, unique_ptr<Level> pNewLevel);
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FixedUpdate();
	HRESULT Render();

private:
	uint32 _currentLevelIndex = {};
	unique_ptr<class Level> _currentLevel = { nullptr };

public:
	static unique_ptr<Level_Manager> Create();
};

NS_END
