#include "DbofX.h"


namespace dbx
{
	using namespace core;
//********** INITIALIZATION OF STATIC VARIABLES **********

LPDIRECT3D9				dbDevice::d3d = NULL;
LPDIRECT3DDEVICE9		dbDevice::d3ddev = NULL;
LPDIRECT3DSURFACE9		dbDevice::d3dbb = NULL;
D3DPRESENT_PARAMETERS	dbDevice::pres_param;	
DEVCONFIG				dbDevice::dev_config;
bool					dbDevice::isInit = false;
dbMatrix				dbDevice::matProj = dbMatrixIdentity();
float					dbDevice::aspect = 0.f;

bool					dbDevice::winIsActive = false;
HWND					dbDevice::hWindow = NULL;
char*					dbDevice::windowTitle;
dbColor					dbDevice::bckgrndColor = dbColor(0.f);
std::unordered_map<D3DRENDERSTATETYPE, DWORD> dbDevice::render_states_map;
std::unordered_multimap<int, std::pair<D3DTEXTURESTAGESTATETYPE, DWORD>> dbDevice::texture_states_map;

// Timer
float					dbDevice::anim_rate = 1.f;
scene::dbScene*			dbDevice::scene = NULL;
scene::dbSceneMgr*		dbDevice::scene_mgr = NULL;

//**********************************************************************

// Function to call when console is closed
BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
		case CTRL_C_EVENT:
			DBX_INFO("CTRL-C pressed");
			return true;
		break;

		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			if (dbDevice::isInitialized())
			{
				if (dbDevice::isWindowed())
					PostMessage(dbDevice::getWindow(), WM_DESTROY, 0, 0);
				DBX_MSG_WARN("You closed the console. You should always close the window to exit the application.");
			}
			dbExit();
			return true;
		break;
	}
		
	return true;	//false means shutting down application immediately
}

//**********************************************************************

void dbDevice::enableConsole(bool state)
{
	HWND hwd = GetConsoleWindow();
	if (hwd != NULL)
		EnableMenuItem(GetSystemMenu(hwd, false), SC_CLOSE, (state) ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_DISABLED);
}

