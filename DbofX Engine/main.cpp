#include "DbofX.h"
#include <ctime>
#include <fstream>
#include <iostream>

#ifdef _DEBUG
#pragma comment(lib, "DbofX_D.lib")
#else
#pragma comment(lib, "DbofX.lib")
#endif

using std::cout;
using std::cin;
using std::endl;

using namespace dbx;
using namespace scene;
using namespace res;
using namespace core;
using namespace video;

// global objects
dbCamera* cam;
dbSkybox* skybox;
dbScene* thescene;
video::dbParticleSystem* particleSystem;
dbTerrain terrain;
dbTextureManager* TMan;
dbFontObject* Text;

particle::dbPS psystem(100000);
particle::dbTextureRenderer renderer;

dbSkinnedMesh* skinned;

std::string path_mesh = "../meshes/";

// boolean
std::string instructions = "Press F1 to enable wireframe, F2 for solid mode\n\
							Press F3 to toggle sky box\nPress F4 to toggle fog\nPress F5 to take screenshot\n\
							Press F10 to toggle terrain follow\n\
							Press F11 to toggle sun\nPress F12 to toggle strong light";

bool terrainFollow = false;
bool enable_sun = false;
bool strongLight = false;
bool drawSkybox = false;
bool fogEnabled = false;
float light_outer = DB_TO_RAD(50);
float strongLight_outer = DB_TO_RAD(90);

float slender_light = 0.f;

float MAX_WALK = 50.f;

// Game timer
LARGE_INTEGER					timerStart;
LARGE_INTEGER					timerEnd;
LARGE_INTEGER					timerFreq;
float							anim_rate = 1.f;
float mid = 0.1f;

void initTexture()
{
	TMan->AddTexture(TEX_2D, path_mesh+"water.jpg", true, true, dbColor(0.f));
	TMan->AddTexture(TEX_2D, path_mesh+"rock.jpg", true, true);
	TMan->AddTexture(TEX_2D, path_mesh+"wall.jpg", true, true);
	TMan->AddTexture(TEX_2D, path_mesh+"ando.png", true, true, dbColor(1.f, 0.f, 1.f));
	TMan->AddTexture(TEX_2D, path_mesh+"andoisawesome.png", true, true, dbColor(1.f, 0.f, 1.f));
	TMan->AddTexture(TEX_2D, path_mesh+"slender.png", true, true);
	TMan->AddTexture(TEX_2D, path_mesh+"fire.png", true, true, dbColor(1.f));
}

void InitMesh()
{
	thescene->AddMesh("Office", path_mesh+"office.x")->Scale(dbVector3(10.f, 10.f, 10.f));
	skybox = thescene->AddSkybox(path_mesh+"cloudy.dds");

	dbMesh* male = thescene->AddMesh("Sonic", path_mesh+"sonic.x");
	male->Scale(dbVector3(5.f, 5.f, 5.f));
	male->SetPos(dbVector3(-10.f, 0.f, 0.f));
	male->RotateY(DB_TO_RAD(180));

	skinned = new dbSkinnedMesh("Animated Girl");
	skinned->Init(path_mesh+"anim_girl.x");
	skinned->Scale(dbVector3(0.25f,0.25f,0.25f));
	skinned->SetPos(dbVector3(0.f, 7.f, 0.f));
}

bool Exit()
{
	if (!dbDevice::Reset())
		return false;

	InitMesh();
	return true;
}

