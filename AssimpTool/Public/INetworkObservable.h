#pragma once
#include "CommonStruct.h"

enum class NetworkViewType : uint8
{
    TransfomrView,
    AnimationView,
    //...
    END
};

enum { VIEW_COUNT = static_cast<uint8>(NetworkViewType::END) };

class INetworkObservable
{
protected:
    INetworkObservable(NetworkViewType type);
    virtual ~INetworkObservable();
public:
	// 데이터를 패킷에 쓸 때 (IsMine == true)
	// payload 버퍼에 자기 데이터를 밀어넣고, flags에 자기 비트를 켠다
    virtual void OnSerialize(std::vector<uint8_t>& outPayload, uint32_t& outFlags) = 0;

	// 패킷에서 데이터를 읽어올 때 (IsMine == false)
    // 수신 시에는 읽어야 할 데이터의 시작 포인터를 받음
    virtual void OnDeserialize(const void* data) = 0;

    NetworkViewType GetViewType() { return _viewType; }

protected:
    NetworkViewType _viewType = NetworkViewType::END;
};

