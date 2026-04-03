#pragma once

#include "Level.h"


/* 레벨로서의 기본역활 */
/* -> 이 레벨에 필요한 객체를 생성한다. */
/* -> 생성한 객체의 반복적인 업데이트 렌더등을 호출해준다. */

/* 다음 레벨을 보여주기위해 필요한 자원을 미리 준비해주는 역할 */

NS_BEGIN(AssimpTool)

class Level_Loading final : public Level
{
private:
	Level_Loading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext);
public:
	~Level_Loading();

public:
	virtual HRESULT Initialize(LEVEL eNextLevelIndex);
	virtual void Update() override;
	virtual HRESULT Render() override;


private:
	LEVEL _nextLevelIndex = { LEVEL::END };
	unique_ptr<class Loader> _loader = { nullptr };

private:
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);


public:
	static unique_ptr<Level_Loading> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext, LEVEL eNextLevelIndex);


};

NS_END
