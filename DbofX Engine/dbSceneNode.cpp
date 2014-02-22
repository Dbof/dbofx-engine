#include "DbofX.h"

using std::string;

namespace dbx
{
namespace scene
{
	using namespace core;
// static variables definition

float					dbSceneNode::elapsed_time_;

//**********************************************************************

// constructor
/*
dbSceneNode::dbSceneNode()
{
	init();

#ifdef _DEBUG
	if (identifier != "")
		DBX_INFO("Node (ID=\"" + identifier + "\") added");
#endif
}*/

dbSceneNode::dbSceneNode(string id)
{
	Init();
	identifier_ = id;

#ifdef _DEBUG
	DBX_INFO("Node (ID=\"" + identifier_ + "\") added");
#endif
}

// destructor
dbSceneNode::~dbSceneNode()
{
}

void dbSceneNode::Init()		// used by constructors to initialize data
{
	ZeroMemory(this, sizeof(dbSceneNode));	// this deletes the vector "childNodes"

	// few default values for the object
	node_pos_						= dbVector3(0.f, 0.f, 0.f);
	node_right_						= dbVector3(1.f, 0.f, 0.f);
	node_up_						= dbVector3(0.f, 1.f, 0.f);
	node_direction_					= dbVector3(0.f, 0.f, 1.f);
	node_movespeed_					= 1.f;
	node_rotationspeed_				= 1.f;
	elapsed_time_					= 1.f;
	node_rotationx_ = node_rotationy_ = node_rotationz_ = 0.f;
	mat_rotationx_ = mat_rotationy_		= mat_rotationz_ = dbMatrixIdentity();
}

//**********************************************************************

// connection methods
dbSceneNode* dbSceneNode::AttachChild(dbSceneNode* snode)
{
	if (snode != NULL)
		child_nodes_vec_.push_back(snode);
	return snode;
}

//**********************************************************************

dbSceneNode* dbSceneNode::DetachChild(dbSceneNode* snode)
{
	if (snode != NULL)
	{
		for (unsigned int i = 0; i < child_nodes_vec_.size(); i++)
		{
			if (snode == child_nodes_vec_.at(i))	// if pointer equal
				child_nodes_vec_.erase(child_nodes_vec_.begin() + i);
		}
	}
	return snode;
}

//**********************************************************************

dbSceneNode* dbSceneNode::DetachChild(string identifier)
{
	dbSceneNode* snode = NULL;
	for (unsigned int i = 0; i < child_nodes_vec_.size(); i++)
	{
		if ( child_nodes_vec_.at(i)->GetId() == identifier)	// if identifier equal
			child_nodes_vec_.erase(child_nodes_vec_.begin() + i);
	}
	return snode;
}

//**********************************************************************

// movement methods

void dbSceneNode::Move(const dbVector3& direction, const float& units)
{	
	int i = -1;
	dbSceneNode* active = this;
	
	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);
		
		active->node_pos_ += dbVector3Norm(direction) * units * node_movespeed_ * elapsed_time_;

		i++;
	} while (i < (signed) child_nodes_vec_.size());
}

//**********************************************************************

void dbSceneNode::MoveUp(const float& units)
{
	int i = -1;
	dbSceneNode* active = this;
	
	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);
		
		active->node_pos_ += node_up_ * units * node_movespeed_ * elapsed_time_;

		i++;
	} while (i < (signed) child_nodes_vec_.size());
}

//**********************************************************************

void dbSceneNode::MoveRight(const float& units)
{
	int i = -1;
	dbSceneNode* active = this;
	
	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);
		
		active->node_pos_ += node_right_ * units * node_movespeed_ * elapsed_time_;

		i++;
	} while (i < (signed) child_nodes_vec_.size());
}

//**********************************************************************

void dbSceneNode::MoveForward(const float& units)
{
	int i = -1;
	dbSceneNode* active = this;
	
	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);
		
		active->node_pos_ += node_direction_ * units * node_movespeed_ * elapsed_time_;

		i++;
	} while (i < (signed) child_nodes_vec_.size());
}

//**********************************************************************

