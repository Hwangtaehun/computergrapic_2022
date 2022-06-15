////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

//필수 들어가는 부분
/////////////
// LINKING //
/////////////
#pragma comment(lib, "dxgi.lib")		// 상용 개임들도 쓰는 파일, 강제로딩
#pragma comment(lib, "d3d11.lib")		// 상용 게임들도 쓰는 파일, 강제로딩

//////////////
// INCLUDES //
//////////////
#include <dxgi.h>						// 다이렉트 인터페이스
#include <d3dcommon.h>
#include <d3d11.h>
#include <directxmath.h>

#include "AlignedAllocationPolicy.h"	// 다이렉트 x 포함 안됨, 16bit 사용하게위해서 정의 

using namespace DirectX;

//필수로 들어가는 부분

////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass : public AlignedAllocationPolicy<16>  
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();
	
	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();					//(1)
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);		//(2)
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:										//다 이해할 필요없음
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;

	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
};

#endif