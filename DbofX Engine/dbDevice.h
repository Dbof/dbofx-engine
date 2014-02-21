#ifndef DEVICE_H
#define DEVICE_H

namespace dbx
{
	struct DBX_API DEVCONFIG
	{
		struct
		{
			bool					Windowed;				// Windowed or fullscreen mode
			D3DDEVTYPE				DeviceType;				// HAL or REF
			D3DDISPLAYMODE			VideoMode;				// full screen video mode
			//KCD3DMULTISAMPLETYPE	MultiSampleType;		// FullScreenAntiAliasing (FSAA)
			DWORD					PresentInterval;		// Speed of back buffer flip
			DWORD					Flags;					// use hardware, software or a mix of both
			//KCD3DFORMAT				DepthStencilFormat;		// Z-Stencil Format
			//KCD3DFORMAT				BackBufferFormat;
			UINT					Adapter;				// number of adapter
		} Graphics;
	};

	// CLASS
	class DBX_API dbDevice
	{
	public:
		// No constructor or destructor needed
	
		// selection of options
		static bool Config(DEVCONFIG* outConfig);					

		static bool Init(const HWND ownWindow, DEVCONFIG* inConfig);	// initialize Direct3D (with own window)
		static bool InitDevice();									// initialize Direct3D objects
		static bool isInitialized();								// check if initialized
	
		static void Exit();											// shutdown device
		static bool Reset();										// reset device
		static bool ResetDevice(DEVCONFIG* pConfig);				// reset device with new device configuration
		static bool SetFullScreen(bool fullscreen);

		static HRESULT canRender();									// check if device is ready to render
		static bool StartRender();									// clear back buffer
		static bool StopRender();									// flip front and back buffer			

		static HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
		static HRESULT SetTextureStageState(int stageIndex, D3DTEXTURESTAGESTATETYPE State, DWORD Value);

		

		// window functions
		static bool setWindowTitle(char* newTitle);
		inline static void setActiveState(bool active)	{ winIsActive = active; }		// check if window active
		static bool isWindowed()	{ return dev_config.Graphics.Windowed; }


		// set* methods
		static void setBackgroundColor(core::dbColor col);
		static core::dbMatrix setProjectionMat(float FOV, float nearPlane, float farPlane);
		//static bool setMaterial(D3DMATERIAL9 mat);
		static bool setAnimRate(float a_rate);
		static bool setTransform(core::dbMatrix matTransform);
		static bool setTexture(res::TEXTURE_ITEM* texture, DWORD textureStage = 0);
	
	
		// Texture Stage 
		static bool setTextureStageStateColor(int stageIndex, int arg1, int arg2, _D3DTEXTUREOP op);
		static bool setTextureStageStateColor(int stageIndex, int arg1, int arg2, int arg3, _D3DTEXTUREOP op);
		static bool setTextureStageStateAlpha(int stageIndex, int arg1, int arg2, _D3DTEXTUREOP op);
		static bool setTextureStageStateAlpha(int stageIndex, int arg1, int arg2, int arg3, _D3DTEXTUREOP op);
		static bool setTextureStageTexCoords(int stageIndex, int indexTexCoordsToUse);
		static bool resetTextureStageState(int stageIndex = -1);	// set stageIndex to -1 for all stages


		static void enableConsole(bool state);		// enable/disable [X] button
		static bool enableFog(float fogStart, float fogEnd, core::dbColor fogColor, bool fogExponential = false);		//TODO - maybe make an enum for fog type
		static void enableFog(bool state);
		static void enableLighting(bool state);

		// get* methods
		inline static LPDIRECT3DSURFACE9	getBackBuffer()	{ return d3dbb; }
		inline static scene::dbSceneMgr*	getSceneMgr()	{ return scene_mgr; }
		inline static scene::dbScene*		getScene()		{ return scene; }
		inline static HWND					getWindow()		{ return hWindow; }
		inline static LPDIRECT3DDEVICE9		getDevice()		{ return d3ddev; }
		static float						getVideoAspect() { return (float)dev_config.Graphics.VideoMode.Width / (float)dev_config.Graphics.VideoMode.Height; }
	

	private:
		// destructor if Exit() was not called)
		~dbDevice() { Exit(); }

		static LPDIRECT3D9				d3d;			// the pointer to our Direct3D interface
		static LPDIRECT3DDEVICE9		d3ddev;			// the pointer to the device class
		static LPDIRECT3DSURFACE9		d3dbb;			// the pointer to the back buffer
		static D3DPRESENT_PARAMETERS	pres_param;	
		static DEVCONFIG				dev_config;		// save a copy of the device configuration
		static bool						isInit;
		static float					aspect;			// Width / height
		static core::dbMatrix			matProj;

		// Renderstate manager
		static bool set_value(D3DRENDERSTATETYPE State, DWORD Value);
		static std::unordered_map<D3DRENDERSTATETYPE, DWORD> render_states_map;

		// TextureState manager
		static bool set_value(int stageIndex, D3DTEXTURESTAGESTATETYPE State, DWORD Value);
		static std::unordered_multimap<int, std::pair<D3DTEXTURESTAGESTATETYPE, DWORD>> texture_states_map;

		// window settings
		static bool winIsActive;

		static HWND						hWindow;		// Window handle
		static char*					windowTitle;
		static core::dbColor			bckgrndColor;	// Surface background
		static const DWORD				style_fullscreen = WS_EX_TOPMOST | WS_POPUP;
		static const DWORD				style_windowed = WS_OVERLAPPEDWINDOW;

		// Timer
		static float					anim_rate;

		// Scene manager
		static scene::dbSceneMgr*		scene_mgr;
		static scene::dbScene*			scene;

		// private functions
		static bool InitWindow(int width, int heigth);	// create window if user does not provide own one
	};

	//	***** EXTERNAL FUNCTIONS *****
	DEVCONFIG openConfig();


	BOOL WINAPI HandlerRoutine(DWORD dwCtrlType);

} // namespace

#endif