#include "DbofX.h"


void dbDevice::OnDeviceLost()
{
	if (hWindow != GetForegroundWindow())
	{
		return;
	}
	SetForegroundWindow(hWindow);
	SetFocus(hWindow);

	HRESULT hr = d3ddev->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST)
	{
		Sleep(50);
		hr = d3ddev->TestCooperativeLevel();
	}
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		d3ddev->Reset(&pres_param);
		Sleep(200);
		hr = d3ddev->TestCooperativeLevel();
	}
	
	if (hr != D3DERR_DEVICENOTRESET && hr != D3DERR_DEVICELOST)
	{
		DBX_INFO("Device was recovered successfully!");
		dev_lost = false;
	}
}


// Function to call when console is closed
BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	//TODO - fix it!

	//if (dwCtrlType == CTRL_CLOSE_EVENT)
	//	delete this_device;
	return true;
}


//WNDPROC
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
				PostQuitMessage(0);
		break;

		default:
			// We do not want to handle this message so pass back to Windows
			// to handle it in a default way
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



dbDevice::~dbDevice()
{
	Stop();
}
	
dbDevice *dbDevice::createDevice(DEVCONFIG config, WindowType type)
{
#ifdef _DEBUG
	std::cout << "DbofX Engine version 0.1 - Debug build\n" << std::endl;
#else
	std::cout << "DbofX Engine version 0.1 - Release build\n" << std::endl;
#endif

	static dbDevice dev(config, type);
	return &dev;
}

dbDevice *dbDevice::createDevice(HWND customWindow)
{
#ifdef _DEBUG
	std::cout << "DbofX Engine version 0.1 - Debug build\n" << std::endl;
#else
	std::cout << "DbofX Engine version 0.1 - Release build\n" << std::endl;
#endif

	hWindow = customWindow;
	static dbDevice dev(customWindow);
	return &dev;
}

dbDevice::dbDevice(DEVCONFIG config, WindowType type)
{
	// dbError initialization
	dbError::Run();

	
	disp_mode = config;
	wt = type;

	ZeroMemory(&pres_param, sizeof(pres_param));
	pres_param.Windowed = (type == WT_DESKTOP);


	d3d = NULL;

	if (!cWindow)
	{
		if (!InitWindow(config.Width, config.Height, "Test", NULL))
			return;
	}
	

	if (!InitD3D())
		return;

	scene = new dbScene(d3ddev);
}


dbDevice::dbDevice(HWND customWindow)
{
	// dbError initialization
	dbError::Run();

	// Use own window
	cWindow = true;
	hWindow = customWindow;

	if (customWindow == NULL)
		DBX_WARN("Specified Window was not found. Check if HWND is valid.");

	ZeroMemory(&pres_param, sizeof(pres_param));
	
	d3d = NULL;

	pres_param.Windowed = true;
	

	// set custom display mode
	disp_mode = openConfig();
		
	if (!InitD3D())
		return;

	scene = new dbScene(d3ddev);
}

void dbDevice::Stop(void)
{
	if (d3ddev)
	{
		d3ddev->Release();
		d3ddev = NULL;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = NULL; 
	}

	if (scene)
	{
		delete scene;
		scene = NULL;
	}
}


//***** PRIVATE FUNCTIONS *****

