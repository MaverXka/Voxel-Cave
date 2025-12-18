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

using namespace Microsoft::WRL;

class Shader
{
public:

	Shader(const wchar_t* ShaderPath, const char* EntryName, ShaderType shaderType);

	LPVOID GetShaderPointer()
	{
		return ShaderBlob->GetBufferPointer();
	}

	SIZE_T GetShaderBufferSize()
	{
		return ShaderBlob->GetBufferSize();
	}

protected:

	ComPtr<ID3DBlob> ShaderBlob;

};