#pragma once


NS_BEGIN(Engine)

struct GlobalDesc
{
	Matrix V = Matrix::Identity; // 뷰 스페이스 변환행렬
	Matrix P = Matrix::Identity; // 투영변환 행렬
	Matrix VP = Matrix::Identity; // View * Proj
	Matrix VInv = Matrix::Identity; // View 역행렬 (카메라 월드행렬)
};

struct TransformDesc
{
	Matrix W = Matrix::Identity;
};

struct MaterialDesc
{
	Color ambient = Color(0.f, 0.f, 0.f, 1.f);
	Color diffuse = Color(1.f, 1.f, 1.f, 1.f);
	Color specluar = Color(0.f, 0.f, 0.f, 1.f);
	Color emissive = Color(0.f, 0.f, 0.f, 1.f);
};

NS_END