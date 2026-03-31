#pragma once

NS_BEGIN(Engine)

struct VertexData
{
	Vec3 position = { 0.f,0.f,0.f };
};

struct VertexTextureData
{
	Vec3 position = { 0.f,0.f,0.f };
	Vec2 uv = { 0.f, 0.f };
};

struct VertexColorData
{
	Vec3 position = { 0.f, 0.f, 0.f };
	Color color = { 0, 0, 0, 0 };
};

struct VertexTextureNormalData
{
	Vec3 position = { 0.f, 0.f, 0.f };
	Vec2 uv = { 0.f, 0.f };
	Vec3 normal = { 0.f, 0.f, 0.f };
};

struct VertexTextureNormalTangentData
{
	Vec3 position = { 0.f, 0.f, 0.f };
	Vec2 uv = { 0.f, 0.f };
	Vec3 normal = { 0.f, 0.f, 0.f };
	Vec3 tangent = { 0.f, 0.f, 0.f };
};

struct VertexTextureNormalTangentBlendData
{
	Vec3 position = { 0.f, 0.f, 0.f };
	Vec2 uv = { 0.f, 0.f };
	Vec3 normal = { 0.f, 0.f, 0.f };
	Vec3 tangent = { 0.f, 0.f, 0.f };
	Vec4 blendIndices = { 0.f, 0.f, 0.f, 0.f };
	Vec4 blendWeights = { 0.f, 0.f, 0.f, 0.f };
};

using ModelVertexType = VertexTextureNormalTangentBlendData;

NS_END