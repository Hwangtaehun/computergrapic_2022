////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"
#include "DDSTextureLoader.h"

using namespace DirectX;

TextureClass::TextureClass()
{
	m_texture = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}


bool TextureClass::Initialize(ID3D11Device* device, const WCHAR* filename)
{
	HRESULT result;

	// Load texture data from a file by using DDS texture loader. 
	result = CreateDDSTextureFromFile(device, filename, nullptr, &m_texture); //dds파일을 로드해서 &m_texture(쉐이더 리소스 뷰) 외부 이미지 파일 로드를 없졌으므로 따로 cpp파일과 h파일을 만들어야함
	if (FAILED(result))
	{
			return false;
	}
	
	return true;
}


void TextureClass::Shutdown()
{
	// Release the texture resource.
	if(m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}


ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}