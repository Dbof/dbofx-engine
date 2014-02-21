#include "DbofX.h"


using std::string;

namespace dbx
{
namespace scene
{
	using namespace core;

dbSpotLight::dbSpotLight(const string identifier) : dbLight(identifier)
{
	is_init_ = false;
	dbSceneNode::identifier_ = identifier;
}

dbSpotLight::~dbSpotLight(void)
{
}


bool dbSpotLight::InitLight()
{
	if (is_init_)	//already initiated
		return true;
	
	d3d_index_ = 0;
	ZeroMemory(&d3d_light_, sizeof(d3d_light_));

	d3d_light_.Type = D3DLIGHT_SPOT;
	d3d_light_.Attenuation0 = 0.f;
	d3d_light_.Attenuation1 = 0.125f;
	d3d_light_.Attenuation2 = 0.f;

	return is_init_ = true;
}

void dbSpotLight::SetInnerAngle(const float &inAngle)
{
	d3d_light_.Theta = inAngle;
	Update();
}

void dbSpotLight::SetOuterAngle(const float &outAngle)
{
	d3d_light_.Phi = outAngle;
	Update();
}

void dbSpotLight::SetRange(const float &newRange)
{
	d3d_light_.Range = newRange;
	Update();
}

void dbSpotLight::SetAttenuation( const float& att0, const float& att1, const float& att2 )
{
	d3d_light_.Attenuation0 = att0;
	d3d_light_.Attenuation1 = att1;
	d3d_light_.Attenuation2 = att2;
	Update();
}

}}	// namespace