//**********************************************************************
//WNDPROC

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool active = true;
	switch (message)
	{
		case WM_DESTROY:
			DestroyWindow(dbDevice::getWindow());
			dbDevice::enableConsole(true);
			return false;
		break;

		case WM_ACTIVATE:
			dbDevice::setActiveState(true);
			break;

		case WM_ACTIVATEAPP:
			if (!active && wParam)
			{
				dbDevice::Reset();
				active = true;
			}
			break;
		case WA_INACTIVE:
			active = false;
			break;

		case WM_CLOSE: 
			if (dbDevice::isWindowed())
			{
				if (DBX_MSG_QUESTION("Are you sure you want to exit?") == IDYES)
					PostMessage(hWnd, WM_DESTROY, 0, 0);
			}
			else
				PostMessage(GetConsoleWindow(), WM_DESTROY, 0, 0);			
			//return 0;
		break;

		default:
			// We do not want to handle this message so pass back to Windows
			// to handle it in a default way
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//**********************************************************************
// METHODS

bool dbDevice::Init(const HWND ownWindow, DEVCONFIG* inConfig)
{
	// DirectX 9
	if (!d3d)
	{
		DBX_INFO("Creating Direct3D");
		d3d = Direct3DCreate9(D3D_SDK_VERSION);
	}

	if (!d3d)
	{
		DBX_MSG_ERROR("Direct3DCreate9 failed.\nCheck if DirectX 9 or higher is installed on your computer.");
		DBX_ERROR("Direct3DCreate9 failed. Initialization aborted");
		return false;
	}
	
	// if console is closed manually, run "HandlerRoutine"
	SetConsoleCtrlHandler(HandlerRoutine, true);

	// Configuration
	if (!inConfig)
	{
		if (!Config(&dev_config))
		{
			DBX_ERROR("Configuration could not be loaded");
			return false;
		}
	}
	else
		dev_config = *inConfig;

	clear_screen();
	
#ifdef _DEBUG
	std::cout << "DbofX Engine version 0.1 - Debug build\n" << std::endl;
#else
	std::cout << "DbofX Engine version 0.1 - Release build\n" << std::endl;
#endif

	// Window
	if (!ownWindow)
	{
		if ( (InitWindow(dev_config.Graphics.VideoMode.Width, dev_config.Graphics.VideoMode.Height)) == false)
		{
			DBX_ERROR("No window could be created. Initialization aborted");
			return false;
		}
	}
	else
		hWindow = ownWindow;
	

	// D3D device
	DBX_INFO("Initialize DirectX device..");
	if (!InitDevice())
	{
		DBX_ERROR("DirectX device initialization failed. Initialization aborted");
		return false;
	}

	isInit = true;
	
	enableConsole(false);

	DBX_INFO("DirectX and its components successfully initialized");
	
	// Initialize dbSceneMgr from here
	scene_mgr = scene::dbSceneMgr::GetInstance();
	// add one scene to manager
	scene = new scene::dbScene("Default");
	return true;
}

//**********************************************************************

bool dbDevice::InitWindow(const int width, const int heigth)
{
	DBX_INFO("Creating window..");

	WNDCLASSEX wc = 
	{ 
		sizeof(WNDCLASSEX),
		CS_CLASSDC, 
		WndProc, 
		0, 0, 
		GetModuleHandle(NULL), 
		NULL, 
		0, 0, 0,
		"DbofX Engine Window",
		0
	};
	
	if (!RegisterClassEx(&wc))
	{
		DBX_ERROR("RegisterClassEx failed.");
		return false;
	}

	
	DWORD dwStyle;
	int x, y;

		dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
		x = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
		y = GetSystemMetrics(SM_CYSCREEN) / 2 - heigth / 2;

	hWindow = CreateWindow(wc.lpszClassName, 
		"DbofX Engine", 
		dwStyle, 
		x, y,
		width, heigth, 
		NULL, NULL, 
		GetModuleHandle(NULL), 
		NULL);

	if (!hWindow)
	{
		DBX_ERROR("CreateWindow failed.");
		return false;
	}
	
	ShowWindow(hWindow, SW_NORMAL);
	UpdateWindow(hWindow);

	DBX_INFO("Window successfully created.");
	return true;
}

//**********************************************************************

bool dbDevice::InitDevice()
{
#if _DEBUG
	D3DADAPTER_IDENTIFIER9 adapter;
	d3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, NULL, &adapter);
	std::cout << "\nUsed graphic adapter: \"" << adapter.Description << "\"" << std::endl;
	std::cout << "Used driver: \"" << adapter.Driver << "\"\n" << std::endl;
#endif	

	//Presentation parameters - how the device should work like	
	pres_param.Windowed = dev_config.Graphics.Windowed;
	pres_param.SwapEffect = D3DSWAPEFFECT_FLIP;
	pres_param.BackBufferCount = 1;
	pres_param.BackBufferWidth = dev_config.Graphics.VideoMode.Width;
	pres_param.BackBufferHeight = dev_config.Graphics.VideoMode.Height;
	pres_param.BackBufferFormat = dev_config.Graphics.VideoMode.Format;
	pres_param.EnableAutoDepthStencil = true;
    pres_param.AutoDepthStencilFormat = D3DFMT_D16;
	pres_param.hDeviceWindow = hWindow;
	pres_param.FullScreen_RefreshRateInHz = (dev_config.Graphics.Windowed) ? (0) : (dev_config.Graphics.VideoMode.RefreshRate);		//Has to be 0 for non-fullscreen app
	
	pres_param.PresentationInterval = dev_config.Graphics.PresentInterval;
	
	ZeroMemory(&d3ddev, sizeof(d3ddev));

	HRESULT hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pres_param, &d3ddev);
	if (FAILED(hr))
	{
		hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pres_param, &d3ddev);
		if (FAILED(hr))		// if failed again
		{
			DBX_ERROR("Direct3D Device could not be created.");
			return false;
		}
		
		DBX_INFO("Device can not run in hardware mode, now running in software mode.");
	}
	
	//d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB (255, 255, 255)); 
	SetRenderState(D3DRS_LIGHTING, true);
	SetRenderState(D3DRS_NORMALIZENORMALS, true);
	SetRenderState(D3DRS_COLORVERTEX, true);
	SetRenderState(D3DRS_SPECULARENABLE, true);
	SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	//SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// Alpha blending
	SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	SetRenderState(D3DRS_ALPHATESTENABLE, true);
	SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	SetRenderState(D3DRS_ALPHAREF, 20);

	// Z-Buffer
	SetRenderState(D3DRS_ZENABLE, true);
	SetRenderState(D3DRS_ZWRITEENABLE, true);
	SetRenderState(D3DRS_DITHERENABLE, true);
	
	// Sampler States
	d3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	d3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	d3ddev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	d3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// wrap texture coords
	d3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// wrap texture coords

	// Texture stage state
	//dbDevice::setTextureStageStateAlpha(0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);

	//Set default background color
	bckgrndColor = D3DCOLOR_XRGB(0, 0, 0);		//black

	//Clear the buffers for security reasons
	d3ddev->Clear(NULL, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, NULL);
	d3ddev->Present(0, 0, 0, 0);

	DBX_INFO("Direct3D device initialized");
	return true;
}

