#include "pch.h"
#include "Timer_Manager.h"
#include "Timer.h"
#include "GameInstance.h"

Timer_Manager::Timer_Manager()
{

}

Timer_Manager::~Timer_Manager()
{
	_Timers.clear();
}

float Timer_Manager::Get_TimeDelta(const wstring& strTimerTag)
{
	Timer* pTimer = Find_Timer(strTimerTag);
	if (pTimer == nullptr)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

void Timer_Manager::Set_TimeDelta(const wstring& strTimerTag)
{
	Timer* pTimer = Find_Timer(strTimerTag);
	if (pTimer == nullptr)
		return;

	pTimer->Update_Timer();
}

HRESULT Timer_Manager::Ready_Timer(const wstring& strTimerTag)
{
	if (nullptr != Find_Timer(strTimerTag))
		return E_FAIL;

	auto pTimer = Timer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	_Timers.emplace(strTimerTag, std::move(pTimer));

	return S_OK;
}

Timer* Timer_Manager::Find_Timer(const wstring& strTimerTag)
{
	auto iter = _Timers.find(strTimerTag);

	if (iter == _Timers.end())
		return nullptr;

	return iter->second.get();
}

unique_ptr<Timer_Manager> Timer_Manager::Create()
{
	return unique_ptr<Timer_Manager>(new Timer_Manager);
}