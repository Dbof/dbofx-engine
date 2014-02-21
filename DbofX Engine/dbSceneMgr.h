#ifndef _SCENE_MGR_H
#define _SCENE_MGR_H

namespace dbx
{
namespace scene
{
	class DBX_API dbSceneMgr
	{
	public:
		static dbSceneMgr* GetInstance();
		dbScene* AddScene(dbScene* newscene);
	
		void Exit();	// for cleanup
		void OnLostDevice();
		void OnResetDevice();
	private:
		dbSceneMgr(void);
		~dbSceneMgr(void);

		static dbSceneMgr* instance_;
		std::unordered_map<std::string, dbScene*> scenes_map_;
	};

}}	// namespace
#endif