//**********************************************************************

bool dbDevice::isInitialized()
{
	return isInit;
}

//**********************************************************************

bool dbDevice::Config(DEVCONFIG* outConfig) 
{
	if (!outConfig)
	{
		DBX_ERROR("Config() could not find a valid configuration");
		return false;
	}
	
	if (!d3d)
		d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
	{
		DBX_MSG_ERROR("Direct3DCreate9 failed.\nCheck if DirectX 9 or higher is installed on your computer.");
		DBX_ERROR("Direct3DCreate9 failed. Initialization aborted");
		return false;
	}

	int count = d3d->GetAdapterModeCount(0, D3DFMT_X8R8G8B8);		//TODO - support of different formats
	D3DDISPLAYMODE* mode = new D3DDISPLAYMODE[count];

	std::cout << "Avaiable modes for your graphical device: " << std::endl;
	for (int i = 0; i < count; i++)
	{
		d3d->EnumAdapterModes(0, D3DFMT_X8R8G8B8, i, &mode[i]);
		std::cout << "[" << i+1 << "] " << mode[i].Width << "x" << mode[i].Height << " " << mode[i].RefreshRate << " Hz." << std::endl;
	}
	std::cout << "\n";
	int select = 0;
	do
	{
		std::cout << "Select a mode: ";
		std::cin >> select;
		if (!std::cin.fail() && (select >= 1) && (select <= count))	
			break;

		std::cin.clear();
		std::cin.ignore(256, '\n');
	} while (1);

	outConfig->Graphics.Adapter = 0;
	outConfig->Graphics.VideoMode = mode[select-1];


	//Select window/fullscreen mode
	select = 0;
	do
	{
		std::cout << "\n";
		std::cout << "[1] Run in window mode" << std::endl;
		std::cout << "[2] Run in fullscreen mode" << std::endl;

		std::cout << "\nSelect a mode: ";
		std::cin >> select;
		if (!std::cin.fail() && (select >= 1) && (select <= 2))	
			break;

		
		std::cin.clear();
		std::cin.ignore(256, '\n');
	} while (1);
	outConfig->Graphics.Windowed = (select == 1);

	// CleanUp
	DB_MEM_DELETE_ARRAY(mode);

	clear_screen();
	return true; 
}

//**********************************************************************

void dbDevice::Exit() 
{
	isInit = false;

	DB_MEM_RELEASE(d3dbb);		// back buffer
	DB_MEM_RELEASE(d3ddev);		// d3d device
	DB_MEM_RELEASE(d3d);		// d3d

	DBX_INFO("DirectX was shut down successfully");
}

//**********************************************************************

