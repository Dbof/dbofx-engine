#ifndef SKINNED_MESH_H
#define SKINNED_MESH_H

namespace dbx 
{
	class DBX_API dbSkinnedMesh
	{
	public:
		dbSkinnedMesh();
		~dbSkinnedMesh();
		void Load(const char fileName[]);
		void SetupBoneMatrixPointers(Bone* bone);
		void Render(Bone* bone);

		Bone* FindBone(LPCSTR name);
		Bone* GetRoot() { return (Bone*)m_pRootBone; }
		void UpdateMatrices(Bone* bone, D3DXMATRIX *parentMatrix);
		void SetTransform(Bone* bone, core::dbMatrix &transformMat, boolean recursive);
		void Move(float delta);

	private:
		D3DXFRAME* m_pRootBone;
		ID3DXAnimationController* m_pAnimControl;
		void Cleanup(D3DXFRAME* b);

		int actualPosition;
	};
}

#endif