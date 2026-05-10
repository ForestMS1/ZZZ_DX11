#pragma once

//#include "Session.h"
   class Session;

class SessionManager 
{
    DECLARE_SINGLETON(SessionManager)
private:
    SessionManager();
public:
    ~SessionManager();
public:
    shared_ptr<Session> GetOrCreateSession(const udp::endpoint& endpoint);

    void RemoveSession(const string& key);

    // 전체 세션 순회 (브로드캐스트용)
    void ForEachSession(std::function<void(shared_ptr<Session>)> func);

    uint32_t GenerateObjectId();

    void CleanupInactiveSessions(std::chrono::seconds timeout);

private:
    unordered_map<string, shared_ptr<Session>> _sessions;
    mutex _mutex;
    atomic<uint32_t> _idGenerator = 1; // Session ID용
    std::atomic<uint32_t> _objectIdGenerator = 1000; // Object ID용 (구분하기 쉽게 1000부터 시작)
};