void dbSceneNode::RotateX(const float& angle, bool relative)
{
	int i = -1;
	dbSceneNode* active = this;
	
	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);

		if (relative)
			active->node_rotationx_ += angle * node_rotationspeed_ * elapsed_time_;
		else
			active->node_rotationx_ = angle * node_rotationspeed_ * elapsed_time_;
	
		active->mat_rotationx_ = dbMatrixRotationAxis(active->node_right_, active->node_rotationx_);

		D3DXVec3TransformCoord((D3DXVECTOR3*)&active->node_direction_, (D3DXVECTOR3*)&active->node_direction_, (D3DXMATRIX*)&(active->mat_rotationx_));
		D3DXVec3TransformCoord((D3DXVECTOR3*)&active->node_up_, (D3DXVECTOR3*)&active->node_up_, (D3DXMATRIX*)&(active->mat_rotationx_));

		i++;
	} while (i < (signed) child_nodes_vec_.size());
}

//**********************************************************************

void dbSceneNode::RotateY(const float& angle, bool relative)
{
	int i = -1;
	dbSceneNode* active = this;
	
	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);

		if (relative)
			active->node_rotationy_ += angle * node_rotationspeed_ * elapsed_time_;
		else
			active->node_rotationy_ = angle * node_rotationspeed_ * elapsed_time_;
	
		active->mat_rotationy_ = dbMatrixRotationAxis(active->node_up_, active->node_rotationy_);

		D3DXVec3TransformCoord((D3DXVECTOR3*)&active->node_direction_, (D3DXVECTOR3*)&active->node_direction_, (D3DXMATRIX*)&(active->mat_rotationy_));
		D3DXVec3TransformCoord((D3DXVECTOR3*)&active->node_right_, (D3DXVECTOR3*)&active->node_right_, (D3DXMATRIX*)&(active->mat_rotationy_));
	
		i++;
	} while (i < (signed) child_nodes_vec_.size());
}

//**********************************************************************

void dbSceneNode::RotateZ(const float& angle, bool relative)
{
	int i = -1;
	dbSceneNode* active = this;
	
	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);


		if (relative)
			active->node_rotationz_ += angle * node_rotationspeed_ * elapsed_time_;
		else
			active->node_rotationz_ = angle * node_rotationspeed_ * elapsed_time_;
	
		active->mat_rotationz_ = dbMatrixRotationAxis(node_direction_, node_rotationz_);

		D3DXVec3TransformCoord((D3DXVECTOR3*)&active->node_up_, (D3DXVECTOR3*)&active->node_up_, (D3DXMATRIX*)&(active->mat_rotationz_));
		D3DXVec3TransformCoord((D3DXVECTOR3*)&active->node_right_, (D3DXVECTOR3*)&active->node_right_, (D3DXMATRIX*)&(active->mat_rotationz_));
		
		i++;
	} while (i < (signed) child_nodes_vec_.size());
}

//**********************************************************************

void dbSceneNode::RotateAroundX(const dbVector3& position, float angle)
{
	angle *= node_rotationspeed_ * elapsed_time_;

	dbMatrix mat = dbMatrixIdentity();
	mat *= dbMatrixTranslation(-1 * position);
	mat *= dbMatrixRotationX(angle);
	mat *= dbMatrixTranslation(position);

	int i = -1;
	dbSceneNode* active = this;
	
	do
	{
		if (i < 0)
			active = this;
		else
			active = child_nodes_vec_.at(i);
		
		D3DXVec3TransformCoord((D3DXVECTOR3*)&active->node_pos_, (D3DXVECTOR3*)&active->node_pos_, (D3DXMATRIX*)&(mat));
	
		i++;
	} while (i < (signed) child_nodes_vec_.size());
}
	
//**********************************************************************

void dbSceneNode::RotateAroundY(const dbVector3& position, float  angle)
{
	angle *= node_rotationspeed_ * elapsed_time_;

	dbMatrix mat = dbMatrixIdentity();
	mat *= dbMatrixTranslation(-1 * position);
	mat *= dbMatrixRotationY(angle);
	mat *= dbMatrixTranslation(position);
	
	
	int i = -1;
	dbSceneNode* active = this;
	
	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);

		D3DXVec3TransformCoord((D3DXVECTOR3*)&active->node_pos_, (D3DXVECTOR3*)&node_pos_, (D3DXMATRIX*)&(mat));
	
		i++;
	} while (i < (signed) child_nodes_vec_.size());
}
	
