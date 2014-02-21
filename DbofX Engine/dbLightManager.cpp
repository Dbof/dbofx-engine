#include "DbofX.h"

using std::string;
using std::unordered_map;

namespace dbx
{
namespace scene
{
	using namespace core;

dbLightManager* dbLightManager::instance_ = NULL;

dbLightManager::dbLightManager()
{
	// TODO: device caps in device manager
	D3DCAPS9 test;
	dbDevice::getDevice()->GetDeviceCaps(&test);
	max_light_count_ = test.MaxActiveLights;
}

void dbLightManager::OnLostDevice()
{
}

void dbLightManager::OnResetDevice()
{
	for (std::unordered_map<string, dbLight*>::const_iterator iterator = lights_map_.begin(); iterator != lights_map_.end(); ++iterator) 
	{
		dbLight* light = (iterator->second);
		dbDevice::getDevice()->SetLight(light->d3d_index_, &light->d3d_light_);
		if (light->visible_)
			dbDevice::getDevice()->LightEnable(light->d3d_index_, true);
	}

}



dbLightManager* dbLightManager::GetInstance()
{
	if (!instance_)
		instance_ = new dbLightManager();
	return instance_;
}

void dbLightManager::Exit()
{
	DB_MEM_DELETE(instance_);
}


bool dbLightManager::AddLight(dbLight* newlight)
{
	newlight->d3d_index_ = lights_map_.size();
	lights_map_[newlight->GetId()] = newlight;

	if (FAILED( dbDevice::getDevice()->SetLight(newlight->d3d_index_, &newlight->d3d_light_) ))
		return false;
	
	return true;
}

dbLight* dbLightManager::GetLight(const string id)
{
	std::unordered_map<string, dbLight*>::iterator it = lights_map_.find(id);
	if (it == lights_map_.end())
		return NULL;
	return it->second;

	/*
	for (unsigned int i = 0; i < lights_arr.size(); i++)
	{
		if (lights_arr[i]->getId() == id)
			return lights_arr[i];
	}
	return NULL;
	*/
}

bool dbLightManager::SetVisible(dbLight* l1, bool state)
{
	if (!l1)
		return false;

	if (state)
	{
		if (l1->IsVisible())	// already visible
			return true;

		if (active_lights_list_.size() < (unsigned)max_light_count_)
		{
			if (FAILED(dbDevice::getDevice()->LightEnable(l1->GetIndex(), true)))
				return false;

			active_lights_list_.push_back(l1->GetIndex());
		}
		else
		{
			DBX_WARN("Too many lights enabled! Deactivating oldest light..");

			int i_disable = active_lights_list_.front();	// deactivate oldest light
			if (FAILED(dbDevice::getDevice()->LightEnable(i_disable, false)))
				return false;

			active_lights_list_.pop_front();					// delete first
			active_lights_list_.push_back(l1->GetIndex());	// add new value at end
		}
	}
	else
	{	// deactivate light
		if (!l1->IsVisible())
			return true;

		int l_index = l1->GetIndex();
		if (FAILED(dbDevice::getDevice()->LightEnable(l_index, false)))
			return false;

		for (std::list<int>::const_iterator iterator = active_lights_list_.begin(); iterator != active_lights_list_.end(); ++iterator) 
		{
			if (l_index == *iterator)	// found
			{
				active_lights_list_.erase(iterator);
				break;		// iterator is invalid, so break
			}
		}
	}

	l1->visible_ = state;
	return true;
}

// SETUP LIGHT

// Spot light
dbSpotLight* dbLightManager::InitSpot(const std::string id, core::dbVector3 pos, core::dbVector3 direction, float range, core::dbColor Diffuse, float angleInCircle, float angleOutCircle)
{
	return InitSpot(id, pos, direction, range, Diffuse, Diffuse, dbColor(0.f), angleInCircle, angleOutCircle);
}

dbSpotLight* dbLightManager::InitSpot(const string id, dbVector3 pos, dbVector3 direction, float range, dbColor Diffuse, dbColor Specular, dbColor Ambient, float angleInCircle, float angleOutCircle)
{
	dbSpotLight* tmp = new dbSpotLight(id);
	tmp->InitLight();

	D3DLIGHT9* light = &tmp->d3d_light_;

	light->Type = D3DLIGHT_SPOT;
	light->Position = pos;
	light->Direction = direction;
	light->Range = range;

	light->Diffuse = Diffuse;
	light->Specular = Specular;
	light->Ambient = Ambient;
	
	light->Attenuation0 = 0.f;
	light->Attenuation1 = 0.05f;
	light->Attenuation2 = 0.f;
	
	light->Theta = angleInCircle;
	light->Phi = angleOutCircle;
	light->Falloff = 1.f;

	if (!AddLight(tmp))
		return NULL;
	return tmp;
}

// Point light
dbPointLight* dbLightManager::InitPoint(const std::string id, core::dbVector3 pos, float range, core::dbColor Diffuse)
{
	return InitPoint(id, pos, range, Diffuse, Diffuse, dbColor(0.f));
}

dbPointLight* dbLightManager::InitPoint(const string id, dbVector3 pos, float range, dbColor Diffuse, dbColor Specular, dbColor Ambient)
{	
	dbPointLight* tmp = new dbPointLight(id);
	tmp->InitLight();

	D3DLIGHT9* light = &tmp->d3d_light_;

	light->Type = D3DLIGHT_POINT;
	light->Position = pos;
	light->Range = range;
	light->Diffuse = Diffuse;
	light->Specular = Specular;
	light->Ambient = Ambient;

	light->Attenuation0 = 0.f;
	light->Attenuation1 = 0.05f;
	light->Attenuation2 = 0.f;

	if (!AddLight(tmp))
		return NULL;
	return tmp;
}

// Directional light
dbDirectionalLight* dbLightManager::InitDirectional(const std::string id, core::dbVector3 direction, core::dbColor Diffuse)
{
	return InitDirectional(id, direction, Diffuse, Diffuse, dbColor(0.f));
}

dbDirectionalLight* dbLightManager::InitDirectional(const string id, dbVector3 direction, dbColor Diffuse, dbColor Specular, dbColor Ambient)
{
	dbDirectionalLight* tmp = new dbDirectionalLight(id);
	tmp->InitLight();

	D3DLIGHT9* light = &tmp->d3d_light_;

	light->Type = D3DLIGHT_DIRECTIONAL;
	light->Direction = direction;
	light->Diffuse = Diffuse;
	light->Specular = Specular;
	light->Ambient = Ambient;

	if (!AddLight(tmp))
		return NULL;
	return tmp;
}

}}	// namespace