bool dbDevice::Reset() 
{
	DBX_WARN("Device lost! Trying to reset..");

	scene::dbFontObject::Lost();
	scene_mgr->OnLostDevice();
	scene::dbLightManager::GetInstance()->OnLostDevice();
	res::dbTextureManager::GetInstance()->Lost();
	video::db2DManager::GetInstance()->OnLostDevice();
	dbIndexBuffer::OnLostDevice();
	dbVertexBuffer::OnLostDevice();

	DB_MEM_RELEASE(d3dbb);

	HRESULT hr = d3ddev->Reset(&pres_param);
	if (FAILED(hr))
	{
		DBX_ERROR("Could not reset device. Reset() failed.");
		return false;
	}
	isInit;
	scene::dbFontObject::Reset();
	res::dbTextureManager::GetInstance()->Reset();
	scene_mgr->OnResetDevice();
	video::db2DManager::GetInstance()->OnResetDevice();
	dbIndexBuffer::OnResetDevice();
	dbVertexBuffer::OnResetDevice();
	scene::dbLightManager::GetInstance()->OnResetDevice();
	
	render_states_map.clear();
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &d3dbb);
	winIsActive = true;

	/*

	//d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB (255, 255, 255)); 
	d3ddev->SetRenderState(D3DRS_LIGHTING, true);
	d3ddev->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	d3ddev->SetRenderState(D3DRS_COLORVERTEX, false);
	d3ddev->SetRenderState(D3DRS_SPECULARENABLE, true);
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	d3ddev->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	//d3ddev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// Alpha blending
	d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	d3ddev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	d3ddev->SetRenderState(D3DRS_ALPHATESTENABLE, true);
	d3ddev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	d3ddev->SetRenderState(D3DRS_ALPHAREF, 20);

	// Z-Buffer
	d3ddev->SetRenderState(D3DRS_ZENABLE, true);
	d3ddev->SetRenderState(D3DRS_ZWRITEENABLE, true);
	d3ddev->SetRenderState(D3DRS_DITHERENABLE, true);
	*/

	SetRenderState(D3DRS_LIGHTING, true);
	SetRenderState(D3DRS_NORMALIZENORMALS, true);
	SetRenderState(D3DRS_COLORVERTEX, true);
	SetRenderState(D3DRS_SPECULARENABLE, true);
	SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	//SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// Alpha blending
	SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	SetRenderState(D3DRS_ALPHATESTENABLE, true);
	SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	SetRenderState(D3DRS_ALPHAREF, 20);

	// Z-Buffer
	SetRenderState(D3DRS_ZENABLE, true);
	SetRenderState(D3DRS_ZWRITEENABLE, true);
	SetRenderState(D3DRS_DITHERENABLE, true);
	
	// Sampler States
	d3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	d3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	d3ddev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	d3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// wrap texture coords
	d3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// wrap texture coords

	//Set default background color
	bckgrndColor = D3DCOLOR_XRGB(0, 0, 0);		//black

	//Reset projection matrix
	d3ddev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&matProj);

	//Clear the buffers for security reasons
	d3ddev->Clear(NULL, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, NULL);
	d3ddev->Present(0, 0, 0, 0);

	DBX_INFO("Reset was successful!");
	return true;
}

//**********************************************************************

bool dbDevice::ResetDevice(DEVCONFIG* pConfig)
{
	if (pConfig)
	{
		if (dev_config.Graphics.Windowed)
		{
			pConfig->Graphics.VideoMode.RefreshRate = NULL;			// not supported in windowed mode, has to be 0 for non-fullscreen app
			pConfig->Graphics.PresentInterval = D3DPRESENT_INTERVAL_DEFAULT;
		}

		ZeroMemory(&pres_param, sizeof(pres_param));
		pres_param.Windowed						= pConfig->Graphics.Windowed;
		pres_param.hDeviceWindow				= hWindow;
		pres_param.SwapEffect					= D3DSWAPEFFECT_DISCARD;
		pres_param.BackBufferCount				= 1;
		pres_param.BackBufferWidth				= pConfig->Graphics.VideoMode.Width;
		pres_param.BackBufferHeight				= pConfig->Graphics.VideoMode.Height;
		pres_param.BackBufferFormat				= pConfig->Graphics.VideoMode.Format;
		pres_param.EnableAutoDepthStencil		= true;
		pres_param.AutoDepthStencilFormat		= D3DFMT_D16;
		pres_param.FullScreen_RefreshRateInHz	= pConfig->Graphics.VideoMode.RefreshRate;
		pres_param.PresentationInterval			= pConfig->Graphics.PresentInterval;
		pres_param.Flags						= NULL;
	}

	HRESULT hr = d3ddev->Reset(&pres_param);
	if (FAILED(hr))
	{
		DBX_ERROR("Device could not be resetted.");
		return false;
	}

	return true;
}


