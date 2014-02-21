#include "DbofX.h"

using std::string;

namespace dbx
{
namespace scene
{
	using namespace core;

dbLight::dbLight(const string id) : dbSceneNode(id)
{
	visible_ = is_init_ = false;
}

dbLight::~dbLight(void)
{
}

bool dbLight::InitLight()
{
	if (is_init_)	//already initiated
		return true;

	ZeroMemory(&d3d_light_, sizeof(d3d_light_));
	return is_init_ = true;
}

void dbLight::SetPos(dbVector3 newPos)
{
	dbSceneNode::SetPos(newPos);
	d3d_light_.Position = node_pos_;
	Update();
}

void dbLight::SetDirection(core::dbVector3 newDir)
{
	dbSceneNode::SetDirection(newDir);
	d3d_light_.Direction = node_direction_;
	Update();
}

void dbLight::Move(dbVector3 direction, float units)
{
	dbSceneNode::Move(direction, units);
	d3d_light_.Position = node_pos_;
	Update();
}

void dbLight::MoveUp(float units)
{
	dbSceneNode::MoveUp(units);
	d3d_light_.Position = node_pos_;
	Update();
}
void dbLight::MoveRight(float units)
{
	dbSceneNode::MoveUp(units);
	d3d_light_.Position = node_pos_;
	Update();
}
void dbLight::MoveForward(float units)
{
	dbSceneNode::MoveForward(units);
	d3d_light_.Position = node_pos_;
	Update();
}

void dbLight::LookAt(dbVector3 target)
{
	dbSceneNode::LookAt(target);
	d3d_light_.Direction = node_direction_;
	Update();
}


void dbLight::RotateX(float angle, bool relative)
{
	dbSceneNode::RotateX(angle, relative);
	d3d_light_.Direction = node_direction_;
	Update();
}

void dbLight::RotateY(float angle, bool relative)
{
	dbSceneNode::RotateY(angle, relative);
	d3d_light_.Direction = node_direction_;
	Update();
}

void dbLight::RotateZ(float angle, bool relative)
{
	dbSceneNode::RotateZ(angle, relative);
	d3d_light_.Direction = node_direction_;
	Update();
}

void dbLight::RotateAroundX(dbVector3 position, float angle)
{
	dbSceneNode::RotateAroundX(position, angle);
	d3d_light_.Position = node_pos_;
	Update();
}

void dbLight::RotateAroundY(dbVector3 position, float angle)
{
	dbSceneNode::RotateAroundY(position, angle);
	d3d_light_.Position = node_pos_;
	Update();
}

void dbLight::RotateAroundZ(dbVector3 position, float angle)
{
	dbSceneNode::RotateAroundZ(position, angle);
	d3d_light_.Position = node_pos_;
	Update();
}

void dbLight::RotateAroundXYZ(dbVector3 position, dbVector3 axis, float angle)
{
	dbSceneNode::RotateAroundXYZ(position, axis, angle);
	d3d_light_.Position = node_pos_;
	Update();
}

void dbLight::SetColor(dbColor Diffuse, dbColor Specular, dbColor Ambient)
{
	d3d_light_.Diffuse = Diffuse;
	d3d_light_.Specular = Specular;
	d3d_light_.Ambient = Ambient;
	Update();
}

void dbLight::SetDiffuseColor(dbColor newcolor)
{
	d3d_light_.Diffuse = newcolor;
	Update();
}

void dbLight::SetSpecularColor(dbColor newcolor)
{
	d3d_light_.Specular = newcolor;
	Update();
}
void dbLight::SetAmbientColor(dbColor newcolor)
{
	d3d_light_.Ambient = newcolor;
	Update();
}

bool dbLight::SetVisible(bool enable)
{
	if (this == NULL)		//Light does not exist
		return false;

	return dbLightManager::GetInstance()->SetVisible(this, enable);
}

void dbLight::Update()
{
	dbDevice::getDevice()->SetLight(d3d_index_, &d3d_light_);
}

bool dbLight::IsVisible()
{
	return visible_;
}

}}	// namespace