#include "DbofX.h"


dbCamera::dbCamera(void)
{
}

dbCamera::dbCamera(LPDIRECT3DDEVICE9 dev)
{
	d3ddevice = dev;
	position = dbVector3(0.f, 0.f, -10.f);
	LookAt = dbVector3(0.f, 0.f, 0.f);
	D3DXMatrixIdentity(&matRotation);
	D3DXMatrixIdentity(&matTranslation);
	rotation = 0.f;
}

dbCamera::~dbCamera(void)
{
}


dbCamera dbCamera::Setup(dbVector3 pos, dbVector3 lookAt)
{
	camPos = pos;
	dbCamera::LookAt = lookAt;
	return *this;
}


void dbCamera::Setup(dbCamera& newCamera)
{
	position = newCamera.position;
	LookAt = newCamera.LookAt;
}



void dbCamera::Move(dbVector3 pos, bool relative)
{
	if (relative)
		pos += position;

	position = pos;
}


void dbCamera::point(dbVector3 lookAt, bool relative)
{
	if (relative)
		lookAt += dbCamera::LookAt;

	dbCamera::LookAt = lookAt;
}


void dbCamera::rotate(float angle, bool relative)
{
	if (relative)
		rotation += angle;
	else
		rotation = angle;
	D3DXMatrixRotationAxis(&matRotation, &D3DXVECTOR3(0.f, 1.f, 0.f), rotation);
}

void dbCamera::rotateAroundPoint(float angle, dbVector3 point)
{
	D3DXMATRIX matResult, matRot, matTrans1, matTrans2;
	D3DXMatrixTranslation(&matTrans1, -point.x, -point.y, -point.z);
	D3DXMatrixRotationY(&matRot, angle);
	D3DXMatrixTranslation(&matTrans2, point.x, point.y, point.z);

	matTranslation = matTrans2 * matRot * matTrans2;
		//TODO
}

void dbCamera::apply()
{
	D3DXMatrixLookAtLH(&mat_camera_, 
		&D3DXVECTOR3(D3DVECTOR(position)), 
		&D3DXVECTOR3(D3DVECTOR(LookAt)), 
		&D3DXVECTOR3(0.f, 1.f, 0.f));
	mat_camera_ *= matRotation * matTranslation;
	d3ddevice->SetTransform(D3DTS_VIEW, &mat_camera_);
}

dbVector3 dbCamera::getPosition()
{
	return position;
}


dbVector3 dbCamera::getLookAt()
{
	return LookAt;
}