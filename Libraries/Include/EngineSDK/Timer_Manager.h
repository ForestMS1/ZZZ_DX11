#pragma once

NS_BEGIN(Engine)

class Timer;

class Timer_Manager final
{
private:
	Timer_Manager();
public:
	~Timer_Manager();

public:
	float		Get_TimeDelta(const wstring& strTimerTag);
	void		Set_TimeDelta(const wstring& strTimerTag);


public:
	HRESULT		Ready_Timer(const wstring& strTimerTag);

private:
	map<const wstring, unique_ptr<Timer>>		_Timers;

private:
	Timer* Find_Timer(const wstring& strTimerTag);

public:
	static unique_ptr<Timer_Manager> Create();

};
NS_END
