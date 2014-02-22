#include "DbofX.h"

namespace dbx 
{
namespace scene
{
	using namespace core;

	dbSkinnedMesh::dbSkinnedMesh(const std::string& id) : dbSceneNode(id)
	{
		pRootBone_ = NULL;
		pAnimControl_ = NULL;
		mesh_scale_ = dbVector3(1.f, 1.f, 1.f);
	}

	dbSkinnedMesh::~dbSkinnedMesh(void)
	{
		Cleanup(pRootBone_);
		DB_MEM_RELEASE(pAnimControl_);
	}

	void dbSkinnedMesh::Cleanup(Bone* b)
	{
		if (b == NULL)
			return;

		if (b->pFrameFirstChild != NULL)
			Cleanup((Bone*)b->pFrameFirstChild);
		if (b->pFrameSibling != NULL)
			Cleanup((Bone*)b->pFrameSibling);
		
		BoneHierarchyLoader bh;

		if (b->pMeshContainer)
		{
			bh.DestroyMeshContainer(b->pMeshContainer);
		}

		bh.DestroyFrame(b);
	}

	
	void dbSkinnedMesh::Init(const std::string filePath)
	{
		BoneHierarchyLoader boneHierarchy;
		//Load a bone hierarchy from a file
		D3DXLoadMeshHierarchyFromX(filePath.c_str(),
			D3DXMESH_MANAGED,
			dbDevice::getDevice(),
			&boneHierarchy,
			NULL,
			(LPD3DXFRAME*)&pRootBone_,
			&pAnimControl_);
		
		
		//Update all Bone transformation matrices
		D3DXMATRIX i;
		D3DXMatrixIdentity(&i);
		UpdateMatrices((Bone*)pRootBone_, dbMatrixIdentity());
		SetupBoneMatrixPointers((Bone*)pRootBone_);
	}

	void dbSkinnedMesh::SetupBoneMatrixPointers(Bone *bone)
	{
		if (bone == NULL)
			bone = (Bone*)pRootBone_;

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
				boneMesh->boneMatrixPtrs = new dbMatrix*[NumBones];

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

	
	Bone* dbSkinnedMesh::FindBone(std::string name)
	{
		return (Bone*)D3DXFrameFind(pRootBone_, name.c_str());
	}
	
	void dbSkinnedMesh::UpdateMatrices(Bone* bone, const dbMatrix& parentMatrix)
	{
		CalculateWorldMatrix(bone, parentMatrix);	// this is recursive
	}

	
	void dbSkinnedMesh::SetBoneTransform(Bone* bone, const dbMatrix& transformMat, boolean recursive)
	{
		if (bone == NULL)
			bone = GetRoot();

		bone->CombinedTransformationMatrix *= *(D3DXMATRIX*)&transformMat;

		if (recursive)
		{
			if (bone->pFrameFirstChild != NULL)
				SetBoneTransform((Bone*)bone->pFrameFirstChild, transformMat, true);
			if (bone->pFrameSibling != NULL)
				SetBoneTransform((Bone*)bone->pFrameSibling, transformMat, true);
		}
	}


	dbMatrix* dbSkinnedMesh::GetBoneTransform(Bone* bone)
	{
		if (bone != NULL)
			return &bone->CombinedTransformationMatrix;
		return NULL;
	}

	
	dbMatrix dbSkinnedMesh::GetMatrix()
	{		
		viewMatrix_._11 = node_right_.x; viewMatrix_._12 = node_up_.x; viewMatrix_._13 = node_direction_.x;
		viewMatrix_._21 = node_right_.y; viewMatrix_._22 = node_up_.y; viewMatrix_._23 = node_direction_.y;
		viewMatrix_._31 = node_right_.z; viewMatrix_._32 = node_up_.z; viewMatrix_._33 = node_direction_.z;

		viewMatrix_._41 = - dbVector3Dot(node_pos_, node_right_ );
		viewMatrix_._42 = - dbVector3Dot(node_pos_, node_up_ );
		viewMatrix_._43 = - dbVector3Dot(node_pos_, node_direction_ );
	
		viewMatrix_ = dbMatrixScaling(mesh_scale_) * dbMatrixInvert(viewMatrix_);
		return viewMatrix_;	// SRT = scale, rotate, translate
	}

	void dbSkinnedMesh::Animate(float delta)
	{
		if (pAnimControl_ != NULL)
		{
			if (FAILED(pAnimControl_->AdvanceTime(delta, NULL)))
				DBX_WARN("AdvanceTime() failed!");
		}
	}

	
	//**********************************************************************

	void dbSkinnedMesh::Scale(const dbVector3 &factor)
	{
		mesh_scale_ = factor;
	}

	bool dbSkinnedMesh::Render(Bone* bone)
	{		
		if (!is_init_)
		{
			DBX_ERROR("Mesh is not initialized and could not be rendered");
			return false;
		}

		if (bone == NULL)
			bone = (Bone*)pRootBone_;

		// if there is a mesh to render...
		if (bone->pMeshContainer != NULL)
		{
			BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

			if (boneMesh->pSkinInfo != NULL)
			{
				// set up bone transforms
				int numBones = boneMesh->pSkinInfo->GetNumBones();
				for (int i = 0; i < numBones; i++)
					D3DXMatrixMultiply((D3DXMATRIX*)&boneMesh->currentBoneMatrices[i],
						(D3DXMATRIX*)&boneMesh->boneOffsetMatrices[i],
						(D3DXMATRIX*)boneMesh->boneMatrixPtrs[i]);

				// Update the skinned mesh
				BYTE* src = NULL, *dest = NULL;
				boneMesh->OriginalMesh->LockVertexBuffer(
					D3DLOCK_READONLY, (VOID**)&src);
				boneMesh->MeshData.pMesh->LockVertexBuffer(
					0, (VOID**)&dest);
				boneMesh->pSkinInfo->UpdateSkinnedMesh(
					(D3DXMATRIX*)boneMesh->currentBoneMatrices,
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
					dbDevice::setTransform(bone->CombinedTransformationMatrix);
					boneMesh->OriginalMesh->DrawSubset(i);
				}
			}

		}

		// Render Siblings & Children
		if (bone->pFrameSibling != NULL)
			Render((Bone*)bone->pFrameSibling);
		if (bone->pFrameFirstChild != NULL)
			Render((Bone*)bone->pFrameFirstChild);

		return true;
	}
}}  // namespace