//**********************************************************************

bool dbDevice::SetFullScreen(bool fullscreen)
{
	if (isWindowed())
	{
		SetWindowLong(dbDevice::getWindow(), GWL_STYLE, style_fullscreen);

		//D3DPRESENT_PARAMETERS pres_params = dbDevice::pres_param;
		pres_param.Windowed = false;
		pres_param.BackBufferWidth = dev_config.Graphics.VideoMode.Width;
        pres_param.BackBufferHeight = dev_config.Graphics.VideoMode.Height;

		bool hr = dbDevice::Reset();
		if (!hr)
		{
			// Couldn't do the change, set things back
            SetWindowLong(getWindow(), GWL_STYLE, style_windowed);
            // Set the window position
			isInit = true;
            return false;
        }
		isInit = true;
	}
	return true;
}

//**********************************************************************

HRESULT dbDevice::canRender() 
{
	if (!winIsActive)
		return D3DERR_DEVICELOST;

	HRESULT hr = d3ddev->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST)	//device was lost
	{
		winIsActive = false;
	}
	else
		winIsActive = true;

	return hr; 
}

//**********************************************************************

bool dbDevice::StartRender() 
{
	if (!winIsActive)
		return false;

	if (FAILED( d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, bckgrndColor, 1.f, 0) ))
	{
		DBX_ERROR("Surface could not be cleared!");
		return false;
	}

	if (FAILED( d3ddev->BeginScene() ))
	{
		DBX_ERROR("Could not start render!");
		return false;
	}

	return true; 
}

//**********************************************************************

bool dbDevice::StopRender() 
{
	if (!winIsActive)
		return false;

	if (FAILED( d3ddev->EndScene() ))
	{
		DBX_ERROR("Could not stop render! You have to call \"StartRender()\" first!");
		return false;
	}


	if (FAILED( d3ddev->Present(NULL, NULL, NULL, NULL) ))
	{
		DBX_ERROR("Could not present surface");
		return false;
	}

	return true; 
}

//**********************************************************************

bool dbDevice::setWindowTitle(char* newTitle) 
{
	if (!SetWindowText(hWindow, newTitle))
	{
		DBX_ERROR("Could not set new window title!");
		return false;
	}

	return true;
}

void dbDevice::setBackgroundColor(dbColor col) 
{
	bckgrndColor = col;
}
	
dbMatrix dbDevice::setProjectionMat(float FOV, float nearPlane, float farPlane) 
{
	aspect = ((float)dev_config.Graphics.VideoMode.Width / (float)dev_config.Graphics.VideoMode.Height); 
	matProj = dbMatrixProjection(DB_TO_RAD(FOV), aspect, nearPlane, farPlane);
	
	d3ddev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&matProj);
	return matProj;
}

//**********************************************************************

bool dbDevice::enableFog(float fogStart, float fogEnd, dbColor fogColor, bool fogExponential)
{
	float fogDensity = 0.005f;
	SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)&fogDensity);
	
	bool success = true;
	if (!	SetRenderState(D3DRS_FOGENABLE,  true))
		success = false;
	if (!	SetRenderState(D3DRS_FOGVERTEXMODE, (fogExponential) ? D3DFOG_EXP : D3DFOG_LINEAR))
		success = false;
	if (!	SetRenderState(D3DRS_FOGCOLOR, fogColor))
		success = false;
	if (!	SetRenderState(D3DRS_RANGEFOGENABLE, true))
		success = false;
	if (!	SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE))		// set this to something valid for pixel fog
		success = false;
	if (!	SetRenderState(D3DRS_FOGSTART, *((DWORD*)(&fogStart))))
		success = false;
	if (!	SetRenderState(D3DRS_FOGEND, *((DWORD*)(&fogEnd))))
		success = false;

	return success;
}

