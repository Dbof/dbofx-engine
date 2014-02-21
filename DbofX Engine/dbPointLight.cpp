#include "DbofX.h"

using std::string;

namespace dbx
{
namespace scene
{
	using namespace core;

dbPointLight::dbPointLight(const string identifier) : dbLight(identifier)
{
	is_init_ = false;
	dbSceneNode::identifier_ = identifier;
}

dbPointLight::~dbPointLight(void)
{
}

bool dbPointLight::InitLight()
{
	if (is_init_)	//already initiated
		return true;
	
	d3d_index_ = 0;
	ZeroMemory(&d3d_light_, sizeof(d3d_light_));

	d3d_light_.Type = D3DLIGHT_POINT;
	d3d_light_.Attenuation0 = 0.f;
	d3d_light_.Attenuation1 = 0.125f;
	d3d_light_.Attenuation2 = 0.f;

	return is_init_ = true;
}

void dbPointLight::SetRange(const float &newRange)
{
	d3d_light_.Range = newRange;
	Update();
}

void dbPointLight::SetAttenuation(const float& att0, const float& att1, const float& att2)
{
	d3d_light_.Attenuation0 = att0;
	d3d_light_.Attenuation1 = att1;
	d3d_light_.Attenuation2 = att2;
	Update();
}


}}	// namespace