bool Move()
{
	QueryPerformanceCounter(&timerStart);

	if (GetAsyncKeyState(VK_ESCAPE))
		return false;

	if (GetAsyncKeyState(VK_F1))
		dbDevice::SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	if (GetAsyncKeyState(VK_F2))
		dbDevice::SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	if (GetAsyncKeyState(VK_F3))
	{
		drawSkybox = !drawSkybox;
		while (GetAsyncKeyState(VK_F3));
	}
	if (GetAsyncKeyState(VK_F4))
	{
		fogEnabled = !fogEnabled;
		dbDevice::enableFog(fogEnabled);
		while (GetAsyncKeyState(VK_F4));
	}

	if (GetAsyncKeyState(VK_F5))
	{
		thescene->SaveScreenshot("screenshot\\screenshot.jpg");
		while (GetAsyncKeyState(VK_F5));
	}
	if (GetAsyncKeyState(VK_F6))
	{
		particleSystem->AddParticle(10.f, cam->GetPos() + cam->GetDirection(),
			cam->GetDirection() * 10.f, 1.5f, 10.f, 1.f, dbColor(1.f, 1.f, 1.f, 0.2f), dbColor(1.f, 1.f, 1.f, 0.f));
	}

	if (GetAsyncKeyState(VK_F10))
	{
		terrainFollow = !terrainFollow;
		while (GetAsyncKeyState(VK_F10));
	}
	if (GetAsyncKeyState(VK_F11))
	{
		enable_sun = !enable_sun;
		thescene->GetLight("Sun")->SetVisible(enable_sun);
		while (GetAsyncKeyState(VK_F11));
	}
	if (GetAsyncKeyState(VK_F12))
	{
		strongLight = !strongLight;
		
		dbSpotLight* sl = static_cast<dbSpotLight*>(thescene->GetLight("Spot"));
		if (strongLight)
		{
			sl->SetDiffuseColor(dbColor(10.f));
			sl->SetOuterAngle(strongLight_outer);
		}
		else
		{
			sl->SetDiffuseColor(dbColor(1.f));
			sl->SetOuterAngle(light_outer);
		}
		while (GetAsyncKeyState(VK_F12));
	}

	if (GetAsyncKeyState('R'))
	{
		cam->SetDirection(dbVector3(0.f, 0.f, 1.f));
		cam->SetPos(dbVector3(0.f, 10.f, -20.f));
	}

	if (GetAsyncKeyState(VK_SPACE))
	{
		// Tiny says no to the camera
		static int f = 0;
		static boolean toright = true;

		Bone* b = skinned->FindBone("Armature_testa");
		if (b != NULL)
		{
			skinned->SetBoneTransform(b, dbMatrixRotationY(DB_TO_RAD(f)), true);

			if (toright)
				f += 4;
			else
				f -= 4;

      		if (f >= 10)
				toright = false;
			if (f <= -10)
				toright = true;
		}
	}

	/*
	// particle system
	int max = getRandomInt(5000, 5000);
	for (int i = 0; i < max; i++)
		particleSystem->AddParticle(1.5f,
		dbVector3(getRandomFloat(-50, 50), getRandomFloat(9.f, 30.f, 2), getRandomFloat(-50.f, 50.f)), 
		dbVector3(0.f, -10.f, 0.f), 1.f,
		0.2f, 0.1f, 
		dbColor(1.f), dbColor(0.5f)); 
	particleSystem->Move(anim_rate);
	*/
	psystem.Update(anim_rate);

	// camera movement
	if (GetAsyncKeyState(VK_UP))
	{
		cam->MoveUp(MAX_WALK);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		cam->MoveUp(-MAX_WALK);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		cam->RotateY(DB_TO_RAD(+60));
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		cam->RotateY(DB_TO_RAD(-60));
	}

	if (GetAsyncKeyState('W'))
	{
		cam->Move(cam->GetDirection(), +MAX_WALK);
	}
	if (GetAsyncKeyState('S'))
	{
		cam->Move(cam->GetDirection(), -MAX_WALK);
	}
	if (GetAsyncKeyState('A'))
	{
		cam->MoveRight(-MAX_WALK);
	}
	if (GetAsyncKeyState('D'))
	{
		cam->MoveRight(+MAX_WALK);
	}

	// move light with camera
	thescene->GetLight("Spot")->SetPos(cam->GetPos());
	thescene->GetLight("Spot")->SetDirection(cam->GetDirection()+ dbVector3(0.f, 0.1f, 0.f));

	if (GetAsyncKeyState(VK_LSHIFT))
		cam->RotateX(DB_TO_RAD(-90));
	if (GetAsyncKeyState(VK_LCONTROL))
		cam->RotateX(DB_TO_RAD(90));

	if (terrainFollow)
	{
		cam->SetPos((dbVector3(cam->GetPos().x/2, terrain.GetHeight(cam->GetPos()/2)+10.f, cam->GetPos().z/2)) * 2);
	}

	// calc slender light
	dbVector3 camera_pos = cam->GetPos();
	dbVector3 slender_pos = dbVector3(256.f, 6.f, 256.f);
	float distance = sqrtf( pow(camera_pos.x - slender_pos.x, 2) + pow(camera_pos.z - slender_pos.z, 2) );
	if (distance < 400.f)
		slender_light = (200-distance) / 400.f;
	else
		slender_light = 0.f;

	cam->Update(anim_rate);
	skinned->Animate(anim_rate);
	return true;
}

