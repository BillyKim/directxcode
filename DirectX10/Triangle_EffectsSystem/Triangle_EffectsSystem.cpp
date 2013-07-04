/*
Description:  This demo show you how to draw a simple triangle in D3D10, we use a pure hlsl shader file without effect system here.
Date: 2013-06-30
Author: zdd
*/

#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <d3dcompiler.h>

// DirectX Globals
ID3D10Device*			g_pd3dDevice		= NULL;
IDXGISwapChain*			g_pSwapChain		= NULL;
ID3D10RenderTargetView* g_pRenderTargetView = NULL;
ID3D10InputLayout*		g_pVertexLayout		= NULL;
ID3D10Buffer*			g_pVertexBuffer     = NULL;
ID3D10Effect*           g_pEffect			= NULL;
ID3D10EffectTechnique*  g_pTechnique		= NULL;

// Is window active?
bool					g_bActive			= true ; 

// The vertex format
struct SimpleVertex
{
	D3DXVECTOR3 Pos;
};

// Safe release a COM object
#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

// Forward declaration
VOID CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
VOID InitVertexBuffer();
VOID InitEffects();

HRESULT InitD3D( HWND hWnd )
{
	// Setup a DXGI swap chain descriptor
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );

	sd.BufferCount = 1; // number of buffer
	sd.BufferDesc.Width = 600; // buffer width, can we set it to the screen width?
	sd.BufferDesc.Height = 600; // buffer height, can we set it to the screen height?
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // buffer format, 32 bit color with alpha(RGBA)
	sd.BufferDesc.RefreshRate.Numerator = 60; // refresh rate?
	sd.BufferDesc.RefreshRate.Denominator = 1; // WHAT'S THIS?
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // use buffer as render target
	sd.OutputWindow = hWnd; // output window handle
	sd.SampleDesc.Count = 1; // WHAT'S THIS?
	sd.SampleDesc.Quality = 0; // WHAT'S THIS?
	sd.Windowed = TRUE; // full-screen mode

	// Create device and swap chain
	HRESULT hr;
	if (FAILED (hr = D3D10CreateDeviceAndSwapChain( NULL, 
	    D3D10_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		D3D10_SDK_VERSION,
		&sd, 
		&g_pSwapChain,
		&g_pd3dDevice)))
	{
		return hr;
	}

	// Create render target and bind the back-buffer
	ID3D10Texture2D* pBackBuffer;

	// Get a pointer to the back buffer
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID* )&pBackBuffer);
	if (FAILED(hr))
		return hr;

	// Create a render-target view
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);

	// Bind the view
	g_pd3dDevice->OMSetRenderTargets(1, &g_pRenderTargetView, NULL); // WHAT'S OM here mean?

	// Setup the viewport
	D3D10_VIEWPORT vp;
	vp.Width = 640; // this should be similar with the back-buffer width, global it!
	vp.Height = 480;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pd3dDevice->RSSetViewports(1, &vp);

	InitVertexBuffer();
	InitEffects();
		                       
	return S_OK;
}

VOID InitVertexBuffer()
{
	// Create the vertex buffer
	SimpleVertex vertices[] = 
	{
		D3DXVECTOR3(0.0f, 0.5f, 0.5f),
		D3DXVECTOR3(0.5f, -0.5f, 0.5f),
		D3DXVECTOR3(-0.5f, -0.5f, 0.5f),
	};

	// Vertex Buffer
	D3D10_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D10_USAGE_DEFAULT;		// Buffer usage
	bd.ByteWidth = sizeof(SimpleVertex) * 3;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	// copy vertex buffer data
	D3D10_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	HRESULT	hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &g_pVertexBuffer);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create vertex buffer failed", L"Error", 0);
	}
}

