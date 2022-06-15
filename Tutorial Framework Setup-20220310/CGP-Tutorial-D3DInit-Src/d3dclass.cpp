////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"

D3DClass::D3DClass()		//초기화
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}


bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, 
						  float screenDepth, float screenNear)
{	//초기화 시작
	HRESULT result;  //결과를 답아두는 변수
	IDXGIFactory* factory;								/*시작*/ //단계1 그래픽 카드 기능 확인
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;						/*끝*/  //gpu 스팩을 알아보는 코드 -> 해상도, 메모리 알 수 있음
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;					/*시작*/ // 단계2
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;						/*끝*/  //swap buffer = Double buffer -> buffer은 여러개 둘 수 있다. 앞버퍼는 쓰는것은 아님 -> 뒤버퍼 사용
	D3D11_TEXTURE2D_DESC depthBufferDesc;				/*시작*/ //v단계3
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc; /*끝*/ //Depth buffer = G buffer, 스텐실 버퍼 -> 그림자 사용
	D3D11_RASTERIZER_DESC rasterDesc;					/*시작*/ // 단계4 라스트라이제이션(rasterization) -> 도트를 찍을 때 사용할때 
	D3D11_VIEWPORT viewport;							// viewport: 가로 세로 공간에서 보이는 공간을 말함 -> 이 클래스 전체화면 사용
	float fieldOfView, screenAspect;					/*끝*/

	/*시작*/
	// Store the vsync setting.
	m_vsync_enabled = vsync;

	// Create a DirectX graphics interface factory. = GPU 정보 가져오기 -> 인터페이스를 거쳐가야함 / factory = 하드웨어 / 인터페이스 = 통로
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card). -> 윈도우가 선택한 우선대는 그래픽 카드 사용
	result = factory->EnumAdapters(0, &adapter);  //EnumAdapters: 그래픽 카드 몇개 인지 확인
	if(FAILED(result))
	{
		return false;
	}

	// Enumerate the primary adapter output (monitor). -> 모니터 지원가능한 정보 알아보기
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);  // &numModes에 집어 넣는다.
	if(FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];  // ->그래픽 성능에 따라 다르다.
	if(!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor. ->여기서 해상도 확인, 주사율 확인 -> 너무 낮으면 눈에 안좋음 최소한 30hz, 요즘 60hz, 백그 140hz 
	for(i=0; i<numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if(displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;     // 주사율 알아보기 위해서
				denominator = displayModeList[i].RefreshRate.Denominator; // 주사율 알아보기 위해서
			}
		}
	}

	// Get the adapter (video card) description. = 그래픽카드에 대한 정보 알아보기 - 1) 메모리 크기 -> 3GB도 차고 넘침 2) GPU 속도
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it. = 파일에다가 메모리를 알려주는 정보만 하고 끝낸다.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
	{
		return false;
	}

	// Release the display mode list. = 앞에서 사용한 코드 지움 - 리소드 잡아 먹음
	delete [] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;							   /*끝*/

	// Initialize the swap chain description. /*시작*/
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));  //메모리 비우기

	// Set to a single back buffer.
    swapChainDesc.BufferCount = 1;						//backbuffer 1개 사용

	// Set the width and height of the back buffer.  -> 버퍼 사이즈
    swapChainDesc.BufferDesc.Width = screenWidth;		
    swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // DXGI_FORMAT_R8G8B8A8_UNORM -> DX에 사용하는 데이터형 크기 / UNORM -> unit normal

	// Set the refresh rate of the back buffer. -> 화면 주사율 사용하는 곳
	if(m_vsync_enabled)   //초당 몇회 뿌린다 (고정주사율).
	{
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else			// 거짓 -> 최대로 빨리 뿌린다.(최대주사율)(확인)
	{
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
    swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if(fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified. -> 픽셀 순서
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11. -> 다이렉트 버전 
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context. -> swap이 cpu에 생김
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
										   D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if(FAILED(result))
	{
		return false;
	}

	// Get the pointer to the back buffer. ->현재 주소를 가져온다.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if(FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer. -> 타겟뷰를 가르킨다. 
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if(FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it. -> cpu에 있는 것은 필요없어서 삭제
	backBufferPtr->Release();
	backBufferPtr = 0;												/*끝*/

	// Initialize the description of the depth buffer.				/*시작*/
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));			// 메모리 정리

	// Set up the description of the depth buffer. -> 화면에 3차원 깊이가 결정된다.
	depthBufferDesc.Width = screenWidth;    // 크기
	depthBufferDesc.Height = screenHeight;  // 크기
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;  // 데이터의 형
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description. -> GPU에게 device통해 알려줌
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));  /*stencil buffer 시작*/

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;  /*끝*/

	// Create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);  //OMset: cpu만든 정보를 gpu로 보낸다.

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(result))
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline. 
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView); /*끝*/

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;   // back face culling = 뒷부분을 렌더링 하지 말라. why? 성능에 영향을 끼친다.
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;	// wild모드 보는냐 solid모드 보는냐? text모드 모드냐? -> 채워서 볼거냐? 면만 볼거냐?
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if(FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	m_deviceContext->RSSetState(m_rasterState);  //RSset -> rasterization set의 약자
	
	// Setup the viewport for rendering.
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

	// Create the viewport.
    m_deviceContext->RSSetViewports(1, &viewport);

	// Setup the projection matrix.
	fieldOfView = (float)XM_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
//	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

    // Initialize the world matrix to the identity matrix.
//	m_worldMatrix = XMMatrixIdentity();

	// Create an orthographic projection matrix for 2D rendering.
//	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    return true;
}
// 초기화 끝

void D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if(m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if(m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if(m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}


void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    
	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if(m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return;
}


ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}


void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}


void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}