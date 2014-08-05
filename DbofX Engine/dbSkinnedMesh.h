#ifndef SKINNED_MESH_H
#define SKINNED_MESH_H

namespace dbx 
{
namespace scene
{
	class DBX_API dbSkinnedMesh : public dbSceneNode
	{
	public:
		// private constructor
		dbSkinnedMesh(const std::string& identifier);
		~dbSkinnedMesh();

		void Init(const std::string filePath);
		bool IsInitialized()	{ return is_init_; };

		bool Render(Bone* bone);
		
		// change mesh
		void Scale(const core::dbVector3 &factor);

		Bone* FindBone(std::string name);

		void UpdateMatrices(Bone* bone, const core::dbMatrix& parentMatrix);
		void SetBoneTransform(Bone* bone, const core::dbMatrix& transformMat, boolean recursive);

		// alpha blending
		void EnableAlphaBlending(bool enabled) { use_alpha_ = enabled; }

		core::dbMatrix* GetBoneTransform(Bone* bone);
		core::dbMatrix GetMatrix();
		Bone* GetRoot() { return pRootBone_; }

		void Animate(float delta);

	private:
		friend class dbScene;

		void SetupBoneMatrixPointers(Bone* bone);
		void Cleanup(Bone* b);
		
		bool						is_init_;
		bool						use_materials_;	// TODO
		bool						use_textures_; // TODO
		bool						use_alpha_; // TODO
		core::dbVector3				mesh_scale_;
		core::dbMatrix				viewMatrix_;

		Bone*						pRootBone_;
		ID3DXAnimationController*	pAnimControl_;

		int actualPosition;
	};
}} // namespace

#endif