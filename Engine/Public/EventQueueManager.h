#pragma once
#include <queue>

NS_BEGIN(Engine)

//enum class EventType
//{
//	LEVEL_START,
//
//	BOSS_SPAWN,
//	WIPEOUT,
//};



// 필요 시 특정 이벤트 전용 데이터 확장 예시
// struct BossSpawnDesc : public EventDesc { int bossId; };


class ENGINE_DLL EventQueueManager
{
private:
	EventQueueManager();
public:
	~EventQueueManager();

	// 객체들이 특정 이벤트에 자신의 멤버 함수를 등록할 때 사용
	void Subscribe(uint32 eventType, const EventCallback& callback)
	{
		_subscribers[eventType].push_back(callback);
	}

	// 기본 이벤트용 (추가 데이터가 없을 때 파라미터 넘기기 편한 오버로딩)
	void PushEvent(uint32 eventType, void* sender = nullptr)
	{
		auto desc = std::make_shared<EventDesc>();
		desc->sender = sender;
		desc->eventType = eventType;
		_eventQueue.push({ eventType, desc });
	}

	void PushEvent(uint32 eventType, shared_ptr<EventDesc> desc)
	{
		if (desc)
		{
			desc->eventType = eventType; // 구조체 내부 값도 싱크를 맞춰줍니다.
		}
		_eventQueue.push({eventType, desc});
	}

	void PushEvent(shared_ptr<EventDesc> desc)
	{
		if(desc)
			_eventQueue.push({ desc->eventType, desc });
	}

	void Update()
	{
		size_t eventCount = _eventQueue.size();

		while (eventCount > 0 && !_eventQueue.empty())
		{
			auto [eventType, desc] = _eventQueue.front();
			_eventQueue.pop();
			--eventCount;

			if (desc == nullptr) continue;

			auto iter = _subscribers.find(eventType);
			if (iter != _subscribers.end())
			{
				for (const auto& callback : iter->second)
				{
					callback(*desc);
				}
			}
		}
	}

	void ClearSubscribers()
	{
		_subscribers.clear();
		_eventQueue = {};
	}

public:
	static unique_ptr<EventQueueManager> Create()
	{
		return std::unique_ptr<EventQueueManager>(new EventQueueManager());
	}

private:
	// 이벤트별 구독자 리스트
	map<uint32, vector<EventCallback>> _subscribers;

	// 이벤트 큐
	std::queue<std::pair<uint32, shared_ptr<EventDesc>>> _eventQueue;
};

NS_END