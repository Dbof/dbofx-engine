#ifndef GLOBAL_H
#define GLOBAL_H
#pragma once

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif
#define DIRECTINPUT_VERSION 0x0800		//TODO

#include <windows.h>			//Windows functions
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <assert.h>
#include <list>
#include <string>
#include <math.h>
#include <time.h>
#include <sstream>				//utils.h
#include <iostream>				//utils.h
#include <math.h>				//sinus and so on..
#include <d3d9.h>				//Direct3D 9 header
#include <d3dx9.h>				//DirectX DX functions
#include <dinput.h>				//DirectX input

// Libraries
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

// ********** MACROS ********************
// declares copy and operator= methods without definition
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	void operator=(const TypeName&)

// defines for DLL creation
#ifdef DBOFX_EXPORTS
#define DBX_API __declspec(dllexport)
#else
#define DBX_API __declspec(dllimport)
#endif

// enable D3D Debug Info
#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

// C4251 warning
#pragma warning( disable: 4251 )

//#ifdef _DEBUG
#define DBX_INFO(msg)					core::dbError::AddError(msg, core::MT_INFO)
#define DBX_WARN(msg)					core::dbError::AddError(msg, core::MT_WARNING)
#define DBX_ERROR(msg)					core::dbError::AddError(msg, core::MT_ERROR)
/*#else
#pragma warning( disable: 4390 )	// empty instruction ";"
#define DBX_INFO(msg)					;
#define DBX_WARN(msg)					;
#define DBX_ERROR(msg)					;
#endif
*/
#define DBX_MSG_INFO(msg)				MessageBox(dbDevice::getWindow(), msg, "Information",	MB_APPLMODAL | MB_ICONINFORMATION | MB_OK)
#define DBX_MSG_WARN(msg)				MessageBox(dbDevice::getWindow(), msg, "Warning",		MB_APPLMODAL | MB_ICONWARNING | MB_OK)
#define DBX_MSG_ERROR(msg)				MessageBox(dbDevice::getWindow(), msg, "Information",	MB_APPLMODAL | MB_ICONERROR | MB_OK)
#define DBX_MSG_QUESTION(msg)			MessageBox(dbDevice::getWindow(), msg, "Question",		MB_APPLMODAL | MB_ICONQUESTION | MB_YESNO)

#define DB_SLEEP(ms)					Sleep(ms);

#define DB_MIN							( (a) < (b) ? (a) : (b))
#define DB_MAX							( (a) > (b) ? (a) : (b))
#define DB_PI							((FLOAT)  3.141592654f)
#define DB_TO_RAD(degree)				((degree) * (DB_PI / 180.0f))
#define DB_TO_DEG(radian)				((radian) * (180.0f / DB_PI))
#define DB_RAND_INT(iMin, iMax)			iMin + (rand() % (iMax - iMin + 1))
#define DB_RAND_FLT(fMin, fMax)			fMin + (fMax - fMin) * ((float)(rand() % 10001) / 10000.0f)

// Memory macros
#define DB_MEM_DELETE(x)				if (x) { delete (x); (x) = NULL; }
#define DB_MEM_DELETE_ARRAY(x)			if (x) { delete[] (x); (x) = NULL; }
#define DB_MEM_RELEASE(x)				{	if (x) { (x)->Release(); (x) = NULL; }	}

enum DB_STATE	//order does not matter :)
{
	DBS_STANDARD,
	DBS_ALPHABLEND,
	DBS_SPECULAR,
	DBS_WIREFRAME,
	DBS_FLATOBJ,
	DBS_ZBUFFER,
	DBS_LIGHTING
};

// forward declarations
namespace dbx
{
	namespace core 
	{
		class dbVector3;
		class dbColor;
		class dbIndexBuffer;
		class dbVertexBuffer;
		class dbError;
		class dbMatrix;
	}
	namespace scene 
	{
		class dbScene;
		class dbSceneMgr;
		class dbCamera;
		class dbDirectionalLight;
		class dbLight;
		class dbLightManager;
		class dbMesh;
		class dbPointLight;
		class dbSceneNode;
		class dbSkybox;
		class dbSpotLight;
		class dbTerrain;
		class dbFontObject;
	}
	namespace res 
	{
		class dbTextureManager;
	}
	namespace video 
	{
		class db2DManager;
		class dbParticleSystem;
	}
	class dbDevice;

}

#include "dbIndexBuffer.h"
#include "dbVertexBuffer.h"
#include "dbError.h"
#include "utils.h"
#include "dbVector3.h"
#include "dbColor.h"
#include "dbMatrix.h"
#include "dbParticleSystem.h"
#include "dbTerrain.h"

#include "dbTextureManager.h"
#include "dbFontObject.h"

#include "dbSceneNode.h"
#include "dbCamera.h"
#include "dbLight.h"
#include "dbDirectionalLight.h"
#include "dbSpotLight.h"
#include "dbPointLight.h"
#include "dbLightManager.h"
#include "dbMesh.h"
#include "dbSkybox.h"
#include "dbScene.h"
#include "dbSceneMgr.h"
#include "dbDevice.h"
#include "db2DManager.h"

#include "BoneHierarchyLoader.h"
#include "dbSkinnedMesh.h"

#include "dbPS.h"
#include "dbParticleRenderer.h"
#include "dbRoundPosGen.h"
#include "dbEulerUpdater.h"
#include "dbTextureRenderer.h"
#include "dbPointRenderer.h"
#include "dbBasicTimeEmitter.h"

namespace dbx
{
// Funktionsdeklarationen
DBX_API HRESULT dbMainLoop(bool (* pMoveProc)(void),
							bool (* pRenderProc)(void));
DBX_API bool dbInit();
DBX_API void dbExit();
DBX_API void dbSleep(DWORD dwMillis); 

}

#endif