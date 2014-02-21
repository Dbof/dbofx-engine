#ifndef CAMERA_H
#define CAMERA_H

namespace dbx
{
namespace scene
{
	class DBX_API dbCamera : public dbSceneNode
	{
	public:
		// destructor
		~dbCamera();

		core::dbMatrix GetMatrix()	{ return view_matrix_; }

		void Update(float fElapsedTime);
		void Setup(core::dbVector3 position, core::dbVector3 direction);
	private:
		friend class dbScene;

		// private constructor
		dbCamera(const std::string id);

		core::dbMatrix view_matrix_;
		/*
		core::dbMatrix mat_camera_;
		core::dbMatrix mat_rotationx_;
		core::dbMatrix mat_rotationy_;
		core::dbMatrix mat_rotationz_;
		*/

		void SetCameraView(float fElapsedTime);

		DISALLOW_COPY_AND_ASSIGN(dbCamera);
	};

}}	// namespace

#endif