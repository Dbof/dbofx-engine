#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

namespace dbx
{
namespace scene
{
	class DBX_API dbLightManager
	{
	public:
		static dbLightManager* GetInstance();
		void Exit();
		
		void OnLostDevice();
		void OnResetDevice();
	private:
		friend class dbLight;
		friend class dbScene;

		dbLightManager(void);
		~dbLightManager(void) { Exit(); }

		// init methods with overloaded method
		dbSpotLight*				InitSpot(const std::string id, core::dbVector3 pos, core::dbVector3 direction, float range, core::dbColor Diffuse, core::dbColor Specular, core::dbColor Ambient, float angleInCircle, float angleOutCircle);
		dbSpotLight*				InitSpot(const std::string id, core::dbVector3 pos, core::dbVector3 direction, float range, core::dbColor Diffuse, float angleInCircle, float angleOutCircle);
		dbPointLight*				InitPoint(const std::string id, core::dbVector3 pos, float range, core::dbColor Diffuse, core::dbColor Specular, core::dbColor Ambient);
		dbPointLight*				InitPoint(const std::string id, core::dbVector3 pos, float range, core::dbColor Diffuse);
		dbDirectionalLight*			InitDirectional(const std::string id, core::dbVector3 direction, core::dbColor Diffuse, core::dbColor Specular, core::dbColor Ambient);
		dbDirectionalLight*			InitDirectional(const std::string id, core::dbVector3 direction, core::dbColor Diffuse);

		bool						SetVisible(dbLight* l1, bool state);
		bool						AddLight(dbLight* newlight);
		dbLight*					GetLight(const std::string id);

		int							max_light_count_;
		
		std::unordered_map<std::string, dbLight*> lights_map_;
		std::list<int>				active_lights_list_;
		static dbLightManager*		instance_;
	};

}}	// namespace

#endif