void dbDevice::enableFog(bool state)
{
	SetRenderState(D3DRS_FOGENABLE,  state);
}

//**********************************************************************

void dbDevice::enableLighting(bool state)
{
	SetRenderState(D3DRS_LIGHTING, state);
}

//**********************************************************************

bool dbDevice::setAnimRate(float a_rate)
{
	anim_rate = a_rate;
	return true;
}

//**********************************************************************

bool dbDevice::setTransform(const dbMatrix& matTrans)
{
	if (FAILED( d3ddev->SetTransform(D3DTS_WORLD, (const D3DMATRIX*)&matTrans) ))
		return false;
	return true;
}

//**********************************************************************

bool dbDevice::setTexture(res::TEXTURE_ITEM* texture, DWORD textureStage)
{
	if (!texture)
		return false;
	if (FAILED( d3ddev->SetTexture(textureStage, texture->Texture) ))
	{
		DBX_ERROR("Could not set texture!");
		return false;
	}
	return true;
}

//**********************************************************************

bool dbDevice::setTextureStageStateColor(int stageIndex, int arg1, int arg2, _D3DTEXTUREOP op)
{
	if (FAILED( d3ddev->SetTextureStageState(stageIndex, D3DTSS_COLORARG1, arg1) ) ||
		FAILED( d3ddev->SetTextureStageState(stageIndex, D3DTSS_COLORARG2, arg2) ) ||
		FAILED( d3ddev->SetTextureStageState(stageIndex, D3DTSS_COLOROP, op) ) )
	{
		DBX_ERROR("Could not set texture stage state!");
		return false;
	}
	return true;
}

bool dbDevice::setTextureStageStateColor(int stageIndex, int arg1, int arg2, int arg3, _D3DTEXTUREOP op)
{
	if (FAILED( d3ddev->SetTextureStageState(stageIndex, D3DTSS_COLORARG1, arg1) ) ||
		FAILED( d3ddev->SetTextureStageState(stageIndex, D3DTSS_COLORARG2, arg2) ) ||
		FAILED( d3ddev->SetTextureStageState(stageIndex, D3DTSS_COLORARG0, arg3) ) ||
		FAILED( d3ddev->SetTextureStageState(stageIndex, D3DTSS_COLOROP, op) ) )
	{
		DBX_ERROR("Could not set texture stage state!");
		return false;
	}
	return true;
}

//**********************************************************************

bool dbDevice::setTextureStageStateAlpha(int stageIndex, int arg1, int arg2, _D3DTEXTUREOP op)
{
	if (FAILED( SetTextureStageState(stageIndex, D3DTSS_ALPHAARG1, arg1) ) ||
		FAILED( SetTextureStageState(stageIndex, D3DTSS_ALPHAARG2, arg2) ) ||
		FAILED( SetTextureStageState(stageIndex, D3DTSS_ALPHAOP, op) ) )
	{
		DBX_ERROR("Could not set texture stage state!");
		return false;
	}
	return true;
}

bool dbDevice::setTextureStageStateAlpha(int stageIndex, int arg1, int arg2, int arg3, _D3DTEXTUREOP op)
{
	if (FAILED( SetTextureStageState(stageIndex, D3DTSS_ALPHAARG1, arg1) ) ||
		FAILED( SetTextureStageState(stageIndex, D3DTSS_ALPHAARG2, arg2) ) ||
		FAILED( SetTextureStageState(stageIndex, D3DTSS_ALPHAARG0, arg3) ) ||
		FAILED( SetTextureStageState(stageIndex, D3DTSS_ALPHAOP, op) ) )
	{
		DBX_ERROR("Could not set texture stage state!");
		return false;
	}
	return true;
}

//**********************************************************************

bool dbDevice::setTextureStageTexCoords(int stageIndex, int indexTexCoordsToUse)
{
	if (FAILED( SetTextureStageState(stageIndex, D3DTSS_TEXCOORDINDEX, indexTexCoordsToUse)))
	{
		DBX_ERROR("Could not set texture coordinates for the specified index!");
		return false;
	}
	return true;
}
	

