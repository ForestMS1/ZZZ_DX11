#pragma once
#include "Component.h"

NS_BEGIN(Engine)

enum class LIGHTTYPE
{
	Direction,
	Point,
	Spot,
	End
};

class ENGINE_DLL Light : public Component
{
public:
	Light();
	~Light();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;

	// 생명주기 함수
	virtual void Update() override;

	void UpdateMatrix();

	LightDesc& GetLightDesc() { return _lightDesc; }
	LIGHTTYPE GetLightType() { return _lightType; }
	const Matrix& GetLighViewMatrix() { return _lightViewMatrix; }
	const Matrix& GetLighProjMatrix() { return _lightProjMatrix; }

	void SetLightDesc(LightDesc& desc) { _lightDesc = desc; }
	void SetAmbient(const Color& color) { _lightDesc.ambient = color; }
	void SetDiffuse(const Color& color) { _lightDesc.diffuse = color; }
	void SetSpecular(const Color& color) { _lightDesc.specular = color; }
	void SetEmissive(const Color& color) { _lightDesc.emissive = color; }
	void SetLightDirection(Vec3 direction) { _lightDesc.direction = direction; }
	void SetLightType(LIGHTTYPE type) { _lightType = type; }

	// ImGUI Inspector 창 정보
	virtual void	OnInspectorGUI() override;

	// 컴포넌트가 객체로부터 떨어질 때 컴포넌트안의 메모리 정리 기회
	virtual void	OnDestroy() override {};

	virtual shared_ptr<Prototype> Clone(void* pArg = nullptr) { return make_shared<Light>(*this); }
private:
	LightDesc _lightDesc;
	LIGHTTYPE _lightType = LIGHTTYPE::End;

	Matrix _lightViewMatrix;
	Matrix _lightProjMatrix;
};

NS_END

