#include <d3dx9.h>
#include "Camera.h"

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
ID3DXMesh*				g_pTeapotMesh		= NULL ; // Hold the teapot
LPDIRECT3DVERTEXBUFFER9 g_pVB				= NULL ; // Vertex buffer
LPDIRECT3DTEXTURE9		g_pGroundTexture	= NULL ; // Texture

D3DXMATRIX				g_matView ;
D3DXVECTOR3				g_EyePt(0.0f, 20.0f, -20.0f) ;
float					g_Radius			= 20.0f ;
float					g_TotalTime			= 0.0f ;

struct Vertex
{
	float x, y, z ; // Position
	float u, v ;	// Texture coordinates
};

#define Vertex_FVF D3DFVF_XYZ | D3DFVF_TEX1

HRESULT InitD3D( HWND hWnd )
{
	// Create the D3D object, which is needed to create the D3DDevice.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		MessageBoxA(NULL, "Create D3D9 object failed!", "Error", 0) ;
		return E_FAIL;
	}

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );

	d3dpp.Windowed = TRUE; // use window mode, not full screen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE ;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16 ;

	// Create device
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		MessageBoxA(NULL, "Create D3D9 device failed!", "Error", 0) ;
		return E_FAIL;
	}

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE) ;

	// translate model to origin
	D3DXMATRIX world ;
	D3DXMatrixTranslation(&world, 0.0f, 0.0f, 0.0f) ;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &world) ;

	// set view
	D3DXVECTOR3 upVec(0.0f, 1.0f, 0.0f) ;
	D3DXVECTOR3 lookCenter(0.0f, 0.0f, 0.0f) ;

	D3DXMatrixLookAtLH(&g_matView, &g_EyePt, &lookCenter, &upVec) ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &g_matView) ;

	// set projection
	D3DXMATRIX proj ;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;

	// Create texture
	HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, "../Common/Media/ground.jpg", &g_pGroundTexture) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
	}

	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

	return S_OK;
}

VOID InitVertexBuffer()
{
	// Define vertex data
	Vertex Quad[] =
	{
		{-64.0f, 0.0f, -64.0f,  0.0f, 16.0f},	// 0
		{-64.0f, 0.0f,  64.0f,  0.0f,  0.0f},	// 1
		{ 64.0f, 0.0f, -64.0f, 16.0f, 16.0f},   // 3
		{ 64.0f, 0.0f,  64.0f, 16.0f,  0.0f},	// 2
	} ;

	// Create vertex buffer
	HRESULT hr ;
	hr = g_pd3dDevice->CreateVertexBuffer(4 * sizeof(Vertex), D3DUSAGE_WRITEONLY, 
		Vertex_FVF, D3DPOOL_MANAGED, &g_pVB, NULL) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
	}

	// Fill vertex buffer
	Vertex* v ;
	g_pVB->Lock(0, 0, (void**)&v, 0) ;
	memcpy(v, Quad, 4 * sizeof(Vertex)) ;
	g_pVB->Unlock() ;
}

void SetupEyePoint(float angle)
{
	//D3DXMATRIX matRot ;
	//D3DXMatrixRotationY(&matRot, angle) ;

	//// set view
	//D3DXVec3TransformNormal(&g_EyePt, &g_EyePt, &matRot) ;

	g_EyePt.x = 20 * cosf(angle) ;
	g_EyePt.z = 40 * sinf(angle) ;
	g_EyePt.y = 10.0f ;

	D3DXVECTOR3 upVec(0.0f, 1.0f, 0.0f) ;
	D3DXVECTOR3 lookCenter(0.0f, 0.0f, 0.0f) ;

	D3DXMatrixLookAtLH(&g_matView, &g_EyePt, &lookCenter, &upVec) ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &g_matView) ;
}

VOID RenderGround()
{
	// Disable lighting
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE) ;

	// Set texture
	g_pd3dDevice->SetTexture(0, g_pGroundTexture) ;
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(Vertex)) ;
	g_pd3dDevice->SetFVF(Vertex_FVF) ;

	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID) ;
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) ;
}


VOID Cleanup()
{
	// Release vertex buffer
	SAFE_RELEASE(g_pVB) ;

	// Release teapot mesh
	SAFE_RELEASE(g_pTeapotMesh) ;

	// Release ground texture
	SAFE_RELEASE(g_pGroundTexture) ;

	// Release device
	SAFE_RELEASE(g_pd3dDevice) ;

	// Release D3D
	SAFE_RELEASE(g_pD3D) ;
}

VOID Render(float timeDelta)
{
	g_TotalTime += timeDelta ;
	SetupEyePoint(g_TotalTime) ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		RenderGround() ;

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the back-buffer contents to the display
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
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
		break ;

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

	winClass.lpszClassName = "Teapot";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = MsgProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = NULL ;
	winClass.hIconSm	   = NULL ;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW) ; // to avoid busy cursor
	winClass.hbrBackground = NULL ;
	winClass.lpszMenuName  = NULL ;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	RegisterClassEx (&winClass) ;  

	HWND hWnd = CreateWindowEx(NULL,  
		winClass.lpszClassName,		// window class name
		"Teapot",					// window caption
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
		InitVertexBuffer() ;

		// Show the window
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

		// Enter the message loop
		MSG    msg ; 
		ZeroMemory( &msg, sizeof(msg) );
		PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

		// Get last time
		static DWORD lastTime = timeGetTime();

		while (msg.message != WM_QUIT)  
		{
			if( PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0)
			{
				TranslateMessage (&msg) ;
				DispatchMessage (&msg) ;
			}
			else // Render the game if there is no message to process
			{
				// Get current time
				DWORD currTime  = timeGetTime();

				// Calculate time elapsed
				float timeDelta = (currTime - lastTime)*0.001f;

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