bool dbDevice::InitWindow(int Width, int Height, char* title, HICON hIcon)
{

	WNDCLASSEX wc = 
	{ 
		sizeof(WNDCLASSEX),
		CS_CLASSDC, 
		WndProc, 
		0, 0, 
		GetModuleHandle(NULL), 
		hIcon, 
		0, 0, 0,
		"DbofX Engine Window",
		0
	};
	
	if (FindWindow(wc.lpszClassName, NULL))
		DestroyWindow(hWindow);
	
	UnregisterClass(wc.lpszClassName, NULL);
	if (!RegisterClassEx(&wc))
	{
		DBX_ERROR("RegisterClassEx failed.");
		return false;
	}

	
	DWORD dwStyle;
	int x, y;
	if (wt == WT_DESKTOP)
	{
		dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
		x = GetSystemMetrics(SM_CXSCREEN) / 2 - Width / 2;
		y = GetSystemMetrics(SM_CYSCREEN) / 2 - Height / 2;
	}
	else
	{
		dwStyle = WS_EX_TOPMOST | WS_POPUP;
		x = y = 0;
	}


	hWindow = CreateWindow(wc.lpszClassName, 
		title, 
		dwStyle, 
		x, y,
		Width, Height, 
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


bool dbDevice::InitD3D(void)
{
	//Putting this here since it's the only function called by all the constructors
	SetConsoleCtrlHandler(HandlerRoutine, true);


	// Now the real InitD3D func:

	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (FAILED(d3d))
	{
		DBX_ERROR("Could not create the D3D device.");
		return false;
	}

#if _DEBUG
	D3DADAPTER_IDENTIFIER9 adapter;
	d3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, NULL, &adapter);
	std::cout << "\nUsed graphic adapter: \"" << adapter.Description << "\"" << std::endl;
	std::cout << "Used driver: \"" << adapter.Driver << "\"" << std::endl;
#endif	

	//Presentation parameters - how the device should work like	
	pres_param.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pres_param.BackBufferCount = 1;
	pres_param.BackBufferWidth = disp_mode.Width;
	pres_param.BackBufferHeight = disp_mode.Height;
	pres_param.BackBufferFormat = (cWindow) ? D3DFMT_UNKNOWN : D3DFMT_X8R8G8B8;
	pres_param.EnableAutoDepthStencil = true;
    pres_param.AutoDepthStencilFormat = D3DFMT_D16;
	pres_param.hDeviceWindow = hWindow;
	pres_param.FullScreen_RefreshRateInHz = (wt == WT_DESKTOP) ? (0) : (disp_mode.RefreshRate);		//Has to be 0 for non-fullscreen app
	
	pres_param.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	
	ZeroMemory(&d3ddev, sizeof(d3ddev));

	// TODO - change this to D3DCREATE_HARDWARE_VERTEXPROCESSING when releasing
	HRESULT hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pres_param, &d3ddev);
	if (FAILED(hr))
	{
		hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pres_param, &d3ddev);
		if (FAILED(hr))		// if failed again
		{
			DBX_ERROR("Device could not be created.");
			return false;
		}
		
		DBX_INFO("Device can not run in hardware mode, now running in software mode.");
	}
	
	//d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB (255, 255, 255)); 
	SetRenderState(D3DRS_LIGHTING, true);
	SetRenderState(D3DRS_NORMALIZENORMALS, true);
	SetRenderState(D3DRS_COLORVERTEX, false);
	SetRenderState(D3DRS_SPECULARENABLE, true);
	SetRenderState(thescene, D3DCULL_NONE);

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

	//Set default background color
	bckgrnd_col = D3DCOLOR_XRGB(0, 0, 0);		//black

	//Clear the buffers for security reasons
	d3ddev->Clear(NULL, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, NULL);
	d3ddev->Present(0, 0, 0, 0);

#ifdef _DEBUG
	DBX_INFO("Direct3D initialized!");
#endif
	return true;
}


// ********** CLASS METHODS **********
void dbDevice::setWindowTitle(char* newTitle)
{
	SetWindowText(hWindow, newTitle);
}


void dbDevice::setBackgroundColor(dbColor col)
{
	bckgrnd_col = col;
}



void dbDevice::setState(DB_STATE state, bool enable)
{
	switch (state)
	{
	case DBS_WIREFRAME:	
		SetRenderState(D3DRS_FILLMODE, (enable) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	break;

	case DBS_ALPHABLEND:
		SetRenderState(D3DRS_ALPHABLENDENABLE, enable);
	break;

	case DBS_FLATOBJ:
		SetRenderState(D3DRS_SHADEMODE, (enable) ? D3DSHADE_FLAT : D3DSHADE_GOURAUD);
	break;

	case DBS_SPECULAR:
		SetRenderState(D3DRS_SPECULARENABLE, enable);
	break;
	}
}


void dbDevice::draw()
{
	if (!d3ddev)
		return;
	
	if (dev_lost)
	{
		OnDeviceLost();
		return;
	}
	
	d3ddev->Clear(NULL, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, bckgrnd_col, 1.0f, NULL);

	if (GetAsyncKeyState(VK_F1))
		d3ddev->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
	
	if (GetAsyncKeyState(VK_F2))
		d3ddev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);


	HRESULT dev_state = scene->draw();
	if (dev_state == D3DERR_DEVICELOST)
	{
		DBX_WARN("Device lost. Trying to recover it now.");
		dev_lost = true;
	}
}


// ********** EXTERNAL FUNCTIONS **********
DEVCONFIG openConfig()
{
	LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);

	int count = d3d->GetAdapterModeCount(0, D3DFMT_X8R8G8B8);
	D3DDISPLAYMODE* mode = new D3DDISPLAYMODE[count];
	D3DDISPLAYMODE s;

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

	d3d->Release();

	s = mode[select-1];
	delete[] mode;

	clear_screen();

	return s;
}