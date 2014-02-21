#include "DbofX.h"

namespace dbx 
{
	dbSkinnedMesh::dbSkinnedMesh(void)
	{
		m_pRootBone = NULL;
		m_pAnimControl = NULL;
	}


	dbSkinnedMesh::~dbSkinnedMesh(void)
	{
		Cleanup(m_pRootBone);
		DB_MEM_RELEASE(m_pAnimControl);
	}

	void dbSkinnedMesh::Cleanup(D3DXFRAME* b)
	{
		if (b == NULL)
			return;

		
		if (b->pFrameFirstChild != NULL)
			Cleanup(b->pFrameFirstChild);
		if (b->pFrameSibling != NULL)
			Cleanup(b->pFrameSibling);
		
		BoneHierarchyLoader bh;

		if (b->pMeshContainer)
		{
			bh.DestroyMeshContainer(b->pMeshContainer);
		}

		bh.DestroyFrame(b);
	}

	
	void dbSkinnedMesh::Load(char fileName[])
	{
		BoneHierarchyLoader boneHierarchy;
		//Load a bone hierarchy from a file
		D3DXLoadMeshHierarchyFromX(fileName,
			D3DXMESH_MANAGED,
			dbDevice::getDevice(),
			&boneHierarchy,
			NULL,
			&m_pRootBone,
			&m_pAnimControl);
		
		
		//Update all Bone transformation matrices
		D3DXMATRIX i;
		D3DXMatrixIdentity(&i);
		UpdateMatrices((Bone*)m_pRootBone, &i);
		SetupBoneMatrixPointers((Bone*)m_pRootBone);
	}

	void dbSkinnedMesh::SetupBoneMatrixPointers(Bone *bone)
	{
		if (bone == NULL)
			bone = (Bone*)m_pRootBone;

		// Find all bones containing a mesh
		if (bone->pMeshContainer != NULL)
		{
			BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

			// For the bones with skinned meshes, set up the pointers
			if (boneMesh->pSkinInfo != NULL)
			{
				// Get num bones influencing this mesh
				int NumBones = boneMesh->pSkinInfo->GetNumBones();

				// Create an array of pointers with numBones pointers
				boneMesh->boneMatrixPtrs = new D3DXMATRIX*[NumBones];

				// fill array
				for (int i = 0; i < NumBones; i++)
				{
					// Find influencing bone by name
					Bone* b = FindBone(boneMesh->pSkinInfo->GetBoneName(i));

					// ... and store pointer to it in the array
					if (b != NULL)
					{
						boneMesh->boneMatrixPtrs[i] = &b->CombinedTransformationMatrix;
					}
					else
					{
						//D3DXMatrixIdentity(boneMesh->boneMatrixPtrs[i]);
						boneMesh->boneMatrixPtrs[i] = NULL;
					}
				}
			}
		}

		// Traverse the rest of the hierarchy...
		if (bone->pFrameSibling != NULL)
			SetupBoneMatrixPointers((Bone*)bone->pFrameSibling);

		if (bone->pFrameFirstChild != NULL)
			SetupBoneMatrixPointers((Bone*)bone->pFrameFirstChild);
	}

	
	Bone* dbSkinnedMesh::FindBone(LPCSTR name)
	{
		return (Bone*)D3DXFrameFind(m_pRootBone, name);
	}
	
	void dbSkinnedMesh::UpdateMatrices(Bone* bone, D3DXMATRIX *parentMatrix)
	{
		CalculateWorldMatrix(bone, parentMatrix);	// this is recursive
	}

	
	void dbSkinnedMesh::SetTransform(Bone* bone, core::dbMatrix &transformMat, boolean recursive)
	{
		if (bone == NULL)
			bone = GetRoot();

		bone->CombinedTransformationMatrix *= *(D3DXMATRIX*)&transformMat;

		if (recursive)
		{
			if (bone->pFrameFirstChild != NULL)
				SetTransform((Bone*)bone->pFrameFirstChild, transformMat, true);
			if (bone->pFrameSibling != NULL)
				SetTransform((Bone*)bone->pFrameSibling, transformMat, true);
		}
	}

	void dbSkinnedMesh::Move(float delta)
	{
		if (m_pAnimControl != NULL)
		{
			if (FAILED(m_pAnimControl->AdvanceTime(delta, NULL)))
				DBX_WARN("AdvanceTime() failed!");
		}
	}

	void dbSkinnedMesh::Render(Bone* bone)
	{
		if (bone == NULL)
			bone = (Bone*)m_pRootBone;

		// if there is a mesh to render...
		if (bone->pMeshContainer != NULL)
		{
			BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

			if (boneMesh->pSkinInfo != NULL)
			{
				// set up bone transforms
				int numBones = boneMesh->pSkinInfo->GetNumBones();
				for (int i = 0; i < numBones; i++)
					D3DXMatrixMultiply(&boneMesh->currentBoneMatrices[i],
						&boneMesh->boneOffsetMatrices[i],
						boneMesh->boneMatrixPtrs[i]);

				// Update the skinned mesh
				BYTE* src = NULL, *dest = NULL;
				boneMesh->OriginalMesh->LockVertexBuffer(
					D3DLOCK_READONLY, (VOID**)&src);
				boneMesh->MeshData.pMesh->LockVertexBuffer(
					0, (VOID**)&dest);
				boneMesh->pSkinInfo->UpdateSkinnedMesh(
					boneMesh->currentBoneMatrices,
					NULL, src, dest);
				boneMesh->MeshData.pMesh->UnlockVertexBuffer();
				boneMesh->OriginalMesh->UnlockVertexBuffer();

				// Render the mesh
				for (unsigned int i = 0; i < boneMesh->NumAttributeGroups; i++)
				{
					int mtrl = boneMesh->attributeTable[i].AttribId;
					dbDevice::getDevice()->SetMaterial(&boneMesh->materials[mtrl]);
					dbDevice::getDevice()->SetTexture(0, boneMesh->textures[mtrl]);
					boneMesh->MeshData.pMesh->DrawSubset(mtrl);
				}
			}
			else
			{	// now can render static meshes, too..
				for (unsigned int i = 0; i < boneMesh->materials.size(); i++)
				{
					dbDevice::getDevice()->SetTransform(D3DTS_WORLD, &bone->CombinedTransformationMatrix);
					boneMesh->OriginalMesh->DrawSubset(i);
				}
			}

		}

		// Render Siblings & Children
		if (bone->pFrameSibling != NULL)
			Render((Bone*)bone->pFrameSibling);
		if (bone->pFrameFirstChild != NULL)
			Render((Bone*)bone->pFrameFirstChild);
	}
}