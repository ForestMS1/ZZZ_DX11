#include "pch.h"
#include "MonoBehaviour.h"

MonoBehaviour::MonoBehaviour()
	: Component(ComponentType::Script)
{
}

MonoBehaviour::MonoBehaviour(const MonoBehaviour& rhs)
	: Component(rhs)
{
}

MonoBehaviour::~MonoBehaviour()
{
}



