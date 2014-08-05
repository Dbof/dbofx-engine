#ifndef SCENE_H
#define SCENE_H

namespace dbx
{
namespace scene
{
	enum NODE_TYPE { NODE_STANDARD, NODE_MESH, NODE_LIGHT, NODE_SPRITES };

	class DBX_API dbScene
	{
	public:
		friend class dbDevice;

		// Lightning methods
		dbPointLight* AddPointLight(const std::string& identifier, core::dbVector3 pos, float range, core::dbColor Diffuse, core::dbColor Specular = core::dbColor(1.f), core::dbColor Ambient = core::dbColor(1.f));
		dbDirectionalLight* AddDirectionalLight(const std::string& identifier, core::dbVector3 direction, core::dbColor Diffuse, core::dbColor Specular = core::dbColor(1.f), core::dbColor Ambient = core::dbColor(1.f));
		dbSpotLight*		AddSpotLight(const std::string& identifier, core::dbVector3 pos, core::dbVector3 direction, float range, float angleInnerCircle, float angleOuterCircle, core::dbColor Diffuse, core::dbColor Specular = core::dbColor(1.f), core::dbColor Ambient = core::dbColor(1.f));
		dbLight*			GetLight(const std::string& id);

		// Camera methods
		dbCamera*			AddCamera(const std::string identifier, core::dbVector3 position, core::dbVector3 direction);
		dbCamera*			GetCamera();
		dbCamera*			SetCamera(dbCamera* cam);

		// Scene Node methods
		dbSceneNode*		AddSceneNode(const std::string& identifier);
		dbSceneNode*		GetSceneNode(const std::string& identifier);

		// dbSkybox methods
		dbSkybox*			AddSkybox(const std::string& filename);
		dbSkybox*			GetSkybox();

		// screenshot function
		void				SaveScreenshot(const std::string& filename);

		// GET functions
		dbMesh*				AddMesh(const std::string& identifier, const std::string file);
		dbMesh*				AddTeapot(const std::string& identifier);
		dbMesh*				GetMesh(const std::string& name);

		std::string			GetId()	{ return identifier_; }

		void Release();
		void OnLostDevice();
		void OnResetDevice();

	private:
		dbScene(const std::string& identifier);
		~dbScene() { Release(); }

		//private function to apply camera effects
		static void ApplyCamera();

		// members
		std::string identifier_;

		// scene node
		std::unordered_map<std::string, dbSceneNode*> scene_nodes_map_;

		// meshes
		std::unordered_map<std::string, dbMesh*> meshes_map_;

		// Camera variables
		dbCamera* active_camera_;

		// skybox
		dbSkybox* active_skybox_;

		// sprites
//		static vector< dbSprite* > sprites;
	};

}}	// namespace
#endif