//**********************************************************************


bool dbDevice::resetTextureStageState(int stageIndex)
{
	if (stageIndex > 0)
	{
		if (FAILED( SetTextureStageState(stageIndex, D3DTSS_COLOROP, D3DTOP_DISABLE)) ||
			FAILED( SetTextureStageState(stageIndex, D3DTSS_ALPHAOP, D3DTOP_DISABLE)))
		{
			DBX_ERROR("Could not reset texture stage state!");
			return false;
		}
	}

	if (stageIndex == 0)
	{
		// first one (index 0) gets special treatment (note: default values were checked with GetTextureStageSet)
		if (FAILED( SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE)) ||
			FAILED( SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE)) ||
			FAILED( SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT)))
		{
			DBX_ERROR("Could not reset texture stage state!");
			return false;
		}
	
		if (FAILED( SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1)) ||
			FAILED( SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE)) ||
			FAILED( SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT)))
		{
			DBX_ERROR("Could not reset texture stage state!");
			return false;
		}
	}

	if (stageIndex < 0)
	{
		for (int i = 1; i < 8; i++)	// devices can have up to eight set textures
		{
			if (FAILED( SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE)) ||
				FAILED( SetTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE)))
			{
				DBX_ERROR("Could not reset texture stage state!");
				return false;
			}
		}
	
		// first one (index 0) gets special treatment (note: default values were checked with GetTextureStageSet)
		if (FAILED( SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE)) ||
			FAILED( SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE)) ||
			FAILED( SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT)))
		{
			DBX_ERROR("Could not reset texture stage state!");
			return false;
		}
	
		if (FAILED( SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1)) ||
			FAILED( SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE)) ||
			FAILED( SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT)))
		{
			DBX_ERROR("Could not reset texture stage state!");
			return false;
		}
	}
	return true;
}

// Renderstate manager

HRESULT dbDevice::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	if (d3ddev == NULL)
		return S_FALSE;

	if (set_value(State, Value))
		return d3ddev->SetRenderState(State, Value);
	return S_OK;
}

bool dbDevice::set_value(D3DRENDERSTATETYPE State, DWORD Value)
{
	std::unordered_map<D3DRENDERSTATETYPE, DWORD>::iterator it = render_states_map.find(State);
	if (it == render_states_map.end())	// not found
	{
		render_states_map[State] = Value;
		return true;
	}
	if (it->second == Value)
		return false;

	it->second = Value;
	return true;
}

// Texturestate manager
HRESULT dbDevice::SetTextureStageState(int stageIndex, D3DTEXTURESTAGESTATETYPE State, DWORD Value)
{
	if (d3ddev == NULL)
		return S_FALSE;

	if (set_value(stageIndex, State, Value))
		return d3ddev->SetTextureStageState(stageIndex, State, Value);	
	return S_OK;
}

bool dbDevice::set_value(int stageIndex, D3DTEXTURESTAGESTATETYPE State, DWORD Value)
{
	using std::unordered_multimap;
	using std::pair;
	
	pair<unordered_multimap<int, pair<D3DTEXTURESTAGESTATETYPE, DWORD>>::iterator, 
		unordered_multimap<int, pair<D3DTEXTURESTAGESTATETYPE, DWORD>>::iterator> test;
	test = texture_states_map.equal_range(stageIndex);

	for (unordered_multimap<int, pair<D3DTEXTURESTAGESTATETYPE, DWORD>>::iterator it2 = test.first; it2 != test.second; ++it2)
	{
		if (it2->second.first == State)
		{
			if (it2->second.second == Value)
				return false;	// value already set
			it2->second.second = Value;
			return true;	// new value
		}
	}
	// value not found -> add value
	texture_states_map.insert(pair<int, std::pair<D3DTEXTURESTAGESTATETYPE, DWORD>>(stageIndex, pair<D3DTEXTURESTAGESTATETYPE, DWORD>(State, Value)));
	return true;
}



}	// namespace