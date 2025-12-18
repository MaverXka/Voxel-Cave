#pragma once

#include <wrl.h>
#include <d3dcompiler.h>


enum ShaderType : int
{
	VS5_0,
	PS5_0,
	VS6_0,
	PS6_0

};

//struct ID3DBlob;

class Shader
{
public:

	Shader(const wchar_t* ShaderPath, const char* EntryName, ShaderType shaderType);

	LPVOID GetShaderPointer();

	SIZE_T GetShaderBufferSize();

protected:

	Microsoft::WRL::ComPtr<ID3DBlob> ShaderBlob;

};