//**********************************************************************
	
void dbSceneNode::RotateAroundZ(const dbVector3& position, float  angle)
{
	angle *= node_rotationspeed_ * elapsed_time_;

	dbMatrix mat = dbMatrixIdentity();
	mat *= dbMatrixTranslation(-1 * position);
	mat *= dbMatrixRotationZ(angle);
	mat *= dbMatrixTranslation(position);
	
	int i = -1;
	dbSceneNode* active = this;
	
	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);
		
		D3DXVec3TransformCoord((D3DXVECTOR3*)&active->node_pos_, (D3DXVECTOR3*)&node_pos_, (D3DXMATRIX*)&(mat));
	
		i++;
	} while (i < (signed) child_nodes_vec_.size());
}
	
//**********************************************************************

void dbSceneNode::RotateAroundXYZ(const dbVector3& position, const dbVector3& axis, float  angle)
{
	angle *= node_rotationspeed_ * elapsed_time_;

	dbMatrix mat = dbMatrixIdentity();
	mat *= dbMatrixTranslation(-1 * position);
	mat *= dbMatrixRotationAxis(axis, angle);
	mat *= dbMatrixTranslation(position);
	
	int i = -1;
	dbSceneNode* active = this;
	
	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);
		
		D3DXVec3TransformCoord((D3DXVECTOR3*)&active->node_pos_, (D3DXVECTOR3*)&active->node_pos_, (D3DXMATRIX*)&(mat));
	
		i++;
	} while (i < (signed) child_nodes_vec_.size());
}
	
//**********************************************************************

void dbSceneNode::LookAt(const dbVector3& target)
{
	// from: http://msdn.microsoft.com/en-us/library/windows/desktop/bb281710%28v=vs.85%29.aspx
	
	int i = -1;
	dbSceneNode* active = this;
	
	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);

	active->node_up_ = dbVector3(0.f, 1.f, 0.f);
	
	active->node_direction_ = dbVector3Norm(target - active->node_pos_);
	active->node_right_ = dbVector3Norm(dbVector3Cross(active->node_up_, active->node_direction_));
	active->node_up_ = dbVector3Norm( dbVector3Cross(active->node_direction_, active->node_right_) );

		i++;
	} while (i < (signed) child_nodes_vec_.size());
}

//**********************************************************************

void dbSceneNode::SetPos(const dbVector3& newPos)
{
	int i = -1;
	dbSceneNode* active = this;

	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);

		active->node_pos_ = newPos;
		
		i++;
	} while (i < (signed) child_nodes_vec_.size());
}

//**********************************************************************

void dbSceneNode::SetDirection(const dbVector3& newDirection)
{
	int i = -1;
	dbSceneNode* active = this;

	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);

		active->node_up_ = dbVector3(0.f, 1.f, 0.f);
		active->node_direction_ = dbVector3Norm(newDirection);
		active->node_right_ = dbVector3Norm(dbVector3Cross(active->node_up_, active->node_direction_));
		
		i++;
	} while (i < (signed) child_nodes_vec_.size());
}

//**********************************************************************

void dbSceneNode::SetUp(const dbVector3& newUp)	
{
	int i = -1;
	dbSceneNode* active = this;

	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);

		active->node_up_ = dbVector3Norm(newUp);

		i++;
	} while (i < (signed) child_nodes_vec_.size());
}

//**********************************************************************

void dbSceneNode::SetRight(const dbVector3& newRight)
{
	int i = -1;
	dbSceneNode* active = this;

	do
	{
		if (i >= 0)
			active = child_nodes_vec_.at(i);

		active->node_right_ = dbVector3Norm(newRight);

		i++;
	} while (i < (signed) child_nodes_vec_.size());
}

//**********************************************************************

}}	// namespace