VOID InitEffects()
{
	ID3DBlob* pErrorBlob;

	// Create the effect
    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3D10_SHADER_DEBUG;
    #endif

	// Compile the effects file
    HRESULT hr = D3DX10CreateEffectFromFile( L"triangle_shader.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0,
                                         g_pd3dDevice, NULL, NULL, &g_pEffect, &pErrorBlob, NULL);

    // Output the error message if compile failed
	if(FAILED(hr))
    {
        if(pErrorBlob != NULL)
		{
			OutputDebugString((WCHAR*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
    }

	// Release the Blob
	if(pErrorBlob)
		pErrorBlob->Release();

    // Obtain the technique
    g_pTechnique = g_pEffect->GetTechniqueByName("Render");

    // Define the input layout
    D3D10_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = sizeof(layout) / sizeof(layout[0]);

    // Create the input layout
    D3D10_PASS_DESC PassDesc;
    g_pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    hr = g_pd3dDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature,
                                          PassDesc.IAInputSignatureSize, &g_pVertexLayout);
    if(FAILED(hr))
	{
		MessageBox(NULL, L"Create input layout failed", L"Error", 0);
	}
}

VOID Cleanup()
{
	SAFE_RELEASE( g_pVertexBuffer);
	SAFE_RELEASE( g_pVertexLayout);
	SAFE_RELEASE( g_pRenderTargetView );
	SAFE_RELEASE( g_pSwapChain ) ;
	SAFE_RELEASE( g_pd3dDevice ) ;
}

VOID Render(float timeDelta)
{
	if (!g_bActive)
		Sleep(50) ;

	// Clear the back-buffer to a BLUE color
	float color[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	g_pd3dDevice->ClearRenderTargetView(g_pRenderTargetView, color);

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pd3dDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// Set the input layout
	g_pd3dDevice->IASetInputLayout(g_pVertexLayout);

	// Set geometry type
	g_pd3dDevice->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Apply each pass in technique and draw triangle.
	D3D10_TECHNIQUE_DESC techDesc;
	g_pTechnique->GetDesc(&techDesc);
	for (unsigned int i = 0; i < techDesc.Passes; ++i)
	{
		g_pTechnique->GetPassByIndex(i)->Apply(0);
		g_pd3dDevice->Draw(3, 0);
	}

	// Present the sence from back buffer to front buffer
	g_pSwapChain->Present(0, 0);
}

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_ESCAPE:
				SendMessage( hWnd, WM_CLOSE, 0, 0 );
				break ;
			default:
				break ;
			}
		}
		return 0 ;

	case WM_SIZE:
		if(wParam == SIZE_MAXHIDE || wParam == SIZE_MINIMIZED)
			g_bActive = false;
		else
			g_bActive = true;
		return 0;

	case WM_ACTIVATEAPP:
		if(wParam == TRUE)
			g_bActive = true ;
		else
			g_bActive = false ;
		return 0 ;

	case WM_DESTROY:
		Cleanup();
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR szCmdLine, int iCmdShow)
{
	WNDCLASSEX winClass ;

	winClass.lpszClassName = L"Triangle";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = MsgProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = NULL ;
	winClass.hIconSm	   = NULL ;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW) ;
	winClass.hbrBackground = NULL ;
	winClass.lpszMenuName  = NULL ;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	RegisterClassEx (&winClass) ;  

	HWND hWnd = CreateWindowEx(NULL,  
		winClass.lpszClassName,		// window class name
		L"Triangle",				// window caption
		WS_OVERLAPPEDWINDOW, 		// window style
		32,							// initial x position
		32,							// initial y position
		600,						// initial window width
		600,						// initial window height
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL) ;						// creation parameters

	// Create window failed
	if(hWnd == NULL)
	{
		MessageBoxA(hWnd, "Create Window failed!", "Error", 0) ;
		return -1 ;
	}

	// Initialize Direct3D
	if( SUCCEEDED(InitD3D(hWnd)))
	{ 
		// Show the window
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

		MSG msg ; 
		ZeroMemory( &msg, sizeof(msg) );
		PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

		// Get last time
		static DWORD lastTime = timeGetTime();

		while (msg.message != WM_QUIT)  
		{
			if(PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0)
			{
				TranslateMessage (&msg) ;
				DispatchMessage (&msg) ;
			}
			else // Render the game if there is no message to process
			{
				// Get current time
				DWORD currTime  = timeGetTime();

				// Calculate time elapsed
				float timeDelta = (currTime - lastTime) * 0.001f;

				// Render
				Render(timeDelta) ;

				// Update last time to current time for next loop
				lastTime = currTime;
			}
		}
	}

	UnregisterClass(winClass.lpszClassName, hInstance) ;
	return 0;
}
