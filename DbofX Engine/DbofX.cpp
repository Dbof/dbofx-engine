#include "DbofX.h"

namespace dbx
{

DBX_API HRESULT dbMainLoop(bool (* pMoveProc)(void), bool (* pRenderProc)(void))
{
	DBX_INFO("Entered Main Loop.");
	MSG msg;

	while (1)
	{
		// Handle Windows messages
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{ 
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
			{
				DBX_INFO("Exited Main Loop.");
				dbExit();	// This cleanup is more secure
				return S_OK;
			}
		} 

		HRESULT hr = dbDevice::canRender();
		if (hr == D3D_OK)
		{
			if (pMoveProc)
			{
				if (!pMoveProc())
					break;
			}

			// check again status
			if (D3D_OK != (hr = dbDevice::canRender()))
				continue;

			if (pRenderProc)
			{
				if (!pRenderProc())
					break;
			}
		}
		else if (hr == D3DERR_DEVICENOTRESET)
		{
			//Reset device
			dbDevice::Reset();
		}
		
		else
			Sleep(200);		//Give other processes CPU time
	}
	
	DBX_INFO("Main loop exited");
	dbExit();
	return S_OK;
}

DBX_API void dbExit()
{
	res::dbTextureManager::GetInstance()->Exit();
	scene::dbFontObject::Exit();
	scene::dbSceneMgr::GetInstance()->Exit();
	video::db2DManager::GetInstance()->Exit();
	video::dbParticleSystem::Exit();

	if (dbDevice::isInitialized())
		dbDevice::Exit();
}

}		// namespace