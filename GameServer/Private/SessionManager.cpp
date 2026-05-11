#include "pch.h"
#include "SessionManager.h"
#include "Session.h"

SessionManager::SessionManager()
{

}

SessionManager::~SessionManager()
{
}

shared_ptr<Session> SessionManager::GetOrCreateSession(const udp::endpoint& endpoint)
{
    lock_guard<mutex> lock(_mutex);
    string key = endpoint.address().to_string() + ":" + to_string(endpoint.port());

    auto it = _sessions.find(key);
    if (it != _sessions.end())
        return it->second;

    // 새 세션 생성
    uint32_t newId = _idGenerator++;
    auto session = make_shared<Session>(endpoint, newId);
    _sessions.emplace(key, session);
    std::cout << "새 세션 생성, key : " << key << std::endl;
    return session;
}

void SessionManager::RemoveSession(const string& key) 
{
    lock_guard<mutex> lock(_mutex);
    _sessions.erase(key);
    std::cout << "세션 제거, key : " << key << std::endl;
}

void SessionManager::ForEachSession(std::function<void(shared_ptr<Session>)> func)
{
    lock_guard<mutex> lock(_mutex);
    for (auto& [key, session] : _sessions) {
        func(session);
    }
}

uint32_t SessionManager::GenerateObjectId() {
    // atomic 변수를 사용하여 안전하게 증가
    return _objectIdGenerator.fetch_add(1);
}

void SessionManager::CleanupInactiveSessions(std::chrono::seconds timeout) {
    lock_guard<mutex> lock(_mutex);
    auto now = std::chrono::steady_clock::now();

    for (auto it = _sessions.begin(); it != _sessions.end(); ) {
        if (now - it->second->GetLastSeen() > timeout) {
            // 여기에 세션 종료 시 필요한 정리 로직(플레이어 제거 등)을 추가할 수 있습니다.
            it = _sessions.erase(it);
        }
        else {
            ++it;
        }
    }
}