bool Render()
{
	dbDevice::StartRender();

	// sky box
	if (drawSkybox)
		skybox->Render(cam->GetPos());
	
	// terrain
	dbDevice::setTransform(dbMatrixTranslation(dbVector3(0.f, 0.f, 0.f)) * dbMatrixScaling(dbVector3(2.f, 2.f, 2.f)));
	dbDevice::setTexture(TMan->SearchTexture("rock.jpg"));
	terrain.Render();
	
	// Office
	dbDevice::setTransform(thescene->GetMesh("Office")->GetMatrix());
	dbDevice::setTexture(TMan->SearchTexture("wall.jpg"));
	thescene->GetMesh("Office")->Render(-1, true, false);

	// Sonic
	dbMesh* sonic = thescene->GetMesh("Sonic");
	dbDevice::setTransform(sonic->GetMatrix());
	dbDevice::setTexture(NULL);
	sonic->Render(-1, true, true);

	// skinned girl
	dbDevice::setTransform(skinned->GetMatrix());	// dbMatrixIdentity()
	skinned->Render(NULL);

	// android text
	video::db2DManager::GetInstance()->SetScaling(dbVector3(0.009f, 0.009f, 0));
	video::db2DManager::GetInstance()->DrawBillboard(TMan->SearchTexture("andoisawesome.png"), NULL, dbVector3(2, 7.f, 2));

	// Android
	video::db2DManager::GetInstance()->SetScaling(dbVector3(0.007f, 0.007f, 0));
	video::db2DManager::GetInstance()->DrawBillboard(TMan->SearchTexture("ando.png"), NULL, dbVector3(2, 2.f, 2));

	// Slenderman
	video::db2DManager::GetInstance()->SetScaling(dbVector3(0.04f, 0.04f, 0));
	video::db2DManager::GetInstance()->DrawBillboard(TMan->SearchTexture("slender.png"), NULL, dbVector3(256, 6.f, 256.f), dbColor(slender_light), true);

	/*
	// rain (Particle system)
	dbDevice::setTransform(dbMatrixIdentity());
	dbDevice::setTexture(TMan->SearchTexture("water.jpg"));
	particleSystem->Render();
	*/
	
	dbDevice::setTransform(dbMatrixIdentity());
	dbDevice::setTexture(TMan->SearchTexture("fire.png"));
	renderer.Render();
	

	// timer calculates fps
	static int fps = 0;	static int timer = 0;
	if (timer++ % 10 == 0)
		fps = (int) (1 / anim_rate) ;

	// draw some data on-screen
	dbLight* red = thescene->GetLight("Redlight");
	Text->Draw("FPS: " + toString( fps ), dbColor(1.f), 10, 10, -1, -1);
	Text->Draw("Particle number: " + toString( (int)psystem.GetAliveParticleCount() ), dbColor(0.f, 0.25f, 1.f), 30, 10, -1, -1);
	Text->Draw(instructions, dbColor(1.f, 0.f, 0.f), 50, 10, -1, -1);
	
	// draw everything
	dbDevice::StopRender();
	
	// calculate animation rate
	QueryPerformanceCounter(&timerEnd);
	anim_rate = ((float)(timerEnd.QuadPart - timerStart.QuadPart) / timerFreq.QuadPart); 
	return true;
}

