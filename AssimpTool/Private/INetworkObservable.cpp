#include "pch.h"
#include "INetworkObservable.h"

INetworkObservable::INetworkObservable(NetworkViewType type)
	: _viewType(type)
{
}

INetworkObservable::~INetworkObservable()
{
}
