#include "DbofX.h"

dbSceneNode::dbSceneNode(void)
{
}


dbSceneNode::~dbSceneNode(void)
{
}


bool dbSceneNode::attachMesh(dbMesh* newMesh)
{
	if (newMesh->IsInitialized())
	{
		meshes.push_back(newMesh);
		return true;
	}
	return false;
}

dbMesh*	dbSceneNode::getAttachedMesh(unsigned short index)
{
	if (index >= meshes.size())
	{
		DBX_ERROR("Could not find any mesh at the indicated index.");
		return NULL;
	}
	return meshes[index];
}

void dbSceneNode::rotateX(const float &angle)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i]->RotateX(angle);
}
void dbSceneNode::rotateY(const float &angle)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i]->RotateY(angle);
}
void dbSceneNode::rotateZ(const float &angle)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i]->RotateZ(angle);
}
void dbSceneNode::scale(const dbVector3 &factor)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i]->Scale(factor);
}