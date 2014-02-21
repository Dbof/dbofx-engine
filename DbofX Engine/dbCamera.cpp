#include "DbofX.h"

using std::string;

namespace dbx
{
namespace scene
{
	using namespace core;
// constructor/destructor
dbCamera::dbCamera(const string id) : dbSceneNode(id)
{
	/*
	node_pos					= dbVector3(0.f, 0.f, -5.f);
	node_right					= dbVector3(1.f, 0.f, 0.f);
	node_up						= dbVector3(0.f, 1.f, 0.f);
	node_direction				= dbVector3(0.f, 0.f, -1.f);
	node_movespeed				= 1.f;
	node_rotationspeed			= 1.f;
	node_rotationX = node_rotationY = node_rotationZ = 0.f;

	matRotationX = matRotationY = matRotationZ = dbMatrixIdentity();
	*/
}

dbCamera::~dbCamera()
{
}

//**********************************************************************

void dbCamera::Update(float fElapsedTime)
{
	elapsed_time_ = fElapsedTime;
	SetCameraView(fElapsedTime);
}

//**********************************************************************

void dbCamera::Setup(dbVector3 position, dbVector3 direction)
{
	node_pos_					= position;
	node_direction_				= dbVector3Norm(direction);

	node_right_					= dbVector3(node_direction_.z, 0, -node_direction_.x);
	node_up_						= dbVector3Cross(node_direction_, node_right_);
}

//**********************************************************************

void dbCamera::SetCameraView(float fElapsedTime)	
{
	dbMatrix viewMatrix = dbMatrixIdentity();

	viewMatrix._11 = node_right_.x; viewMatrix._12 = node_up_.x; viewMatrix._13 = node_direction_.x; viewMatrix._14 = 0;
	viewMatrix._21 = node_right_.y; viewMatrix._22 = node_up_.y; viewMatrix._23 = node_direction_.y; viewMatrix._24 = 0;
	viewMatrix._31 = node_right_.z; viewMatrix._32 = node_up_.z; viewMatrix._33 = node_direction_.z; viewMatrix._34 = 0;

	viewMatrix._41 = - dbVector3Dot(node_pos_, node_right_ );
	viewMatrix._42 = - dbVector3Dot(node_pos_, node_up_ );
	viewMatrix._43 = - dbVector3Dot(node_pos_, node_direction_ );
	viewMatrix._44 = 1;

	dbDevice::getDevice()->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&viewMatrix);
}

}}	// namespace