#include "DbofX.h"

namespace dbx 
{
namespace scene
{
	HRESULT BoneHierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
	{
		Bone* b = new Bone;
		if (Name != NULL)
		{
			b->Name = new char[strlen(Name)+1];
			strcpy_s(b->Name, strlen(Name)+1, Name);
		}
		
		D3DXMatrixIdentity(&b->TransformationMatrix);
		D3DXMatrixIdentity((D3DXMATRIX*)&b->CombinedTransformationMatrix);

		*ppNewFrame = (LPD3DXFRAME)b;
		return S_OK;
	}


	HRESULT BoneHierarchyLoader::DestroyFrame(THIS_ LPD3DXFRAME pFrameToFree)
	{
		if(pFrameToFree)
		{
			//Free Name String
			DB_MEM_DELETE_ARRAY(pFrameToFree->Name);
		}

		//Free Frame
		DB_MEM_DELETE(pFrameToFree);
		return S_OK;
	}

	HRESULT BoneHierarchyLoader::CreateMeshContainer(
			THIS_ LPCSTR Name, 
			CONST D3DXMESHDATA *pMeshData, 
			CONST D3DXMATERIAL *pMaterials, 
			CONST D3DXEFFECTINSTANCE *pEffectInstances, 
			DWORD NumMaterials, 
			CONST DWORD *pAdjacency, 
			LPD3DXSKININFO pSkinInfo, 
			LPD3DXMESHCONTAINER *ppNewMeshContainer)
	{
		// Create new bone Mesh
		BoneMesh* boneMesh = new BoneMesh;
		ZeroMemory(boneMesh, sizeof(BoneMesh));

		// Get mesh data
		boneMesh->OriginalMesh = pMeshData->pMesh;
		boneMesh->MeshData.pMesh = pMeshData->pMesh;
		boneMesh->MeshData.Type = pMeshData->Type;

		// Add reference so the mesh is not deallocated
		pMeshData->pMesh->AddRef();

		// Copy materials and load textures (just like with a static mesh)
		for (unsigned int i = 0; i < NumMaterials; i++)
		{
			D3DXMATERIAL mtrl;
			memcpy(&mtrl, &pMaterials[i], sizeof(D3DXMATERIAL));
			boneMesh->materials.push_back(mtrl.MatD3D);
			IDirect3DTexture9* newTexture = NULL;

			if (mtrl.pTextureFilename != NULL)
			{
				// Load texture
				D3DXCreateTextureFromFile(dbDevice::getDevice(), 
					mtrl.pTextureFilename, &newTexture);
			}
			boneMesh->textures.push_back(newTexture);
		}

		if (pSkinInfo != NULL)
		{
			// Get Skin Info
			boneMesh->pSkinInfo = pSkinInfo;

			// Add reference so SkinInfo is not deallocated
			pSkinInfo->AddRef();

			// Clone mesh and store in boneMesh->MeshData.pMesh
			pMeshData->pMesh->CloneMeshFVF(D3DXMESH_MANAGED,
				pMeshData->pMesh->GetFVF(),
				dbDevice::getDevice(),
				&boneMesh->MeshData.pMesh);

			// Get attribute table
			boneMesh->MeshData.pMesh->GetAttributeTable(
				NULL, &boneMesh->NumAttributeGroups);

			boneMesh->attributeTable = 
				new D3DXATTRIBUTERANGE[boneMesh->NumAttributeGroups];

			boneMesh->MeshData.pMesh->GetAttributeTable(
				boneMesh->attributeTable, NULL);

			// Create bone offset and current matrices
			int NumBones = pSkinInfo->GetNumBones();
			boneMesh->boneOffsetMatrices = new core::dbMatrix[NumBones];
			boneMesh->currentBoneMatrices = new core::dbMatrix[NumBones];

			// get bone offset matrices
			for (int i = 0; i < NumBones; i++)
				boneMesh->boneOffsetMatrices[i] = *(boneMesh->pSkinInfo->GetBoneOffsetMatrix(i));

		}

		// set ooNewMeshContainer to the newly created boneMesh container
		*ppNewMeshContainer = boneMesh;

		return S_OK;
	}
	
	HRESULT BoneHierarchyLoader::DestroyMeshContainer(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree)
	{
		BoneMesh* bmesh = (BoneMesh*)pMeshContainerToFree;
		if (bmesh == NULL)
			return S_FALSE;

		for (unsigned int i = 0; i < bmesh->textures.size(); i++)
		{
			DB_MEM_RELEASE(bmesh->textures[i]);
		}
		
		if (bmesh->OriginalMesh != bmesh->MeshData.pMesh)
			DB_MEM_RELEASE(bmesh->OriginalMesh);
		DB_MEM_RELEASE(bmesh->MeshData.pMesh);
		DB_MEM_RELEASE(bmesh->pSkinInfo);
		DB_MEM_DELETE_ARRAY(bmesh->boneMatrixPtrs);
		DB_MEM_DELETE_ARRAY(bmesh->boneOffsetMatrices);
		DB_MEM_DELETE_ARRAY(bmesh->currentBoneMatrices);
		DB_MEM_DELETE_ARRAY(bmesh->attributeTable);

		DB_MEM_DELETE(bmesh);
		return S_OK;
	}


	void CalculateWorldMatrix(Bone* bone, const core::dbMatrix& parentMat)
	{
		if (bone == NULL)
			return;

		//Calculate the combined transformation matrix
		D3DXMatrixMultiply((D3DXMATRIX*)&bone->CombinedTransformationMatrix,
			&bone->TransformationMatrix,
			(D3DXMATRIX*)&parentMat);
		
		//Perform the same calculation on all siblings...
		if(bone->pFrameSibling)
			CalculateWorldMatrix((Bone*)bone->pFrameSibling, parentMat);

		//... and all children
		if(bone->pFrameFirstChild)
		{
			//Note that we send a different parent matrix to siblings and children!
			CalculateWorldMatrix((Bone*)bone->pFrameFirstChild, bone->CombinedTransformationMatrix);
		}
	}
}}  // namespace