int main()
{
	srand((unsigned)time(0));
	QueryPerformanceFrequency(&timerFreq);
	
	// setup device
	dbDevice::Init(NULL, NULL);
	dbDevice::setBackgroundColor(dbColor(0.f, 0.f, 0.f));
	dbDevice::setProjectionMat(60, 0.5f, 500.f);
	dbDevice::enableFog(10.f, 200.f, dbColor(-10.f), false);
	dbDevice::enableFog(false);

	// scene
	thescene = dbDevice::getScene();

	TMan = dbTextureManager::GetInstance();
	TMan->Init(25);		// max textures
	Text = new dbFontObject();
	if (!Text->InitFromDialog())
		Text->Init("Comic Sans MS", 20, false, false);

	InitMesh();
	initTexture();

	// camera
	cam = thescene->AddCamera("Camera", dbVector3(0.f, 10.f, -20.f), dbVector3(0.f, 0.f, 1.f));

	//// init the "sun"
	thescene->AddDirectionalLight("Sun", dbVector3(0.f, -1.f, 0.f), dbColor(1.f), dbColor(0.f), dbColor(0.25f));
	thescene->GetLight("Sun")->SetVisible(enable_sun);

	thescene->AddSpotLight("Spot", dbVector3(0.f, 0.f, 0.f), dbVector3(0.f, 1.f, 1.f), 500.f, DB_TO_RAD(40), light_outer, dbColor(2.f), dbColor(1.f), dbColor(0.2f));
	thescene->GetLight("Spot")->SetVisible(true);

	// init a point light
	thescene->AddPointLight("Redlight", dbVector3(10.f, 20.f, 0.f), 50.f, dbColor(1.f, 0.f, 0.f), dbColor(0.f), dbColor(0.f));
	thescene->GetLight("Redlight")->SetVisible(true);

	// init particle system
	particleSystem = video::dbParticleSystem::GetInstance();
	particleSystem->Init(6000);
	
	
	std::shared_ptr<particle::dbParticleEmitter> emitter(new particle::dbBasicTimeEmitter(5000));
	std::shared_ptr<particle::dbEulerUpdater> updater(new particle::dbEulerUpdater());
	std::shared_ptr<particle::dbRoundPosGen> gen(new particle::dbRoundPosGen(dbVector3(0.f, 0.f, 0.f), 10.f, 10.f));
	emitter->AddGenerator(gen);
	psystem.AddEmitter(emitter);
	psystem.AddUpdater(updater);

	renderer.Generate(&psystem, false);
	renderer.Update();
	
	for (int i = 0; i < 200; i++)
		particleSystem->AddParticle(getRandomFloat(0.5f, 4.f),
			dbVector3(getRandomFloat(-1.f, 1.f, 2), 0.02f, 0), 
			dbVector3(getRandomFloat(-1.f, 1.f), getRandomFloat(0.5f, 1.5f, 2), 0), 1.f,
			0.05f, 0.5f, dbColor(1.f), dbColor(0.2f));

	// terrain
	bool result = terrain.Init((path_mesh+"heightmap.bmp").c_str(), HT_BMP, 15.f);
	if (!result)
	{
		dbExit();
		return -1;
	}

	dbMainLoop(Move, Render);

	DB_MEM_DELETE(skinned);
	DB_MEM_DELETE(Text);

	std::cout << "Application ended!!\n\n";
	return 0;
}