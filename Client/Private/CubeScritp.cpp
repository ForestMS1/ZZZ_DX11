#include "pch.h"
#include "CubeScritp.h"
#include "GameInstance.h"

CubeScritp::CubeScritp()
{
}

CubeScritp::~CubeScritp()
{
}

void CubeScritp::Update()
{
	GAME.Add_RenderObject(RENDERGROUP::NONBLEND, GetGameObject());
}
