#pragma once

NS_BEGIN(Engine)

class _declspec(dllexport) Timer
{
private:
	Timer();
public:
	virtual ~Timer();

public:
	float	Get_TimeDelta() const { return _timeDelta; }

public:
	HRESULT	Ready_Timer();
	void	Update_Timer();

private:
	LARGE_INTEGER	_frameTime = {};
	LARGE_INTEGER	_fixTime = {};
	LARGE_INTEGER	_lastTime = {};
	LARGE_INTEGER	_cpuTick = {};

	float _timeDelta = {};

public:
	static unique_ptr<Timer> Create();
};


NS_END