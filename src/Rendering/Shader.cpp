#include "Shader.h"
#include <string>
#include <unordered_map>
#include <d3dcompiler.h>
#include "Debug/Log/Logger.h"

#pragma comment(lib, "d3dcompiler.lib")

const static std::unordered_map<ShaderType, const char*> ShaderTargetTable =
{
	{ ShaderType::VS5_0, "vs_5_0" },
	{ ShaderType::PS5_0, "ps_5_0" },
	{ ShaderType::VS6_0, "vs_6_0" },
	{ ShaderType::PS6_0, "ps_6_0" }
};


Shader::Shader(const wchar_t* ShaderPath, const char* EntryName, ShaderType shaderType)
{
	ID3DBlob* ErrorBlob = nullptr;
	HRESULT result = D3DCompileFromFile(ShaderPath, nullptr, nullptr, EntryName, ShaderTargetTable.at(shaderType), 0, 0, &ShaderBlob, &ErrorBlob);
	if (SUCCEEDED(result))
	{
		std::wstring name = ShaderPath;
		name = name.substr(name.find_last_of(L"/\\") + 1);
		Logger::Get()->Log("Compiled");
		Logger::Get()->LogWString(std::wstring(L"Shader compiled - ") + name);
	}
	else
	{
		Logger::Get()->LogString(std::string("Shader compile error: ") + (const char*)ErrorBlob->GetBufferPointer());
	}
	
}

LPVOID Shader::GetShaderPointer()
{
	return ShaderBlob->GetBufferPointer();
}

SIZE_T Shader::GetShaderBufferSize()
{
	return ShaderBlob->GetBufferSize();
}
