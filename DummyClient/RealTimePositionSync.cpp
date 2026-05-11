#include "pch.h"
#include "RealTimePositionSync.h"
#include "GameEventBroadcaster.h"

RealTimePositionSync::RealTimePositionSync(boost::asio::io_context& io_context)
	: _socket(io_context), _timer(io_context), _sequence(0)
{
}

RealTimePositionSync::~RealTimePositionSync()
{
}

void RealTimePositionSync::UpdatePosition(uint32_t playerId, float x, float y, float z, float vx, float vy, float vz, float rotation)
{
	lock_guard<mutex> lock(_positionMutex);

	auto& pos = _playerPositions[playerId];
    pos.playerId = playerId;
    pos.x = x; pos.y = y; pos.z = z;
    pos.vx = vx; pos.vy = vy; pos.vz = vz;
    pos.rotation = rotation;
    pos.timestamp = GetCurrentTimestamp();
    pos.sequence = ++_sequence;
}

void RealTimePositionSync::BroadcastPositions()
{
    lock_guard<mutex> lock(_positionMutex);

    for (const auto& pair : _playerPositions)
    {
        const auto& pos = pair.second;

        BroadcastPosition(pos);
    }
}

void RealTimePositionSync::StartSyncTimer()
{
    // 60fps로 위치 동기화
    _timer.expires_after(std::chrono::milliseconds(16));
    _timer.async_wait([this](const boost::system::error_code& ec)
        {
            if (!ec)
            {
                BroadcastPositions();
                StartSyncTimer();
            }
        }
    );
}

void RealTimePositionSync::BroadcastPosition(const PositionData& pos)
{
    // UDP를 통한 위치 데이터 브로드캐스트
    auto buffer = make_shared<vector<char>>(sizeof(PositionData));
    ::memcpy(buffer->data(), &pos, sizeof(PositionData));

    // 모든 활성 클라이언트에게 전송 (클라이언트 목록은 별도 관리)
    for (const auto& client : _activeClients)
    {
        _socket.async_send_to(boost::asio::buffer(*buffer), client,
            [buffer](const boost::system::error_code&, std::size_t) {});
    }
}

uint32_t RealTimePositionSync::GetCurrentTimestamp()
{
    return static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
}


