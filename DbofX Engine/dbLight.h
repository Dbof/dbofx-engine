#ifndef _LIGHT_H
#define _LIGHT_H

#define setupDirectional(direction, range, color) setupDirectionalEx(direction, range, color, color, color)

namespace dbx
{
namespace scene
{
	class DBX_API dbLight : public dbSceneNode
	{
	public:
		void SetPos(core::dbVector3 newPos);
		void SetDirection(core::dbVector3 newDir);
		void Move(core::dbVector3 direction, float units);
		void MoveUp(float units);
		void MoveRight(float units);
		void MoveForward(float units);

		void RotateX(float angle, bool relative = false);
		void RotateY(float angle, bool relative = false);
		void RotateZ(float angle, bool relative = false);

		void RotateAroundX(core::dbVector3 position, float angle);
		void RotateAroundY(core::dbVector3 position, float angle);
		void RotateAroundZ(core::dbVector3 position, float angle);
		void RotateAroundXYZ(core::dbVector3 position, core::dbVector3 axis, float angle);

		void LookAt(core::dbVector3 target);
		
		void SetColor(core::dbColor Diffuse, core::dbColor Specular, core::dbColor Ambient);
		void SetDiffuseColor(core::dbColor newcolor);
		void SetSpecularColor(core::dbColor newcolor);		
		void SetAmbientColor(core::dbColor newcolor);

		bool SetVisible(bool enable);
		bool IsVisible();
		int GetIndex()	{ return d3d_index_; }

	protected:
		friend class dbLightManager;

		dbLight(const std::string id);
		~dbLight(void);

		virtual bool InitLight();
		void Update();

		bool is_init_;
		int	d3d_index_;
		D3DLIGHT9 d3d_light_;
		bool visible_;
	};

}}	// namespace
#endif