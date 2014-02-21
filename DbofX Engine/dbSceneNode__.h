#ifndef _SCENENODE_H
#define _SCENENODE_H

#include "dbMesh.h"
using std::vector;

class dbSceneNode
{
public:
	dbSceneNode(void);
	~dbSceneNode(void);

	bool		attachMesh(dbMesh* newMesh);
	dbMesh*		getAttachedMesh(unsigned short index);

	// move the SceneNode
	void rotateX(const float &angle);
	void rotateY(const float &angle);
	void rotateZ(const float &angle);
	void scale(const dbVector3 &factor);

private:
	string name;

	vector< dbMesh* > meshes;
};

#endif