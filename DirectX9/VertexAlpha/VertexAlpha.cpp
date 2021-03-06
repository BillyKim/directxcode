/*
This demo show you how to use vertex alpha
In this demo, we use vertex alpha to mix with the vertex color to 
generate the transparent effect.
*/


#include <d3dx9.h>
#include <mmsystem.h>


LPDIRECT3D9             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // Buffer to hold vertices
LPDIRECT3DTEXTURE9		g_pCubeTexture = NULL ; // Texture
DWORD					g_DiffuseColor = 0xff00ff00; // Diffuse color: green

int g_rotAxis = 0; 

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	float x, y, z;		// vertex position
	DWORD color;		// diffuse color
	float tu, tv;		// texture coordinate
};

// Vertex format
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

HRESULT InitVB();

HRESULT InitD3D( HWND hWnd )
{
	// Create the D3D object.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// Create the D3DDevice
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	InitVB();

	// Disable lighting, since the vertex contains colors
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE );

	return S_OK;
}

HRESULT InitVB()
{
	// Triangles
	CUSTOMVERTEX vertices[] =
	{
		// Bottom
		// A, B, C, 
		{ 0.0f, 0.0f, -1.0f, g_DiffuseColor },
		{ 1.0f, 0.0f, -1.0f, g_DiffuseColor },
		{ 1.0f, 0.0f,  0.0f, g_DiffuseColor },
		// A, C, D						
		{ 0.0f, 0.0f, -1.0f, g_DiffuseColor },
		{ 1.0f, 0.0f,  0.0f, g_DiffuseColor },
		{ 0.0f, 0.0f,  0.0f, g_DiffuseColor },
										
		// Top							
		// E, H, G						
		{ 0.0f, 1.0f, -1.0f, g_DiffuseColor},
		{ 0.0f, 1.0f,  0.0f, g_DiffuseColor},
		{ 1.0f, 1.0f,  0.0f, g_DiffuseColor},
		// E, G, F						
		{ 0.0f, 1.0f, -1.0f, g_DiffuseColor},
		{ 1.0f, 1.0f,  0.0f, g_DiffuseColor},
		{ 1.0f, 1.0f, -1.0f, g_DiffuseColor},
										
		// Front						
		// A, E, F						
		{ 0.0f, 0.0f, -1.0f, g_DiffuseColor },
		{ 0.0f, 1.0f, -1.0f, g_DiffuseColor },
		{ 1.0f, 1.0f, -1.0f, g_DiffuseColor },
		// A, F, B						
		{ 0.0f, 0.0f, -1.0f, g_DiffuseColor },
		{ 1.0f, 1.0f, -1.0f, g_DiffuseColor },
		{ 1.0f, 0.0f, -1.0f, g_DiffuseColor },
										
		// Back							
		// C, G, H,						
		{ 1.0f, 0.0f,  0.0f, g_DiffuseColor},
		{ 1.0f, 1.0f,  0.0f, g_DiffuseColor},
		{ 0.0f, 1.0f,  0.0f, g_DiffuseColor},
		// C, H, D,						
		{ 1.0f, 0.0f,  0.0f, g_DiffuseColor},
		{ 0.0f, 1.0f,  0.0f, g_DiffuseColor},
		{ 0.0f, 0.0f,  0.0f, g_DiffuseColor},
										
		// Left							
		// D, H, E,						
		{ 0.0f, 0.0f,  0.0f, g_DiffuseColor },
		{ 0.0f, 1.0f,  0.0f, g_DiffuseColor },
		{ 0.0f, 1.0f, -1.0f, g_DiffuseColor },
		// D, E, A						
		{ 0.0f, 0.0f,  0.0f, g_DiffuseColor },
		{ 0.0f, 1.0f, -1.0f, g_DiffuseColor },
		{ 0.0f, 0.0f, -1.0f, g_DiffuseColor },
										
		// Right						
		// B, F, G						
		{ 1.0f, 0.0f, -1.0f, g_DiffuseColor},
		{ 1.0f, 1.0f, -1.0f, g_DiffuseColor},
		{ 1.0f, 1.0f,  0.0f, g_DiffuseColor},
		// B, G, C						
		{ 1.0f, 0.0f, -1.0f, g_DiffuseColor},
		{ 1.0f, 1.0f,  0.0f, g_DiffuseColor},
		{ 1.0f, 0.0f,  0.0f, g_DiffuseColor},
	};

	// Create vertex buffer
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( sizeof(vertices),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	// Fill in date to vertex buffer
	VOID* pVertices;
	if( FAILED( g_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, vertices, sizeof(vertices) );
	g_pVB->Unlock();

	return S_OK;
}

VOID Cleanup()
{
	if( g_pVB != NULL )        
		g_pVB->Release();

	if(g_pCubeTexture != NULL)
		g_pCubeTexture->Release();

	if( g_pd3dDevice != NULL ) 
		g_pd3dDevice->Release();

	if( g_pD3D != NULL )       
		g_pD3D->Release();
}

VOID SetupMatrix()
{
	// Translate so the center of the box is the coordinate system origin.
	D3DXMATRIXA16 matWorld ;
	D3DXMatrixTranslation( &matWorld, -0.5f, -0.5f, 0.5f) ;
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// Set up view matrix
	D3DXVECTOR3 vEyePt( 0.0f, 3.0f, -5.0f );		
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );	
	D3DXVECTOR3 vUpVec( 0.0f, 3.0f, 0.0f );	
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	// Set up perspective matrix
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


void DrawCube()
{
	// Enable alpha blending
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	// Set diffuse source to vertex color, this is the default settings.
	g_pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);   //Ignored
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1);

	//Set the alpha to come completely from the diffuse
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);   //Ignored
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1);

	// How does D3D know which is source, which is dest?
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP,		D3DBLENDOP_ADD);

	g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
	g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
	g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 12 );
}

VOID Render()
{
	// Clear the back-buffer to a blue color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		SetupMatrix() ;
		
		DrawCube();

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
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
			}
		}
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

	winClass.lpszClassName = "Vertex Alpha";
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
		"Vertex Alpha",					// window caption
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
				Render() ;

				// Update last time to current time for next loop
				lastTime = currTime;
			}
		}
	}

	UnregisterClass(winClass.lpszClassName, hInstance) ;
	return 0;
}