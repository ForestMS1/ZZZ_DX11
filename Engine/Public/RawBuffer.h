#pragma once

NS_BEGIN(Engine)

class ENGINE_DLL RawBuffer
{
public:
	RawBuffer(void* inputData, uint32 inputByte, uint32 outputByte);
	~RawBuffer();

public:
	void CreateBuffer();

	// CPU -> GPU
	void CopyToInput(void* data);
	// GPU -> CPU
	void CopyFromOutput(void* data);

public:
	ComPtr<ID3D11ShaderResourceView> GetSRV() { return _srv; }
	ComPtr<ID3D11UnorderedAccessView> GetUAV() { return _uav; }

private:
	// Input
	void CreateInput();
	void CreateSRV();

	// Output
	void CreateOutput();
	void CreateUAV();

	// Result
	void CreateResult();

private:
	// Input
	ComPtr<ID3D11Buffer> _input;
	ComPtr<ID3D11ShaderResourceView> _srv;

	// Output
	ComPtr<ID3D11Buffer> _output;
	ComPtr<ID3D11UnorderedAccessView> _uav;

	// Result
	ComPtr<ID3D11Buffer> _result;

private:
	void* _inputData;
	// 데이터 크기
	uint32 _inputByte = 0;
	uint32 _outputByte = 0;
};

NS_END
