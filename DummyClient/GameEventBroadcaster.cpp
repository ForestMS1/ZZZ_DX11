#include "pch.h"
#include "GameEventBroadcaster.h"

GameEventBroadcaster::GameEventBroadcaster(boost::asio::io_context& io_context, short port)
    : _socket(io_context, udp::endpoint(udp::v4(), port))
{
}

GameEventBroadcaster::~GameEventBroadcaster()
{
}

void GameEventBroadcaster::TriggerEvent(EventType type, uint32_t srcId, uint32_t dstId, float x, float y, float z, uint32_t data)
{
    GameEvent event;
    event.type = type;
    event.srcPlayerId = srcId;
    event.dstPlayerId = dstId;
    event.x = x; event.y = y; event.z = z;
    event.eventData = data;
    event.timestamp = GetCurrentTimestamp();

    BroadcastEvent(event);
    LogEvent(event);
}

void GameEventBroadcaster::AddClient(const udp::endpoint& client)
{
    lock_guard<mutex> lock(_clientsMutex);
    _clients.insert(client);
    std::cout << "Client added: " << client << std::endl;
}

void GameEventBroadcaster::RemoveClient(const udp::endpoint& client)
{
    lock_guard<mutex> lock(_clientsMutex);
    _clients.erase(client);
    std::cout << "Client removed: " << client << std::endl;
}

void GameEventBroadcaster::BroadcastEvent(const GameEvent& event)
{
    auto buffer = make_shared<vector<char>>(sizeof(GameEvent));
    ::memcpy(buffer->data(), &event, sizeof(GameEvent));

    lock_guard<mutex> lock(_clientsMutex);
    for (const auto& client : _clients)
    {
        _socket.async_send_to(boost::asio::buffer(*buffer), client,
            [buffer](const boost::system::error_code& ec, std::size_t bytes)
            {
                if (ec)
                {
                    std::cerr << "Broadcast error: " << ec.message() << std::endl;
                }
            }
        );
    }
}

void GameEventBroadcaster::LogEvent(const GameEvent& event)
{
    std::cout << "Event: " << GetEventTypeName(event.type)
        << " Source: " << event.srcPlayerId
        << " Target: " << event.dstPlayerId
        << " Position: (" << event.x << ", " << event.y << ", " << event.z << ")"
        << std::endl;
}

string GameEventBroadcaster::GetEventTypeName(EventType type)
{
    switch (type)
    {
    case EventType::PLAYER_SPAWN: return "PLAYER_SPAWN";
    case EventType::PLAYER_DEATH: return "PLAYER_DEATH";
    case EventType::ITEM_PICKUP: return "ITEM_PICKUP";
    case EventType::EXPLOSION: return "EXPLOSION";
    case EventType::ACHIEVEMENT: return "ACHIEVEMENT";
    case EventType::MATCH_RESULT: return "MATCH_RESULT";
    default: return "UNKNOWN";
    }
}

uint32_t GameEventBroadcaster::GetCurrentTimestamp()
{
    return static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
}