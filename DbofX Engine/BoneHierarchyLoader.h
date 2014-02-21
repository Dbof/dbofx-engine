#ifndef BONE_HIERARCHY_LOADER_H
#define BONE_HIERARCHY_LOADER_H

#include "d3dx9anim.h"

namespace dbx
{
	struct DBX_API Bone : public D3DXFRAME
	{
		D3DXMATRIX CombinedTransformationMatrix;
	};

	struct DBX_API BoneMesh : public D3DXMESHCONTAINER
	{
		ID3DXMesh* OriginalMesh;					// Reference mesh
		std::vector<D3DMATERIAL9> materials;		// List of materials
		std::vector<IDirect3DTexture9*> textures;	// List of textures
		
		DWORD NumAttributeGroups;					// Number attribute groups
		D3DXATTRIBUTERANGE* attributeTable;			// Attribute table
		D3DXMATRIX** boneMatrixPtrs;				// Pointers to bone matrices
		D3DXMATRIX* boneOffsetMatrices;				// Bone offset matrices
		D3DXMATRIX* currentBoneMatrices;			// Current bone matrices
	};

	class DBX_API BoneHierarchyLoader : public ID3DXAllocateHierarchy
	{
	public:
		STDMETHOD(CreateFrame) (LPCSTR Name, LPD3DXFRAME *ppNewFrame);   
		STDMETHOD(CreateMeshContainer)(
			THIS_ LPCSTR Name, 
			CONST D3DXMESHDATA *pMeshData, 
			CONST D3DXMATERIAL *pMaterials, 
			CONST D3DXEFFECTINSTANCE *pEffectInstances, 
			DWORD NumMaterials, 
			CONST DWORD *pAdjacency, 
			LPD3DXSKININFO pSkinInfo, 
			LPD3DXMESHCONTAINER *ppNewMeshContainer);
		STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
		STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree); 
	};

	DBX_API void CalculateWorldMatrix(Bone* bone, D3DXMATRIX* parentMat);
}

#endif