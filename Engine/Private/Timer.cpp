#include "pch.h"
#include "Timer.h"

Timer::Timer() : _timeDelta(0.f)
{
	ZeroMemory(&_fixTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&_lastTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&_frameTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&_cpuTick, sizeof(LARGE_INTEGER));
}

Timer::~Timer()
{

}

HRESULT Timer::Ready_Timer()
{
	QueryPerformanceCounter(&_frameTime);
	QueryPerformanceCounter(&_lastTime);
	QueryPerformanceCounter(&_fixTime);

	QueryPerformanceFrequency(&_cpuTick);

	return S_OK;
}

void Timer::Update_Timer()
{
	QueryPerformanceCounter(&_frameTime);

	if (_frameTime.QuadPart - _fixTime.QuadPart >= _cpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&_cpuTick);
		_fixTime = _frameTime;
	}

	_timeDelta = (_frameTime.QuadPart - _lastTime.QuadPart) / (float)_cpuTick.QuadPart;

	_lastTime = _frameTime;
}

unique_ptr<Timer> Timer::Create()
{
	auto pInstance = unique_ptr<Timer>(new Timer);

	if (FAILED(pInstance->Ready_Timer()))
		return nullptr;

	return pInstance;
}