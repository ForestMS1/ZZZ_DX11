#pragma once

NS_BEGIN(Engine)

class Level_Manager final
{
private:
	Level_Manager();
public:
	~Level_Manager() = default;

public:
	HRESULT Change_Level(uint32_t iNewLevelIndex, unique_ptr<class Level> pNewLevel, bool clearResource = false);
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
	uint32 _nextLevelIndex = {};
	unique_ptr<class Level> _currentLevel = { nullptr };

public:
	static unique_ptr<Level_Manager> Create();
};

NS_END
